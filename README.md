# Kontrol-Kipas-Lampu-dan-beri-pakan-otomatis-dengan-ESP8266-dan-Thinger.io

Repository ini berisi program untuk mengontrol kipas dan lampu menggunakan ESP8266 dengan integrasi ke Thinger.io, serta menampilkan suhu dan waktu di layar LCD. Program ini juga menyediakan fitur untuk mengontrol kipas dan lampu secara otomatis berdasarkan suhu yang terukur oleh sensor DHT22 dan memiliki mekanisme feeding menggunakan servo.

## Fitur

- Menghubungkan ESP8266 ke jaringan WiFi menggunakan WiFiManager.
- Mengontrol kipas dan lampu melalui platform Thinger.io.
- Menampilkan suhu dari sensor DHT22 di Thinger.io.
- Menampilkan waktu dari server NTP di layar LCD.
- Mengontrol kipas dan lampu berdasarkan suhu.
- Menyediakan web server untuk konfigurasi kredensial Thinger.io.
- Menjalankan mekanisme feeding otomatis menggunakan servo pada waktu yang dijadwalkan.

## Hardware yang Dibutuhkan

- ESP8266
- Sensor DHT22
- Relay module (2 channel)
- Servo motor
- LCD 16x2 dengan modul I2C
- Kabel jumper
- Breadboard

## Library yang Digunakan

- `EEPROM.h`
- `Wire.h`
- `LiquidCrystal_I2C.h`
- `DHTesp.h`
- `ESP8266WiFi.h`
- `NTPClient.h`
- `WiFiUdp.h`
- `Servo.h`
- `ThingerESP8266.h`
- `WiFiManager.h`
- `ESP8266WebServer.h`

## Wiring Diagram

- **DHT22**:
  - Data Pin: GPIO0 (D3)
  
- **Relay Pins**:
  - Relay 1 (Kipas): GPIO12 (D6)
  - Relay 2 (Lampu): GPIO14 (D5)
  
- **Servo**:
  - Signal Pin: GPIO2 (D4)
  
- **LCD**:
  - SDA: GPIO4 (D2)
  - SCL: GPIO5 (D1)

## Setup

1. Clone repository ini.
2. Install Arduino IDE jika belum terpasang.
3. Install library yang diperlukan melalui Library Manager di Arduino IDE.
4. Buka file `sketch_jun7a.ino` di Arduino IDE.
5. Upload kode ke ESP8266.

## Konfigurasi Thinger.io

1. Setelah mengupload kode, buka Serial Monitor untuk melihat informasi koneksi.
2. Hubungkan ke jaringan WiFi "Pak_Restu_Ganteng" yang disediakan oleh ESP8266.
3. Buka browser dan akses halaman konfigurasi di alamat `192.168.4.1`.
4. Isi informasi `USERNAME`, `DEVICE ID`, dan `DEVICE CREDENTIAL` untuk Thinger.io.
5. Klik tombol "Save" dan ESP8266 akan restart dan menyambung ke jaringan WiFi yang dikonfigurasi sebelumnya.

## Penggunaan

### Kontrol Kipas dan Lampu melalui Thinger.io

- Setelah ESP8266 tersambung ke WiFi dan Thinger.io, perangkat Anda akan muncul di dashboard Thinger.io.
- Anda dapat memonitor suhu, mengontrol kipas, lampu, dan mekanisme feeding melalui dashboard Thinger.io.

### Pengontrolan Otomatis Berdasarkan Suhu

- Kipas akan menyala jika suhu lebih dari 31°C.
- Lampu akan menyala jika suhu kurang dari 27°C.
- Jika suhu antara 27°C dan 31°C, kipas dan lampu akan mati.

### Mekanisme Feeding

- Servo akan bergerak untuk menjalankan mekanisme feeding pada jam 7:00 dan 16:00 setiap hari.
- Status feeding dapat dilihat dan dikontrol melalui Thinger.io.

### Konfigurasi Melalui Web Server

- Buka browser dan akses alamat IP ESP8266 untuk membuka halaman konfigurasi.
- Isi informasi yang dibutuhkan dan klik "Save" untuk menyimpan konfigurasi ke EEPROM.

## Troubleshooting

- Pastikan informasi WiFi dan Thinger.io Anda benar.
- Pastikan wiring sesuai dengan diagram.
- Pastikan library yang diperlukan sudah terinstall.

## Lisensi

Proyek ini dilisensikan di bawah MIT License. Silakan baca file LICENSE untuk informasi lebih lanjut.

## Kontribusi

Kontribusi sangat diterima! Silakan fork repository ini dan buat pull request dengan perubahan yang Anda sarankan.

---

Selamat mencoba dan semoga berhasil! Jika Anda memiliki pertanyaan atau masalah, jangan ragu untuk membuka isu di repository ini.
