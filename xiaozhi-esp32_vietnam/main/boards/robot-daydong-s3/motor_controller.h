#ifndef __MOTOR_CONTROLLER_H__
#define __MOTOR_CONTROLLER_H__

#include <driver/gpio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "mcp_server.h"
#include "settings.h"

#define MOTOR_TAG "DRV8833"

class MotorController {
private:
    gpio_num_t in1_, in2_, in3_, in4_;
    bool invert_left_ = false;
    bool invert_right_ = false;
    bool swap_lr_ = false;

    void Drive(int left_dir, int right_dir) {
        // left_dir:  1 (tiến), -1 (lùi), 0 (dừng)
        // right_dir: 1 (tiến), -1 (lùi), 0 (dừng)
        if (invert_left_) left_dir = -left_dir;
        if (invert_right_) right_dir = -right_dir;

        int l_1 = 0, l_2 = 0;
        int r_3 = 0, r_4 = 0;

        if (left_dir > 0) { l_1 = 1; l_2 = 0; }
        else if (left_dir < 0) { l_1 = 0; l_2 = 1; }

        if (right_dir > 0) { r_3 = 1; r_4 = 0; }
        else if (right_dir < 0) { r_3 = 0; r_4 = 1; }

        if (swap_lr_) {
            gpio_set_level(in1_, r_3);
            gpio_set_level(in2_, r_4);
            gpio_set_level(in3_, l_1);
            gpio_set_level(in4_, l_2);
        } else {
            gpio_set_level(in1_, l_1);
            gpio_set_level(in2_, l_2);
            gpio_set_level(in3_, r_3);
            gpio_set_level(in4_, r_4);
        }
    }

public:
    MotorController(gpio_num_t in1, gpio_num_t in2, gpio_num_t in3, gpio_num_t in4)
        : in1_(in1), in2_(in2), in3_(in3), in4_(in4) {

        gpio_config_t config = {
            .pin_bit_mask = (1ULL << in1_) | (1ULL << in2_) | (1ULL << in3_) | (1ULL << in4_),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_ENABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ESP_ERROR_CHECK(gpio_config(&config));

        LoadSettings();
        Stop();

        ESP_LOGI(MOTOR_TAG, "DRV8833 initialized (IN1:%d, IN2:%d, IN3:%d, IN4:%d | InvL:%d, InvR:%d, Swap:%d)",
                 in1_, in2_, in3_, in4_, invert_left_, invert_right_, swap_lr_);

        RegisterMcpTools();
    }

    void LoadSettings() {
        Settings settings("robot_cfg", false);
        invert_left_ = (settings.GetInt("inv_l", 0) == 1);
        invert_right_ = (settings.GetInt("inv_r", 0) == 1);
        swap_lr_ = (settings.GetInt("swap_lr", 0) == 1);
    }

    void Stop() {
        Drive(0, 0);
        ESP_LOGI(MOTOR_TAG, "Motors stopped");
    }

    void Forward() {
        Drive(1, 1);
        ESP_LOGI(MOTOR_TAG, "Motors forward");
    }

    void Backward() {
        Drive(-1, -1);
        ESP_LOGI(MOTOR_TAG, "Motors backward");
    }

    void TurnLeft() {
        Drive(-1, 1);
        ESP_LOGI(MOTOR_TAG, "Motors turn left");
    }

    void TurnRight() {
        Drive(1, -1);
        ESP_LOGI(MOTOR_TAG, "Motors turn right");
    }

    void Dance() {
        ESP_LOGI(MOTOR_TAG, "Robot dancing...");
        for (int i = 0; i < 2; i++) {
            TurnLeft();
            vTaskDelay(pdMS_TO_TICKS(300));
            TurnRight();
            vTaskDelay(pdMS_TO_TICKS(300));
        }
        Stop();
    }

    void RegisterMcpTools() {
        auto& mcp = McpServer::GetInstance();

        // 1. Tiến tới
        mcp.AddTool("self.robot.move_forward", "Điều khiển robot xe di chuyển tiến về phía trước", PropertyList({
            Property("duration_ms", kPropertyTypeInteger, 500, 5000)
        }), [this](const PropertyList& properties) -> ReturnValue {
            int duration = properties.count("duration_ms") ? properties["duration_ms"].value<int>() : 1500;
            Forward();
            vTaskDelay(pdMS_TO_TICKS(duration));
            Stop();
            return true;
        });

        // 2. Lùi lại
        mcp.AddTool("self.robot.move_backward", "Điều khiển robot xe di chuyển lùi lại phía sau", PropertyList({
            Property("duration_ms", kPropertyTypeInteger, 500, 5000)
        }), [this](const PropertyList& properties) -> ReturnValue {
            int duration = properties.count("duration_ms") ? properties["duration_ms"].value<int>() : 1500;
            Backward();
            vTaskDelay(pdMS_TO_TICKS(duration));
            Stop();
            return true;
        });

        // 3. Rẽ trái
        mcp.AddTool("self.robot.turn_left", "Điều khiển robot xe rẽ sang bên trái", PropertyList({
            Property("duration_ms", kPropertyTypeInteger, 200, 3000)
        }), [this](const PropertyList& properties) -> ReturnValue {
            int duration = properties.count("duration_ms") ? properties["duration_ms"].value<int>() : 600;
            TurnLeft();
            vTaskDelay(pdMS_TO_TICKS(duration));
            Stop();
            return true;
        });

        // 4. Rẽ phải
        mcp.AddTool("self.robot.turn_right", "Điều khiển robot xe rẽ sang bên phải", PropertyList({
            Property("duration_ms", kPropertyTypeInteger, 200, 3000)
        }), [this](const PropertyList& properties) -> ReturnValue {
            int duration = properties.count("duration_ms") ? properties["duration_ms"].value<int>() : 600;
            TurnRight();
            vTaskDelay(pdMS_TO_TICKS(duration));
            Stop();
            return true;
        });

        // 5. Dừng xe
        mcp.AddTool("self.robot.stop", "Dừng ngay lập tức chuyển động của robot", PropertyList(), [this](const PropertyList& properties) -> ReturnValue {
            Stop();
            return true;
        });

        // 6. Nhảy múa / Biểu diễn
        mcp.AddTool("self.robot.dance", "Cho robot biểu diễn xoay tròn hoặc nhảy múa vui vẻ", PropertyList(), [this](const PropertyList& properties) -> ReturnValue {
            Dance();
            return true;
        });

        // 7. Lệnh giọng nói đảo chiều bánh xe
        mcp.AddTool("self.robot.invert_left_motor", "Đảo chiều quay của động cơ bánh bên trái", PropertyList(), [this](const PropertyList& properties) -> ReturnValue {
            invert_left_ = !invert_left_;
            Settings settings("robot_cfg", true);
            settings.SetInt("inv_l", invert_left_ ? 1 : 0);
            return invert_left_ ? "Đã đảo chiều bánh trái" : "Đã đặt lại chiều chuẩn bánh trái";
        });

        mcp.AddTool("self.robot.invert_right_motor", "Đảo chiều quay của động cơ bánh bên phải", PropertyList(), [this](const PropertyList& properties) -> ReturnValue {
            invert_right_ = !invert_right_;
            Settings settings("robot_cfg", true);
            settings.SetInt("inv_r", invert_right_ ? 1 : 0);
            return invert_right_ ? "Đã đảo chiều bánh phải" : "Đã đặt lại chiều chuẩn bánh phải";
        });
    }
};

#endif // __MOTOR_CONTROLLER_H__
