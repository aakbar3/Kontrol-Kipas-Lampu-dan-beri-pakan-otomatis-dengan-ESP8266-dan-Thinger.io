#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHTesp.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Servo.h>
#include <ThingerESP8266.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>

// Konfigurasi Thinger.io
char USERNAME[32] = {0};
char DEVICE_ID[32] = {0};
char DEVICE_CREDENTIAL[32] = {0};
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

// Konfigurasi NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 28800); // Offset waktu untuk GMT+8 (28800 detik)

// Konfigurasi DHT
DHTesp dht;
const int DHT_PIN = 0; // Pin GPIO0

// Konfigurasi LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat I2C LCD 16x2

// Pin untuk Kipas dan Lampu
const int FAN_PIN = D6; // Pin GPIO12
const int LIGHT_PIN = D5; // Pin GPIO14

// Servo
Servo servo;
const int SERVO_PIN = D4; // Pin GPIO2

bool isFeeding = false; // Status feed

ESP8266WebServer server(80);

void handleRoot() {
  String html = "<!DOCTYPE html><html><body>";
  html += "<h1>Configuration Page</h1>";
  html += "<form action='/save' method='get'>";
  html += "USERNAME:<br><input type='text' name='username' value='" + String(USERNAME) + "'><br>";
  html += "DEVICE ID:<br><input type='text' name='device_id' value='" + String(DEVICE_ID) + "'><br>";
  html += "DEVICE CREDENTIAL:<br><input type='text' name='device_credential' value='" + String(DEVICE_CREDENTIAL) + "'><br>";
  html += "<br><input type='submit' value='Save'>";
  html += "</form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSave() {
  strlcpy(USERNAME, server.arg("username").c_str(), sizeof(USERNAME));
  strlcpy(DEVICE_ID, server.arg("device_id").c_str(), sizeof(DEVICE_ID));
  strlcpy(DEVICE_CREDENTIAL, server.arg("device_credential").c_str(), sizeof(DEVICE_CREDENTIAL));
  thing.set_credentials(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
  
  // Simpan data ke EEPROM
  EEPROM.put(0, USERNAME);
  EEPROM.put(32, DEVICE_ID);
  EEPROM.put(64, DEVICE_CREDENTIAL);
  EEPROM.commit();
  
  server.send(200, "text/plain", "Terima Kasih telah mensubmit");

  // Setel ulang koneksi WiFi
  ESP.restart();  // Restart perangkat untuk menyambungkan ulang ke WiFi
}

void setup() {
  // Memulai Serial Monitor
  Serial.begin(115200);

  // Inisialisasi EEPROM
  EEPROM.begin(96);

  // Membaca data konfigurasi dari EEPROM
  EEPROM.get(0, USERNAME);
  EEPROM.get(32, DEVICE_ID);
  EEPROM.get(64, DEVICE_CREDENTIAL);

  // Inisialisasi DHT
  dht.setup(DHT_PIN, DHTesp::DHT22);

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();

  // Inisialisasi Pin Kipas dan Lampu
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(FAN_PIN, HIGH); // Pastikan relay dimatikan (HIGH)
  digitalWrite(LIGHT_PIN, HIGH); // Pastikan relay dimatikan (HIGH)

  // Menghubungkan ke WiFi menggunakan WiFiManager
  WiFiManager wifiManager;
  wifiManager.setDebugOutput(true); // Enable debug output for WiFiManager
  if (!wifiManager.autoConnect("Pak_Restu_Ganteng")) {
    Serial.println("failed to connect and hit timeout");
    ESP.restart();
  }
  
  // Ubah logika koneksi WiFi di sini
  WiFi.begin(WiFi.SSID().c_str(), WiFi.psk().c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected...yeey :)");

  // Memulai NTP
  timeClient.begin();
  timeClient.update();

  // Inisialisasi Servo
  servo.attach(SERVO_PIN);

  // Menampilkan suhu di Thinger.io
  thing["temperature"] >> [](pson & out){
    out = dht.getTemperature();
  };

  // Menampilkan status kipas di Thinger.io
  thing["fan_status"] >> [](pson & out){
    out = digitalRead(FAN_PIN) == LOW ? 1 : 0;
  };

  // Menampilkan status lampu di Thinger.io
  thing["light_status"] >> [](pson & out){
    out = digitalRead(LIGHT_PIN) == LOW ? 1 : 0;
  };

  // Menjalankan proses feed dari Thinger.io
  thing["feed"] << [](pson & in) {
    if (in.is_empty()) {
      in = isFeeding; // Menampilkan status feed saat ini
    } else {
      if (in) {
        isFeeding = true;
        feedServo();
        isFeeding = false;
      }
    }
  };

  // Inisialisasi web server
  server.on("/", handleRoot);
  server.on("/save", handleSave);
  server.begin();

  servo.write(0);
}

void loop() {
  // Memperbarui Thinger.io
  thing.handle();

  // Memproses request dari client pada web server
  server.handleClient();

  // Membaca suhu dari DHT22
  float temperature = dht.getTemperature();

  // Mendapatkan waktu dari NTP
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();

  // Memperoleh alamat IP sebagai string
  String ipAddress = WiFi.localIP().toString();

  // Menemukan posisi titik pertama dalam alamat IP
  int dotPosition = ipAddress.indexOf('.');

  // Mengambil bagian IP setelah titik pertama
  String ipAddressSuffix = ipAddress.substring(dotPosition + 5);

  // Menampilkan suhu dan waktu di LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Suhu: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print(formattedTime);
  lcd.print("   ");
  lcd.print(ipAddressSuffix);

  // Logika kontrol kipas dan lampu
  if (temperature > 31.0) {
    digitalWrite(FAN_PIN, LOW); // Nyalakan kipas (relay aktif)
    digitalWrite(LIGHT_PIN, HIGH); // Matikan lampu (relay tidak aktif)
  } else if (temperature < 27.0) {
    digitalWrite(FAN_PIN, HIGH); // Matikan kipas (relay tidak aktif)
    digitalWrite(LIGHT_PIN, LOW); // Nyalakan lampu (relay aktif)
  } else {
    digitalWrite(FAN_PIN, HIGH); // Matikan kipas (relay tidak aktif)
    digitalWrite(LIGHT_PIN, HIGH); // Matikan lampu (relay tidak aktif)
  }

  // Logika proses feed servo
  if ((currentHour == 7 || currentHour == 16) && currentMinute == 0 && currentSecond == 0 && !isFeeding) {
    isFeeding = true;
    feedServo();
    isFeeding = false;
  }

  // Delay sebelum loop berikutnya
  delay(1000); // Delay 1 detik
}

void feedServo() {
  // Gerakkan servo dari sudut 0 hingga 100 dan kembali ke 0
  for (int pos = 0; pos <= 100; pos += 50) { // Gerakkan dari 0 ke 100 derajat
    servo.write(pos);              // Tentukan posisi servo
    delay(15);                     // Tunggu 15ms untuk mencapai posisi
  }
  delay(1000); // Tunggu 1 detik di posisi 100 derajat
  for (int pos = 100; pos >= 0; pos -= 50) { // Kembali ke 0 derajat
    servo.write(pos);              // Tentukan posisi servo
    delay(15);                     // Tunggu 15ms untuk mencapai posisi
  }
}
