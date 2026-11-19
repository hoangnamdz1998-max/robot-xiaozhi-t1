# HƯỚNG DẪN DỰ ÁN ROBOT XE DÂY ĐỒNG XIAOZHI AI (ESP32-S3 N16R8)

Dự án này tích hợp trợ lý ảo AI **Xiaozhi (Tiểu Trí)** vào mô hình **Robot Xe Dây Đồng** dùng vi điều khiển **ESP32-S3 (N16R8)**, điều khiển 2 động cơ qua mạch cầu H **DRV8833**, hiển thị biểu cảm qua **OLED SSD1306**, thu âm bằng **INMP441**, phát âm bằng **MAX98357A**, và **đã loại bỏ phần thẻ nhớ MicroSD** theo yêu cầu.

---

## 1. SƠ ĐỒ CHÂN KẾT NỐI (PINOUT CHUẨN)

### A. Âm thanh I2S (Mic & Loa)
| Ngoại vi | Chân Module | Chân ESP32-S3 | Chú thích |
|---|---|---|---|
| **Mic INMP441** | **WS** | **GPIO 4** | Word Select (LRC) |
| | **SCK** | **GPIO 5** | Clock (BCLK) |
| | **SD** | **GPIO 6** | Data Out của Mic |
| | **VDD / GND** | **3V3 / GND** | Nối nguồn 3.3V |
| | **L/R** | **GND** | Chọn kênh Trái |
| **Loa MAX98357A** | **DIN** | **GPIO 7** | Data In của Loa |
| | **BCLK** | **GPIO 15** | Bit Clock |
| | **LRC** | **GPIO 16** | Left/Right Clock |
| | **VIN / GND** | **5V (hoặc 3V3) / GND** | Nguồn công suất |

### B. Màn hình OLED SSD1306 (128x64 I2C) & Cảm biến TOF
| Ngoại vi | Chân Module | Chân ESP32-S3 | Chú thích |
|---|---|---|---|
| **OLED SSD1306** | **SDA** | **GPIO 41** | I2C Data |
| | **SCL** | **GPIO 42** | I2C Clock |
| | **VCC / GND** | **3V3 / GND** | Nguồn 3.3V |
| **Cảm biến TOF** | **SDA / SCL** | **GPIO 41 / 42** | Nối song song chung bus I2C với OLED |

### C. Điều khiển động cơ DRV8833
| Ngoại vi | Chân DRV8833 | Chân ESP32-S3 | Chú thích |
|---|---|---|---|
| **Motor Trái** | **IN1** | **GPIO 1** | Chiều tiến bánh trái |
| | **IN2** | **GPIO 2** | Chiều lùi bánh trái |
| **Motor Phải** | **IN3** | **GPIO 40** | Chiều tiến bánh phải |
| | **IN4** | **GPIO 39** | Chiều lùi bánh phải |
| **Nguồn DRV8833**| **VCC / GND** | **Pin 3.7V - 5V / GND** | Chung Mass với ESP32-S3 |
| | **EEP / SLP** | **3V3** | Kéo lên 3.3V để bật mạch (không để lơ lửng) |

### D. Đèn LED & Nút bấm
* **Đèn LED mắt:** Chân **GPIO 47** (hoặc GPIO 48).
* **Nút BOOT:** **GPIO 0** (chuyển trạng thái chat / nhấn giữ reset Wi-Fi).
* **Thẻ nhớ MicroSD:** **ĐÃ LOẠI BỎ HOÀN TOÀN** (các chân GPIO 12, 13, 14 để trống).

---

## 2. DANH SÁCH LỆNH GIỌNG NÓI ĐIỀU KHIỂN ROBOT (MCP TOOLS)

Nhờ tích hợp công cụ **Model Context Protocol (MCP)** trong file [motor_controller.h](file:///d:/HNam/CODE/robot%20T1/xiaozhi-esp32_vietnam/main/boards/robot-daydong-s3/motor_controller.h), khi bạn trò chuyện với Xiaozhi, AI sẽ tự động phân tích và kích hoạt động cơ:

| Câu lệnh ví dụ của bạn | Công cụ MCP được gọi | Hành động của Robot |
|---|---|---|
| *"Robot tiến lên", "Đi thẳng nào"* | `self.robot.move_forward` | 2 bánh chạy tiến |
| *"Lùi lại", "Đi lùi"* | `self.robot.move_backward` | 2 bánh chạy lùi |
| *"Rẽ trái", "Quay sang trái"* | `self.robot.turn_left` | Bánh trái lùi, bánh phải tiến |
| *"Rẽ phải", "Quay sang phải"* | `self.robot.turn_right` | Bánh phải lùi, bánh trái tiến |
| *"Dừng lại", "Đứng yên"* | `self.robot.stop` | Ngắt toàn bộ động cơ |
| *"Nhảy múa đi", "Lắc lư nào"* | `self.robot.dance` | Lắc sang trái phải rồi dừng |
| *"Bật đèn", "Mở đèn mắt"* | `self.robot.light_on` | Bật LED GPIO 47 |
| *"Tắt đèn đi"* | `self.robot.light_off` | Tắt LED GPIO 47 |

---

## 3. CÁCH NẠP NHANH BẰNG WEB (KHÔNG CẦN CÀI MÔI TRƯỜNG BUILD)
Nếu bạn muốn dùng ngay lập tức:
1. Vào trang web `https://xiaozhi.vn` (mục **Nạp firmware**).
2. Tìm ô firmware **"Xe dây đồng S3 SUPPER MINI"** (hàng 2 ô thứ 2 trong ảnh bạn chụp).
3. Cắm cáp Type-C nối máy tính với cổng **USB** của bo ESP32-S3.
4. Bấm nút **"Nạp FW"** trên trình duyệt (Chrome / Edge), chọn cổng COM của ESP32-S3 và chờ nạp 100%.
5. Sau khi nạp xong, dùng điện thoại kết nối vào WiFi AP do chip phát ra để cài đặt WiFi nhà bạn.

---

## 4. CÁCH TỰ BUILD MÃ NGUỒN (NẾU MUỐN TÙY BIẾN SÂU)
Thư mục board mới đã được tạo sẵn tại:
`xiaozhi-esp32_vietnam/main/boards/robot-daydong-s3`

Để build bằng ESP-IDF:
```bash
cd xiaozhi-esp32_vietnam
idf.py set-target esp32s3
idf.py menuconfig   # Chọn Board Type: ROBOT DAY DONG S3
idf.py build
idf.py -p COMx flash monitor
```

---

## 5. BO ESP32 PHỤ (SOUNDCARD BLUETOOTH A2DP)
Mã nguồn Arduino hoàn chỉnh nằm trong thư mục:
[esp32_bluetooth_soundcard.ino](file:///d:/HNam/CODE/robot%20T1/esp32_bluetooth_soundcard/esp32_bluetooth_soundcard.ino)
* Mở bằng **Arduino IDE**, cài thư viện `ESP32-A2DP`.
* Chọn board **ESP32 Dev Module** và nạp vào bo ESP32 thường.
* Robot sẽ xuất hiện dưới tên Bluetooth: **`Robot-AI-Speaker`**.
