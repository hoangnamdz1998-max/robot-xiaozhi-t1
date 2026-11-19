#ifndef __ROBOT_CONFIG_SERVER_H__
#define __ROBOT_CONFIG_SERVER_H__

#include <esp_http_server.h>
#include <esp_log.h>
#include <cstring>
#include "settings.h"
#include "motor_controller.h"

#define WEB_TAG "RobotWebConfig"

class RobotConfigServer {
private:
    httpd_handle_t server_ = nullptr;
    MotorController* motor_ = nullptr;

    static esp_err_t IndexHandler(httpd_req_t *req) {
        Settings settings("robot_cfg", false);
        int inv_l = settings.GetInt("inv_l", 0);
        int inv_r = settings.GetInt("inv_r", 0);
        int swap_lr = settings.GetInt("swap_lr", 0);
        int screen_type = settings.GetInt("screen_type", 0); // 0: 0.96", 1: 1.3"

        char resp[3072];
        snprintf(resp, sizeof(resp),
            "<!DOCTYPE html><html><head><meta charset='utf-8'>"
            "<meta name='viewport' content='width=device-width,initial-scale=1'>"
            "<title>Cấu Hình Robot Xiaozhi T1</title>"
            "<style>"
            "body{font-family:system-ui,-apple-system,sans-serif;background:#121826;color:#e2e8f0;padding:20px;max-width:480px;margin:auto}"
            "h2{color:#38bdf8;text-align:center;margin-bottom:8px}"
            ".card{background:#1e293b;border-radius:12px;padding:16px;margin-bottom:16px;box-shadow:0 4px 6px -1px rgba(0,0,0,0.3)}"
            "h3{color:#94a3b8;margin-top:0;font-size:16px;border-bottom:1px solid #334155;padding-bottom:8px}"
            ".row{display:flex;justify-content:space-between;align-items:center;margin:12px 0}"
            "label{font-size:15px}"
            "select,input[type=checkbox]{accent-color:#0284c7;cursor:pointer}"
            "select{background:#0f172a;color:#f8fafc;padding:8px 12px;border:1px solid #475569;border-radius:8px}"
            ".btn-grid{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-top:12px}"
            "button{background:#0284c7;color:#fff;border:none;border-radius:8px;padding:10px;font-weight:600;cursor:pointer;transition:background .2s}"
            "button:hover{background:#0369a1}"
            ".btn-stop{background:#ef4444}.btn-stop:hover{background:#dc2626}"
            ".btn-save{background:#10b981;width:100%%;padding:12px;font-size:16px}.btn-save:hover{background:#059669}"
            ".badge{background:#0369a1;color:#bae6fd;padding:2px 8px;border-radius:999px;font-size:12px}"
            "</style></head><body>"
            "<h2>🤖 Robot Xiaozhi T1</h2>"
            "<div class='card'><h3>⚙️ Tùy Chỉnh Động Cơ (Hàn Dây Ngược)</h3>"
            "<form action='/save' method='POST'>"
            "<div class='row'><label>Đảo chiều Bánh Trái</label><input type='checkbox' name='inv_l' value='1' %s></div>"
            "<div class='row'><label>Đảo chiều Bánh Phải</label><input type='checkbox' name='inv_r' value='1' %s></div>"
            "<div class='row'><label>Đổi vị trí Bánh Trái &harr; Phải</label><input type='checkbox' name='swap_lr' value='1' %s></div>"
            "<h3>📺 Tùy Chỉnh Màn Hình OLED</h3>"
            "<div class='row'><label>Loại màn hình OLED:</label>"
            "<select name='screen_type'>"
            "<option value='0' %s>Màn hình 0.96 inch (SSD1306)</option>"
            "<option value='1' %s>Màn hình 1.3 inch (SH1106 / Offset 2px)</option>"
            "</select></div>"
            "<button type='submit' class='btn-save'>💾 LƯU CẤU HÌNH & KHỞI ĐỘNG LẠI</button>"
            "</form></div>"
            "<div class='card'><h3>🎮 Chạy Thử Bánh Xe (Test Động Cơ)</h3>"
            "<div class='btn-grid'>"
            "<button onclick=\"fetch('/test?cmd=fw')\">⬆️ Tiến Tới</button>"
            "<button onclick=\"fetch('/test?cmd=bw')\">⬇️ Lùi Lại</button>"
            "<button onclick=\"fetch('/test?cmd=left')\">⬅️ Cua Trái</button>"
            "<button onclick=\"fetch('/test?cmd=right')\">➡️ Cua Phải</button>"
            "</div><button class='btn-stop' style='width:100%%;margin-top:8px' onclick=\"fetch('/test?cmd=stop')\">⏹️ DỪNG LẠI</button>"
            "</div></body></html>",
            inv_l ? "checked" : "",
            inv_r ? "checked" : "",
            swap_lr ? "checked" : "",
            screen_type == 0 ? "selected" : "",
            screen_type == 1 ? "selected" : ""
        );

        httpd_resp_set_type(req, "text/html; charset=utf-8");
        return httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    }

    static esp_err_t SaveHandler(httpd_req_t *req) {
        char buf[256];
        int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
        if (ret <= 0) {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        buf[ret] = '\0';

        bool inv_l = (strstr(buf, "inv_l=1") != nullptr);
        bool inv_r = (strstr(buf, "inv_r=1") != nullptr);
        bool swap_lr = (strstr(buf, "swap_lr=1") != nullptr);
        int screen_type = 0;
        char *p_screen = strstr(buf, "screen_type=");
        if (p_screen) {
            screen_type = atoi(p_screen + 12);
        }

        Settings settings("robot_cfg", true);
        settings.SetInt("inv_l", inv_l ? 1 : 0);
        settings.SetInt("inv_r", inv_r ? 1 : 0);
        settings.SetInt("swap_lr", swap_lr ? 1 : 0);
        settings.SetInt("screen_type", screen_type);

        ESP_LOGI(WEB_TAG, "Saved settings: inv_l=%d, inv_r=%d, swap_lr=%d, screen_type=%d",
                 inv_l, inv_r, swap_lr, screen_type);

        const char *resp = "<!DOCTYPE html><html><head><meta charset='utf-8'>"
                           "<meta http-equiv='refresh' content='3;url=/'></head>"
                           "<body style='font-family:sans-serif;background:#121826;color:#e2e8f0;text-align:center;padding:50px'>"
                           "<h2>✅ Đã lưu cấu hình thành công!</h2>"
                           "<p>Đang khởi động lại robot để áp dụng...</p></body></html>";
        httpd_resp_set_type(req, "text/html; charset=utf-8");
        httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

        vTaskDelay(pdMS_TO_TICKS(1500));
        esp_restart();
        return ESP_OK;
    }

    static esp_err_t TestHandler(httpd_req_t *req) {
        auto self = (RobotConfigServer*)req->user_ctx;
        char query[64];
        if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
            char cmd[16] = {0};
            if (httpd_query_key_value(query, "cmd", cmd, sizeof(cmd)) == ESP_OK) {
                if (self && self->motor_) {
                    if (strcmp(cmd, "fw") == 0) self->motor_->Forward();
                    else if (strcmp(cmd, "bw") == 0) self->motor_->Backward();
                    else if (strcmp(cmd, "left") == 0) self->motor_->TurnLeft();
                    else if (strcmp(cmd, "right") == 0) self->motor_->TurnRight();
                    else if (strcmp(cmd, "stop") == 0) self->motor_->Stop();
                }
            }
        }
        httpd_resp_set_type(req, "text/plain");
        return httpd_resp_send(req, "OK", 2);
    }

public:
    RobotConfigServer(MotorController* motor) : motor_(motor) {}

    void Start(uint16_t port = 8080) {
        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.server_port = port;
        config.ctrl_port = port + 1;
        config.stack_size = 8192;

        if (httpd_start(&server_, &config) == ESP_OK) {
            httpd_uri_t uri_index = {
                .uri = "/",
                .method = HTTP_GET,
                .handler = IndexHandler,
                .user_ctx = this
            };
            httpd_register_uri_handler(server_, &uri_index);

            httpd_uri_t uri_save = {
                .uri = "/save",
                .method = HTTP_POST,
                .handler = SaveHandler,
                .user_ctx = this
            };
            httpd_register_uri_handler(server_, &uri_save);

            httpd_uri_t uri_test = {
                .uri = "/test",
                .method = HTTP_GET,
                .handler = TestHandler,
                .user_ctx = this
            };
            httpd_register_uri_handler(server_, &uri_test);

            ESP_LOGI(WEB_TAG, "Robot Web Config Server started on port %d", port);
        } else {
            ESP_LOGE(WEB_TAG, "Failed to start Robot Web Config Server on port %d", port);
        }
    }
};

#endif // __ROBOT_CONFIG_SERVER_H__
