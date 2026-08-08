# MQTT Manager Library

MQTT Manager adalah library C++ (wrapper) ringan dan elegan untuk ESP32/ESP8266 yang menyederhanakan penggunaan library `PubSubClient`. 

Library ini diciptakan untuk memecahkan masalah umum pada perangkat IoT, yaitu menangani putus-nyambung koneksi WiFi & MQTT (*auto-reconnect*) secara *non-blocking*, mengelola Last Will and Testament (LWT), dan memastikan status *subscribe* tidak hilang setelah perangkat melakukan *reconnect*.

## Fitur Utama
* 🚀 **Sangat Mudah Digunakan**: Bungkus kompleksitas inisialisasi MQTT dalam satu fungsi `begin()`.
* 🔄 **Non-Blocking Auto Reconnect**: Terputus dari WiFi atau Broker? Library ini akan mengurus *reconnect* di balik layar tanpa memblokir fungsi `loop()` (tidak menggunakan `delay()`).
* 📡 **Subscription Callback Aman**: Otomatis berlangganan (*subscribe*) kembali ke semua topik Anda setiap kali perangkat terhubung ulang ke broker.
* 💀 **Built-in Last Will & Testament (LWT)**: Mampu memberi tahu sistem jika ESP mati atau terputus mendadak.

## Dependensi
* [PubSubClient](https://github.com/knolleary/pubsubclient) by Nick O'Leary

## Cara Instalasi
1. Unduh repositori ini dalam bentuk `.zip`.
2. Buka Arduino IDE.
3. Masuk ke **Sketch** > **Include Library** > **Add .ZIP Library...**
4. Pilih file zip yang sudah diunduh.

## Referensi API (*Method*) Utama

* `void begin(...)` : Fungsi inisialisasi utama. Memiliki parameter yang sangat lengkap (Broker, Kredensial, Timeout, Interval Reconnect, LWT, dll).
**Catatan Penting Parameter `topic`**: Parameter keenam pada fungsi `begin()` (yaitu `topic`) didesain sebagai **Base Topic** (Topik Dasar). Jika Anda mengisi parameter ini (tidak dikosongkan `""`), library akan secara otomatis melakukan *subscribe* ke Base Topic tersebut setiap kali perangkat berhasil terkoneksi ke broker MQTT. Ini sangat berguna jika perangkat Anda memiliki satu topik kendali utama.
* `void loop_start()` : Wajib dipanggil di dalam `void loop()`. Menangani siklus komunikasi `PubSubClient` dan logika *auto-reconnect*.
* `void set_callback(MQTT_CALLBACK_SIGNATURE)` : Meneruskan pesan masuk ke fungsi buatan *user*.
* `void set_on_connect_subscribe_callback(void (*callback)())` : Mendaftarkan fungsi berisi daftar *subscribe* (seperti `mqtt.subscribe()`). Fungsi ini akan dieksekusi **setelah** koneksi dan Base Topic berhasil di-*subscribe*.
* `boolean publish(topic, payload)` : Mempublikasikan pesan.
* `boolean subscribe(topic, [qos])` : Berlangganan ke topik tambahan tertentu.
* `boolean is_connected()` : Mengecek apakah koneksi MQTT sedang aktif.

---

## Penggunaan Singkat

```cpp
#include <WiFi.h>
#include "MQTT_Manager.h"

MQTT_Manager mqtt;

// 1. Buat daftar topik yang ingin didengarkan
void my_subscriptions() {
  mqtt.subscribe("topik/kontrol/relay");
}

// 2. Buat fungsi penerima pesan
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  // Logika saat pesan masuk
}

void setup() {
  // Koneksi WiFi standar di sini...

  // Inisialisasi MQTT
  mqtt.begin("broker.hivemq.com", 1883, "", "", "ESP32_Client_01");
  
  // Daftarkan fungsi-fungsi ke library
  mqtt.set_callback(mqtt_callback);
  mqtt.set_on_connect_subscribe_callback(my_subscriptions);
}

void loop() {
  // Biarkan library bekerja (mengatur koneksi dan pesan)
  mqtt.loop_start();
  
  // Publish pesan seperti biasa
  // mqtt.publish("topik/sensor", "hello");
}