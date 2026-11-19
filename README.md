# HƯỚNG DẪN DỰ ÁN ROBOT XIAOZHI T1 (ESP32-S3 N16R8)

Dự án này tích hợp trợ lý ảo AI **Xiaozhi (Tiểu Trí)** vào mô hình **Robot Xiaozhi T1** dùng vi điều khiển **ESP32-S3 (N16R8)**, điều khiển 2 động cơ qua mạch cầu H **DRV8833**, hiển thị biểu cảm qua **OLED SSD1306 / SH1106**, thu âm bằng **INMP441**, phát âm bằng **MAX98357A**, và **đã loại bỏ phần thẻ nhớ MicroSD** theo yêu cầu.

Repository GitHub chính thức của bạn:
👉 **https://github.com/hoangnamdz1998-max/robot-xiaozhi-t1**

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
| **OLED SSD1306 / SH1106** | **SDA** | **GPIO 41** | I2C Data |
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

## 2. TRANG WEB CẤU HÌNH ĐẢO CHIỀU ĐỘNG CƠ & CHỌN MÀN HÌNH

Hệ thống tích hợp sẵn một **Web Server cấu hình nội bộ** (chạy tại cổng `8080`):
* Khi robot kết nối Wi-Fi, mở trình duyệt điện thoại hoặc máy tính truy cập:
  `http://<IP_ROBOT>:8080/`
* **Tính năng:**
  1. **Đảo chiều Bánh Trái:** Tích chọn nếu lỡ hàn dây motor trái bị ngược chiều (chạy lùi thay vì tiến).
  2. **Đảo chiều Bánh Phải:** Tích chọn nếu lỡ hàn dây motor phải bị ngược chiều.
  3. **Đổi vị trí Bánh Trái <-> Phải:** Khắc phục trường hợp hàn nhầm kênh động cơ trái phải.
  4. **Chọn loại màn hình OLED:** Chọn `0.96 inch (SSD1306)` hoặc `1.3 inch (SH1106 / offset 2px)`.
  5. **Bàn phím Test động cơ trực tiếp:** Bấm [Tiến], [Lùi], [Cua Trái], [Cua Phải], [Dừng] để kiểm tra ngay chiều quay của bánh xe trước khi lưu.
  6. **Lưu cấu hình:** Tự động lưu vĩnh viễn vào bộ nhớ flash NVS và khởi động lại robot.

> Ngoài ra bạn có thể ra lệnh bằng giọng nói:
> * *"Robot đảo chiều bánh trái"*
> * *"Robot đảo chiều bánh phải"*
> * *"Đổi màn hình 1.3 inch"* hoặc *"Đổi màn hình 0.96 inch"*

---

## 3. DANH SÁCH LỆNH GIỌNG NÓI ĐIỀU KHIỂN ROBOT (MCP TOOLS)

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
| *"Đảo chiều bánh trái"* | `self.robot.invert_left_motor` | Đảo chiều motor trái trong NVS |
| *"Đảo chiều bánh phải"* | `self.robot.invert_right_motor` | Đảo chiều motor phải trong NVS |
| *"Đổi màn hình 1.3 inch"* | `self.robot.set_screen_type` | Cấu hình offset SH1106 1.3" |

---

## 4. HƯỚNG DẪN CLONE SANG MÁY KHÁC ĐỂ TIẾP TỤC CHỈNH SỬA

Khi bạn sang máy tính khác, chỉ cần mở Terminal/Command Prompt và chạy:
```bash
git clone https://github.com/hoangnamdz1998-max/robot-xiaozhi-t1.git
cd robot-xiaozhi-t1
```
Toàn bộ mã nguồn Xiaozhi, cấu hình board `robot-daydong-s3`, web config đảo chiều motor, và code soundcard bluetooth đều có sẵn đầy đủ.

---

## 5. CÁCH NẠP NHANH BẰNG WEB (KHÔNG CẦN CÀI MÔI TRƯỜNG BUILD)
Nếu bạn muốn dùng ngay:
1. Vào trang web `https://xiaozhi.vn` (mục **Nạp firmware**).
2. Tìm ô firmware **"Xe dây đồng S3 SUPPER MINI"** (hàng 2 ô thứ 2).
3. Cắm cáp Type-C nối máy tính với cổng **USB** của bo ESP32-S3.
4. Bấm nút **"Nạp FW"** trên trình duyệt (Chrome / Edge), chọn cổng COM của ESP32-S3 và chờ nạp 100%.

---

## 6. CÁCH TỰ BUILD MÃ NGUỒN BẰNG ESP-IDF
```bash
cd xiaozhi-esp32_vietnam
idf.py set-target esp32s3
idf.py menuconfig   # Board type chọn: ROBOT DAY DONG S3
idf.py build
idf.py -p COMx flash monitor
```

---

## 7. BO ESP32 PHỤ (SOUNDCARD BLUETOOTH A2DP)
Mã nguồn Arduino hoàn chỉnh:
[esp32_bluetooth_soundcard.ino](file:///d:/HNam/CODE/robot%20T1/esp32_bluetooth_soundcard/esp32_bluetooth_soundcard.ino)
* Mở bằng **Arduino IDE**, nạp vào bo ESP32 thường.
* Robot sẽ xuất hiện dưới tên Bluetooth: **`Robot-Xiaozhi-T1`**.
