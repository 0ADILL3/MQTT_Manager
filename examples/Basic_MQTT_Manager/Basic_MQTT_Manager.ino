#include <Arduino.h>

// Penyesuaian Library WiFi secara otomatis untuk ESP32 atau ESP8266
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#else
  #error "Library ini hanya mendukung ESP32 dan ESP8266"
#endif

#include "MQTT_Manager.h"

// Kredensial Jaringan & Broker
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

// Inisialisasi MQTT Manager
MQTT_Manager mqtt(
  "broker.hivemq.com", // Server MQTT
  1883,                // Port
  "user",              // Username MQTT (kosongkan "" jika tidak ada)
  "pass",              // Password MQTT (kosongkan "" jika tidak ada)
  "ESP32_Device",      // Client ID
  "proyek/utama"       // Topik utama (Otomatis subscribe ke "proyek/utama/#" saat terkoneksi)
);

// Fungsi penerima pesan (Callback)
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan diterima di topik: ");
  Serial.println(topic);
  
  // Contoh filter pembacaan berdasarkan sub-topik
  if (String(topic) == "proyek/utama/perintah") {
    Serial.println("Menjalankan perintah dari server!");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Connect WiFi terlebih dahulu
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");
  
  // Mulai MQTT
  mqtt.begin();
  mqtt.set_callback(mqtt_callback);
  // Library akan otomatis melakukan subscribe ke topik "proyek/utama/#"
  // di dalam fungsi mqtt.loop_start() segera setelah berhasil terhubung ke broker.
}

void loop() {
  // Menjaga koneksi MQTT tetap hidup (menangani auto-reconnect dan auto-subscribe)
  mqtt.loop_start();
  
  // Contoh publish data setiap 5 detik
  static unsigned long last_millis = 0;
  if (millis() - last_millis > 5000) {
    
    // Pastikan status terkoneksi sebelum memublikasikan data
    if (mqtt.is_connected()) {
      mqtt.publish("proyek/utama/sensor", "{\"suhu\": 25}");
    }
    
    last_millis = millis();
  }
}