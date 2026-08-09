#include <Arduino.h>
#include <WiFi.h> // Gunakan <ESP8266WiFi.h> jika menggunakan ESP8266
#include "MQTT_Manager.h"

// Kredensial WiFi dan Broker
const char* WIFI_SSID = "NAMA_WIFI_ANDA";
const char* WIFI_PASS = "PASSWORD_WIFI_ANDA";

const char* MQTT_BROKER = "broker.emqx.io";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_USER = "";
const char* MQTT_PASS = "";
const char* CLIENT_ID = "ESP32_Client_Demo_123";

WiFiClient client;
// Inisialisasi Objek MQTT_Manager
MQTT_Manager mqtt(client);

// Variabel untuk non-blocking publish
unsigned long last_publish = 0;

// --- 1. Fungsi Callback Pesan Masuk (Subscribe) ---
void on_message_received(char* topic, byte* payload, unsigned int length) {
  Serial.print("\n[Pesan Masuk] Topik: ");
  Serial.println(topic);
  
  Serial.print("[Pesan Masuk] Payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// --- 2. Fungsi Daftar Topik Subscribe ---
// Fungsi ini akan otomatis dipanggil library setiap kali MQTT berhasil (re)connect
void my_subscription_list() {
  mqtt.subscribe("rumah/ruang_tamu/lampu");
  mqtt.subscribe("rumah/dapur/suhu", 1); // Subscribe dengan QoS 1
  Serial.println("Berhasil resubscribe ke semua topik!");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // --- Setup WiFi Pertama Kali ---
  Serial.println("\nMenghubungkan ke WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");

  // --- Setup MQTT Manager ---
  // Parameter (server, port, user, pass, client_id, default_topic, ...)
  mqtt.begin(
    MQTT_BROKER, 
    MQTT_PORT, 
    MQTT_USER, 
    MQTT_PASS, 
    CLIENT_ID, 
    "",                 // default base topic kosong
    true,               // auto reconnect server
    true,               // auto reconnect wifi
    5000,               // interval reconnect (ms)
    0,                  // max attempts (0 = no force restart)
    256,                // ukuran buffer
    60,                 // keep alive (detik)
    "rumah/status/esp", // Will Topic (LWT)
    "offline",          // Will Message
    0,                  // Will QoS
    true                // Will Retain
  );

  // Daftarkan callback untuk pesan masuk
  mqtt.set_on_message_callback(on_message_received);

  // Daftarkan callback untuk daftar subscribe
  mqtt.set_on_connect_subscribe_callback(my_subscription_list);
}

void loop() {
  // Wajib dipanggil di dalam loop agar proses MQTT (termasuk auto-reconnect) berjalan
  mqtt.loop_start();

  // Contoh Publish data sensor setiap 5 detik (Non-blocking)
  if (mqtt.is_connected() && millis() - last_publish > 5000) {
    last_publish = millis();
    
    String payload = "Suhu saat ini: " + String(random(25, 35)) + " C";
    
    Serial.println("Mengirim data: " + payload);
    mqtt.publish("rumah/dapur/suhu", payload.c_str());
  }
}