#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <Client.h>
#include <PubSubClient.h>

#define DEBUG_MQTT_MANAGER 1

#if DEBUG_MQTT_MANAGER
  #define MQTT_MANAGER_LOG(x) do {Serial.print(x)} while (0)
  #define MQTT_MANAGER_LOG_F(fmt, ...) do {Serial.printf("\n[MQTT_Manager] " fmt, ##__VA_ARGS__);} while (0)
  #define MQTT_MANAGER_LOG_LN(x) do {Serial.println(x)} while (0)
#else
  #define MQTT_MANAGER_LOG(...) do {} while (0)
  #define MQTT_MANAGER_LOG_F(...) do {} while (0)
  #define MQTT_MANAGER_LOG_LN(...) do {} while (0)
#endif

class MQTT_Manager
{
  private:
    PubSubClient MQTT_Client_;

    const char *server_;
    uint16_t port_;
    const char *username_;
    const char *password_;
    const char *client_id_;
    const char *topic_;
    bool auto_reconnect_server_;
    bool auto_reconnect_wifi_;
    uint16_t reconnect_interval_;
    uint8_t max_reconnect_attempts_;
    uint16_t size_;
    uint16_t keep_alive_;
    const char *will_topic_;
    const char *will_message_;
    uint8_t will_qos_;
    bool will_retain_;

    bool is_initialized_ = false;
    uint8_t reconnect_attempts_ = 0;
    unsigned long last_time_ = 0;

    void (*on_connect_subscribe_callback_)() = nullptr;
  
  public:
    /**
     * @brief Konstruktor untuk inisialisasi kelas MQTT_Manager.
     * 
     * @param network_client Referensi ke antarmuka klien jaringan (misal: WiFiClient atau WiFiClientSecure).
     */
    MQTT_Manager(Client &network_client);

    /**
     * @brief Mengatur konfigurasi penuh untuk koneksi broker MQTT, WiFi, dan Last Will and Testament (LWT).
     * 
     * @param server                 Alamat IP atau domain dari broker MQTT.
     * @param port                   Port dari broker MQTT.
     * @param username               Kredensial username MQTT.
     * @param password               Kredensial password MQTT.
     * @param client_id              ID Klien unik untuk perangkat (wajib unik di broker).
     * @param topic                  Base topic opsional untuk otomatis di-subscribe setelah terhubung (Default: "").
     * @param auto_reconnect_server  Aktifkan/nonaktifkan koneksi ulang otomatis ke broker (Default: true).
     * @param auto_reconnect_wifi    Aktifkan/nonaktifkan pemulihan koneksi WiFi otomatis (Default: true).
     * @param reconnect_interval     Jeda waktu (dalam milidetik) antar percobaan reconnect (Default: 5000 ms).
     * @param max_reconnect_attempts Batas maksimal percobaan reconnect WiFi sebelum ESP di-restart otomatis. Isi 0 untuk disable (Default: 0).
     * @param size                   Ukuran buffer MQTT (Default: 256 bytes).
     * @param keep_alive             Interval keep-alive koneksi dalam detik (Default: 60 detik).
     * @param will_topic             Topik untuk pesan LWT (Default: "").
     * @param will_message           Pesan payload saat perangkat offline tiba-tiba (Default: "offline").
     * @param will_qos               Level QoS untuk pesan LWT (Default: 0).
     * @param will_retain            Status flag retain untuk pesan LWT (Default: true).
     */
    void begin(
      const char *server,
      uint16_t port,
      const char *username,
      const char *password,
      const char *client_id,
      const char *topic = "",
      bool auto_reconnect_server = true,
      bool auto_reconnect_wifi = true,
      uint16_t reconnect_interval = 5000,
      uint8_t max_reconnect_attempts = 0,
      uint16_t size = 256,
      uint16_t keep_alive = 60,
      const char *will_topic = "",
      const char *will_message = "offline",
      uint8_t will_qos = 0,
      bool will_retain = true
    );

    /**
     * @brief Mendaftarkan fungsi callback dari pengguna untuk menangani pesan masuk MQTT.
     */
    void set_on_message_callback(MQTT_CALLBACK_SIGNATURE);

    /**
     * @brief Mendaftarkan fungsi pengguna yang berisi daftar topik berlangganan (subscriptions).
     * 
     * @param subscribe_callback Pointer ke fungsi void. Fungsi ini akan selalu dieksekusi 
     *                           setiap kali MQTT_Manager berhasil terhubung/reconnect ke broker.
     */
    void set_on_connect_subscribe_callback(void (*subscribe_callback)());
    
    /**
     * @brief Mengirimkan pesan string ke topik MQTT.
     * 
     * @param topic   Topik tujuan.
     * @param payload Pesan berupa null-terminated string.
     * @return true jika pesan berhasil dipublikasikan, false jika gagal.
     */
    boolean publish(const char *topic, const char *payload);

    /**
     * @brief Mengirimkan pesan biner (byte) ke topik MQTT berdasarkan ukuran panjangnya.
     * 
     * @param topic   Topik tujuan.
     * @param payload Pointer ke array byte (uint8_t) yang akan dikirim.
     * @param plength Panjang/ukuran payload.
     * @return true jika pesan berhasil dipublikasikan, false jika gagal.
     */
    boolean publish(const char *topic, const uint8_t *payload, size_t plength);

    /**
     * @brief Mengirimkan pesan karakter dengan panjang tertentu (overloaded).
     */
    boolean publish(const char *topic, const char *payload, size_t plength);
    
    /**
     * @brief Berlangganan (subscribe) ke suatu topik MQTT dengan QoS 0.
     * 
     * @param topic Topik yang ingin didengarkan.
     * @return true jika perintah subscribe berhasil dikirim ke broker.
     */
    boolean subscribe(const char *topic);

    /**
     * @brief Berlangganan (subscribe) ke suatu topik MQTT dengan level QoS spesifik.
     */
    boolean subscribe(const char *topic, uint8_t qos);

    /**
     * @brief Mengelola logika non-blocking (millis) untuk memastikan perangkat tetap terhubung ke WiFi dan Broker.
     * 
     * @note Dipanggil secara otomatis di dalam fungsi loop_start().
     */
    void reconnect();

    /**
     * @brief Menjalankan siklus loop MQTT (menjaga ping, memproses antrean pesan masuk) dan menjaga koneksi (auto-reconnect).
     * 
     * @note Wajib dipanggil secara konstan di dalam loop() Arduino/ESP32.
     */
    void loop_start();

    /**
     * @brief Memeriksa apakah perangkat saat ini sedang terhubung ke broker MQTT.
     * 
     * @return true jika terhubung, false jika tidak.
     */
    boolean is_connected();

    /**
     * @brief Mendapatkan instance asli dari objek PubSubClient yang sedang digunakan.
     * 
     * Berguna untuk melakukan passing parameter client ke modul/library lain (seperti MQTT_OTA).
     * 
     * @return Referensi ke objek PubSubClient.
     */
    PubSubClient &get_client();
};