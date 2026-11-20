/*
 * =========================================================================================
 * ROBOT XIAOZHI T1 - BỘ PHÁT BLUETOOTH TỰ ĐỘNG KẾT NỐI LOA NGOÀI (A2DP SOURCE)
 * =========================================================================================
 * Nạp cho bo ESP32 thường (NodeMCU-32S / ESP-WROOM-32).
 * 
 * CHỨC NĂNG:
 * - Nhận toàn bộ âm thanh (giọng nói AI Xiaozhi + Nhạc Online) từ ESP32-S3 qua cổng I2S.
 * - Tự động tìm và kết nối không dây với LOA BLUETOOTH NGOÀI (JBL, Sony, Loa kéo, Loa Mini...).
 * - Toàn bộ âm thanh phát ra loa ngoài siêu to và sống động, không cần dùng loa nhỏ trên robot!
 * 
 * SƠ ĐỒ NỐI DÂY (ESP32-S3 sang ESP32 Thường):
 * - ESP32-S3 GPIO 15 (BCLK) ----> ESP32 GPIO 26 (I2S SCK)
 * - ESP32-S3 GPIO 16 (LRC)  ----> ESP32 GPIO 25 (I2S WS)
 * - ESP32-S3 GPIO 7  (DOUT) ----> ESP32 GPIO 22 (I2S SD IN)
 * - Nối chung GND giữa 2 bo mạch!
 * 
 * THƯ VIỆN CẦN THIẾT TRÊN ARDUINO IDE:
 * 1. "ESP32-A2DP" by Phil Schatzmann (Cài trong Library Manager hoặc tải github.com/pschatzmann/ESP32-A2DP)
 * =========================================================================================
 */

#include "BluetoothA2DPSource.h"
#include <driver/i2s.h>

// ===== CẤU HÌNH TÊN LOA BLUETOOTH NGOÀI =====
// Điền tên chính xác của loa Bluetooth của bạn (ví dụ: "JBL Flip 5", "Sony SRS-XB12", "TG117"...)
// Nếu để rỗng "", ESP32 sẽ tự động kết nối với loa Bluetooth có sóng mạnh nhất gần nó!
const char* TARGET_SPEAKER_NAME = "JBL GO 3"; // <-- ĐỔI TÊN LOA CỦA BẠN VÀO ĐÂY

// Cấu hình chân I2S nhận từ ESP32-S3
#define I2S_SCK_PIN   26
#define I2S_WS_PIN    25
#define I2S_SD_PIN    22

BluetoothA2DPSource a2dp_source;

// Callback đọc dữ liệu âm thanh từ ESP32-S3 và chuyển tiếp sang Loa Bluetooth
int32_t get_i2s_data(Frame *frame, int32_t frame_count) {
    size_t bytes_read = 0;
    i2s_read(I2S_NUM_0, (void*)frame, frame_count * sizeof(Frame), &bytes_read, portMAX_DELAY);
    return bytes_read / sizeof(Frame);
}

// Hàm khởi tạo I2S ở chế độ SLAVE RECEIVER (Lắng nghe từ ESP32-S3)
void setup_i2s() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_RX),
        .sample_rate = 24000, // Tốc độ mẫu chuẩn của Xiaozhi AI
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 256,
        .use_apll = false
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK_PIN,
        .ws_io_num = I2S_WS_PIN,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_SD_PIN
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("=========================================");
    Serial.println("ROBOT XIAOZHI T1 - BLUETOOTH TRANSMITTER");
    Serial.println("=========================================");

    // 1. Khởi tạo cổng nhận âm thanh I2S
    setup_i2s();
    Serial.println("-> Da khoi dong I2S Slave thanh cong!");

    // 2. Cài đặt phát Bluetooth
    a2dp_source.set_data_callback_in_frames(get_i2s_data);

    // 3. Bắt đầu tìm kiếm và kết nối loa ngoài
    if (strlen(TARGET_SPEAKER_NAME) > 0) {
        Serial.printf("-> Dang tim va ket noi voi loa: %s ...\n", TARGET_SPEAKER_NAME);
        a2dp_source.start(TARGET_SPEAKER_NAME);
    } else {
        Serial.println("-> Dang tu dong tim kiem loa Bluetooth gan nhat...");
        a2dp_source.start(); // Tự động dò tìm
    }
}

void loop() {
    // Duy trì hoạt động kết nối
    delay(1000);
    if (a2dp_source.is_connected()) {
        static bool connected_printed = false;
        if (!connected_printed) {
            Serial.println(">>> DA KET NOI THANH CONG VOI LOA BLUETOOTH! Âm thanh da san sang!");
            connected_printed = true;
        }
    }
}
