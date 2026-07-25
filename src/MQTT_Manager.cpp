#include "MQTT_Manager.h"

MQTT_Manager::MQTT_Manager(
  const char *server,
  uint16_t port,
  const char *username, 
  const char *password,
  const char *client_id, 
  const char *topic,
  uint16_t size,
  uint16_t keep_alive,
  bool auto_connect,
  uint16_t reconnect_interval,
  uint8_t max_reconnect_attempts
) :
  _MQTT_Server(server),
  _MQTT_Port(port),
  _MQTT_Username(username),
  _MQTT_Password(password),
  _MQTT_Client_ID(client_id),
  _MQTT_Topic(topic),
  _MQTT_size(size),
  _MQTT_keep_alive(keep_alive),
  _MQTT_auto_connect(auto_connect),
  _MQTT_reconnect_interval(reconnect_interval),
  _MQTT_max_reconnect_attempts(max_reconnect_attempts)
{}

void MQTT_Manager::begin()
{
  _MQTT_Client.setClient(_WiFi_Client);

  _MQTT_Client.setServer(_MQTT_Server, _MQTT_Port);
  if (_MQTT_keep_alive != 60) {_MQTT_Client.setKeepAlive(_MQTT_keep_alive);}
  if (_MQTT_size != 256) {_MQTT_Client.setBufferSize(_MQTT_size);}
}

void MQTT_Manager::set_callback(MQTT_CALLBACK_SIGNATURE) {_MQTT_Client.setCallback(callback);}

boolean MQTT_Manager::publish(const char *topic, const char *payload) {return _MQTT_Client.publish(topic, payload);}
boolean MQTT_Manager::publish(const char *topic, const uint8_t *payload, size_t plength) {return _MQTT_Client.publish(topic, payload, (unsigned int)plength);}
boolean MQTT_Manager::publish(const char *topic, const char *payload, size_t plength) {return _MQTT_Client.publish(topic, (const uint8_t*)payload, (unsigned int)plength);}

void MQTT_Manager::connect()
{
  if (is_connected()) {return;}
  
  if (WiFi.status() != WL_CONNECTED)
  {
    _reconnect_attempts++;
    Serial.printf("\nReconnecting...(%d attempt)\n", _reconnect_attempts);
    WiFi.reconnect();
    if (_MQTT_max_reconnect_attempts > 0 && _reconnect_attempts >= _MQTT_max_reconnect_attempts)
    {
      _reconnect_attempts = 0;
      Serial.println();
      Serial.println("Reconnecting timeout, Restarting...");
      Serial.println();
      ESP.restart();
    }
    return;
  }
  else {_reconnect_attempts = 0;}
  
  Serial.println();
  Serial.print("Connecting to MQTT...");
  
  if (_MQTT_Client.connect(_MQTT_Client_ID, _MQTT_Username, _MQTT_Password))
  {
    Serial.println("OK");
    if (strcmp(_MQTT_Topic, "") != 0)
    {
      Serial.println();
      Serial.printf("Subscribe to topic: %s\n", _MQTT_Topic);
      _MQTT_Client.subscribe((String(_MQTT_Topic)+"/#").c_str());
    }
  }
  else
  {
    Serial.printf("Failed. rc=%d\n", _MQTT_Client.state());
  }
}

void MQTT_Manager::loop_start()
{
  if ((millis() - _last_time > _MQTT_reconnect_interval) and _MQTT_auto_connect) {connect(); _last_time = millis();}
  _MQTT_Client.loop();
}

boolean MQTT_Manager::is_connected() {return _MQTT_Client.connected();}

PubSubClient &MQTT_Manager::get_client() {return _MQTT_Client;}