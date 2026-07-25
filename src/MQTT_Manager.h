#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

class MQTT_Manager
{
  private:
    WiFiClient _WiFi_Client;
    PubSubClient _MQTT_Client;

    const char *_MQTT_Server;
    uint16_t _MQTT_Port;
    const char *_MQTT_Username;
    const char *_MQTT_Password;
    const char *_MQTT_Client_ID;
    const char *_MQTT_Topic;
    uint16_t _MQTT_size;
    uint16_t _MQTT_keep_alive;
    bool _MQTT_auto_connect;
    uint16_t _MQTT_reconnect_interval;
    uint8_t _MQTT_max_reconnect_attempts;

    uint8_t _reconnect_attempts = 0;
    unsigned long _last_time = 0;
  
  public:
    MQTT_Manager(
      const char *server,
      uint16_t port,
      const char *username, 
      const char *password,
      const char *client_id, 
      const char *topic = "",
      uint16_t size = 256,
      uint16_t keep_alive = 60,
      bool auto_connect = true,
      uint16_t reconnect_interval = 5000,
      uint8_t max_reconnect_attempts = 5
    );

    void begin();
    void set_callback(MQTT_CALLBACK_SIGNATURE);
    
    boolean publish(const char *topic, const char *payload);
    boolean publish(const char *topic, const uint8_t *payload, size_t plength);
    boolean publish(const char *topic, const char *payload, size_t plength);
    
    void connect();
    void loop_start();

    boolean is_connected();
    PubSubClient &get_client();
};

void MQTT_publish();