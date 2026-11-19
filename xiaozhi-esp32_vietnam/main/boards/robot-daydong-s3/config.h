#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

// ===== ÂM THANH I2S (MIC INMP441 + DAC MAX98357A) =====
#define AUDIO_INPUT_SAMPLE_RATE  16000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000
#define AUDIO_I2S_METHOD_SIMPLEX

// Mic INMP441
#define AUDIO_I2S_MIC_GPIO_WS   GPIO_NUM_4
#define AUDIO_I2S_MIC_GPIO_SCK  GPIO_NUM_5
#define AUDIO_I2S_MIC_GPIO_DIN  GPIO_NUM_6

// Loa MAX98357A
#define AUDIO_I2S_SPK_GPIO_DOUT GPIO_NUM_7
#define AUDIO_I2S_SPK_GPIO_BCLK GPIO_NUM_15
#define AUDIO_I2S_SPK_GPIO_LRCK GPIO_NUM_16

// ===== MÀN HÌNH OLED SSD1306 (128x64 I2C) & CẢM BIẾN TOF =====
#define DISPLAY_SDA_PIN         GPIO_NUM_41
#define DISPLAY_SCL_PIN         GPIO_NUM_42
#define DISPLAY_WIDTH           128
#define DISPLAY_HEIGHT          64
#define DISPLAY_MIRROR_X        true
#define DISPLAY_MIRROR_Y        true

// ===== ĐIỀU KHIỂN ĐỘNG CƠ DRV8833 (XE DÂY ĐỒNG) =====
#define MOTOR_LEFT_IN1_PIN      GPIO_NUM_1
#define MOTOR_LEFT_IN2_PIN      GPIO_NUM_2
#define MOTOR_RIGHT_IN3_PIN     GPIO_NUM_40
#define MOTOR_RIGHT_IN4_PIN     GPIO_NUM_39

// ===== ĐÈN LED VÀ NÚT BẤM =====
#define BUILTIN_LED_GPIO        GPIO_NUM_48
#define EYE_LED_GPIO            GPIO_NUM_47
#define BOOT_BUTTON_GPIO        GPIO_NUM_0

// LƯU Ý: Phần thẻ nhớ MicroSD (GPIO 12, 13, 14) đã được LOẠI BỎ hoàn toàn.

#endif // _BOARD_CONFIG_H_
