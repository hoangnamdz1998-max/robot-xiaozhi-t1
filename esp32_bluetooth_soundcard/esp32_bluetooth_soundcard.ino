/*
 * ESP32 Bluetooth Sound Card cho Robot Xiaozhi
 * Nhận nhạc Bluetooth A2DP từ điện thoại và xuất qua DAC I2S MAX98357A
 * Giao tiếp UART với ESP32-S3 (TX2 = GPIO17, RX2 = GPIO16)
 *
 * Yêu cầu thư viện Arduino:
 * ESP32-A2DP (by Phil Schatzmann): https://github.com/pschatzmann/ESP32-A2DP
 */

#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

// Cấu hình chân I2S xuất sang MAX98357A
#define I2S_BCLK_PIN    26
#define I2S_LRC_PIN     25
#define I2S_DOUT_PIN    22

// Cấu hình Serial UART nối với ESP32-S3
#define RXD2 16
#define TXD2 17

BluetoothA2DPSink a2dp_sink;

void avrc_metadata_callback(uint8_t data1, const uint8_t *data2) {
    // Gửi thông tin bài hát sang ESP32-S3 nếu muốn hiển thị tên bài hát lên màn hình OLED
    Serial2.printf("[BT_META] %s\n", (char*)data2);
}

void setup() {
    Serial.begin(115200);
    // Giao tiếp UART2 với ESP32-S3
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

    Serial.println("Khoi dong ESP32 Bluetooth Audio Sink...");

    // Cấu hình chân I2S cho MAX98357A
    i2s_pin_config_t my_pin_config = {
        .bck_io_num = I2S_BCLK_PIN,
        .ws_io_num = I2S_LRC_PIN,
        .data_out_num = I2S_DOUT_PIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    a2dp_sink.set_pin_config(my_pin_config);
    a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);

    // Tên thiết bị Bluetooth hiển thị trên điện thoại
    a2dp_sink.start("Robot-Xiaozhi-T1");

    Serial.println("San sang ket noi Bluetooth tu dien thoai!");
}

void loop() {
    // Lắng nghe lệnh từ ESP32-S3 qua Serial2
    if (Serial2.available()) {
        String cmd = Serial2.readStringUntil('\n');
        cmd.trim();
        Serial.print("Lenh tu S3: ");
        Serial.println(cmd);

        if (cmd == "PAUSE") {
            a2dp_sink.pause();
        } else if (cmd == "PLAY") {
            a2dp_sink.play();
        } else if (cmd == "NEXT") {
            a2dp_sink.next();
        } else if (cmd == "PREV") {
            a2dp_sink.previous();
        }
    }
}
