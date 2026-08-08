#include "MQTT_Manager.h"

MQTT_Manager::MQTT_Manager() {}

void MQTT_Manager::begin(
  const char *server,
  uint16_t port,
  const char *username, 
  const char *password,
  const char *client_id, 
  const char *topic,
  uint16_t size,
  uint16_t keep_alive,
  bool auto_reconnect_server,
  bool auto_reconnect_wifi,
  uint16_t reconnect_interval,
  uint8_t max_reconnect_attempts,
  const char *will_topic,
  const char *will_message,
  uint8_t will_qos,
  bool will_retain
)
{
  server_ = server;
  port_ = port;
  username_ = username;
  password_ = password;
  client_id_ = client_id;
  topic_ = topic;
  size_ = size;
  keep_alive_ = keep_alive;
  auto_reconnect_server_ = auto_reconnect_server;
  auto_reconnect_wifi_ = auto_reconnect_wifi;
  reconnect_interval_ = reconnect_interval;
  max_reconnect_attempts_ = max_reconnect_attempts;
  will_topic_ = will_topic;
  will_message_ = will_message;
  will_qos_ = will_qos;
  will_retain_ = will_retain;

  is_initialized_ = true;
  
  MQTT_Client_.setClient(WiFi_Client_);

  MQTT_Client_.setServer(server_, port_);
  if (keep_alive_ != 60) {MQTT_Client_.setKeepAlive(keep_alive_);}
  if (size_ != 256) {MQTT_Client_.setBufferSize(size_);}
}

void MQTT_Manager::set_callback(MQTT_CALLBACK_SIGNATURE) {MQTT_Client_.setCallback(callback);}

void MQTT_Manager::set_on_connect_subscribe_callback(void (*subscribe_callback)()) {on_connect_subscribe_callback_ = subscribe_callback;}

boolean MQTT_Manager::publish(const char *topic, const char *payload) {return MQTT_Client_.publish(topic, payload);}
boolean MQTT_Manager::publish(const char *topic, const uint8_t *payload, size_t plength) {return MQTT_Client_.publish(topic, payload, (unsigned int)plength);}
boolean MQTT_Manager::publish(const char *topic, const char *payload, size_t plength) {return MQTT_Client_.publish(topic, (const uint8_t*)payload, (unsigned int)plength);}

boolean MQTT_Manager::subscribe(const char *topic) {return MQTT_Client_.subscribe(topic);}
boolean MQTT_Manager::subscribe(const char *topic, uint8_t qos) {return MQTT_Client_.subscribe(topic, qos);}

void MQTT_Manager::connect()
{ 
  if (!is_initialized_) {return;}
  if (is_connected()) {return;}

  if (auto_reconnect_wifi_)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      reconnect_attempts_++;
      Serial.printf("\n[MQTT_Manager] Reconnecting...(%d attempt)\n", reconnect_attempts_);
      WiFi.reconnect();
      if (max_reconnect_attempts_ > 0 && reconnect_attempts_ >= max_reconnect_attempts_)
      {
        reconnect_attempts_ = 0;
        Serial.println();
        Serial.println("[MQTT_Manager] Reconnecting timeout, Restarting...");
        Serial.println();
        ESP.restart();
      }
      return;
    }
    else {reconnect_attempts_ = 0;}
  }
  
  if (auto_reconnect_server_)
  {
    Serial.println();
    Serial.print("[MQTT_Manager] Connecting to MQTT...");

    bool client_connected = false;

    if (strcmp(will_topic_, "") != 0) {client_connected = MQTT_Client_.connect(client_id_, username_, password_, will_topic_, will_qos_, will_retain_, will_message_);} 
    else {client_connected = MQTT_Client_.connect(client_id_, username_, password_);}
    
    if (client_connected)
    {
      Serial.println("OK");

      if (strcmp(will_topic_, "") != 0) {MQTT_Client_.publish(will_topic_, "online", will_retain_);}
      
      if (strcmp(topic_, "") != 0)
      {
        Serial.println();
        Serial.printf("[MQTT_Manager] Subscribe to topic: %s\n", topic_);
        MQTT_Client_.subscribe(topic_);
      }

      if (on_connect_subscribe_callback_ != nullptr)
      {
        Serial.println("[MQTT_Manager] Executing user subscribe list...");
        on_connect_subscribe_callback_();
      }
    }
    else
    {
      Serial.printf("Failed. rc=%d\n", MQTT_Client_.state());
    }
  }
}

void MQTT_Manager::loop_start()
{
  if (!is_initialized_) {Serial.println("[MQTT_Manager] MQTT_Manager is not initialized, please call begin() first."); return;}

  if (!is_connected() && (millis() - last_time_ > reconnect_interval_) && auto_reconnect_server_) {connect(); last_time_ = millis();}
  MQTT_Client_.loop();
}

boolean MQTT_Manager::is_connected() {return MQTT_Client_.connected();}

PubSubClient &MQTT_Manager::get_client() {return MQTT_Client_;}