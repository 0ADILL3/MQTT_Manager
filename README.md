# MQTT_Manager Library

MQTT Manager adalah *library* untuk ESP32 dan ESP8266 yang berfungsi sebagai *wrapper* (pembungkus) berbasis objek untuk *library* `PubSubClient`. *Library* ini dirancang untuk menyederhanakan manajemen koneksi MQTT, menyediakan fitur *auto-reconnect* (koneksi ulang otomatis), serta terintegrasi secara mulus dengan fitur *Over-The-Air* (OTA).

## 🚀 Fitur Utama
- **Object-Oriented Design:** Kode lebih rapi, modular, dan mencegah bentrok variabel pada skala proyek besar.
- **Auto-Reconnect:** Menangani koneksi ulang (reconnect) WiFi dan MQTT secara otomatis di dalam siklus *loop* tanpa menggunakan *delay* yang dapat memblokir (blocking) keseluruhan sistem.
- **Auto-Subscribe:** Secara cerdas dan otomatis akan melakukan *subscribe* ke *base topic* dengan format *wildcard* `/#` begitu perangkat berhasil membangun koneksi ke *broker*.
- **OTA Integration:** Dilengkapi dukungan khusus untuk menyederhanakan proses pembaruan *firmware* via jaringan MQTT (`MQTT_OTA`).
- **Multi-Arsitektur:** Kompatibel dan stabil digunakan baik pada keluarga ESP32 maupun NodeMCU (ESP8266).

## 📦 Dependensi
Untuk menggunakan *library* ini, pastikan Anda telah menginstal *library* berikut di Arduino IDE / PlatformIO:
- [PubSubClient](https://github.com/knolleary/pubsubclient) (oleh Nick O'Leary)
- Core WiFi bawaan (Menggunakan `WiFi.h` untuk ESP32 atau `ESP8266WiFi.h` untuk ESP8266).

## 🛠️ Cara Instalasi
**Via Arduino IDE:**
1. Unduh *repository* ini sebagai file `.zip`.
2. Buka Arduino IDE.
3. Masuk ke menu **Sketch** -> **Include Library** -> **Add .ZIP Library...**
4. Pilih file `.zip` yang baru saja diunduh.