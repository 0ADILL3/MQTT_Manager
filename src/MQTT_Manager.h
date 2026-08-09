#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <Client.h>
#include <PubSubClient.h>

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
   // Initialize Network Client for MQTT
    MQTT_Manager(Client &network_client);

    // Initialize MQTT connection parameters
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
    // Set MQTT message callback
    void set_on_message_callback(MQTT_CALLBACK_SIGNATURE);
    // Set callback function to execute user subscription lists after successful connection
    void set_on_connect_subscribe_callback(void (*subscribe_callback)());
    
    // Publish message to MQTT topic
    boolean publish(const char *topic, const char *payload);
    // Publish message to MQTT topic with payload length
    boolean publish(const char *topic, const uint8_t *payload, size_t plength);
    // Publish message to MQTT topic with payload length (overloaded for const char* payload)
    boolean publish(const char *topic, const char *payload, size_t plength);
    
    // Subscribe to MQTT topic
    boolean subscribe(const char *topic);
    // Subscribe to MQTT topic with QoS
    boolean subscribe(const char *topic, uint8_t qos);

    // Reconnect to MQTT broker and WiFi
    void reconnect();
    // Handle MQTT loop and auto reconnect
    void loop_start();

    // Check MQTT connection status
    boolean is_connected();
    // Get MQTT client instance
    PubSubClient &get_client();
};

void MQTT_Publish();
void MQTT_Subscribe();
void MQTT_Callback(char* topic, byte* payload, unsigned int length);