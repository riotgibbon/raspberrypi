#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include <queue>
#include <string>
#include <vector>

#include "credentials.h"
#include "deskmate/app/app.h"
// #include "deskmate/arduino/gfx/sharp_mem_display.h"
// #include "deskmate/arduino/input/buttons.h"
// #include "deskmate/arduino/input/crank.h"
#include "deskmate/arduino/net/mqtt.h"
#include "deskmate/arduino/net/wifi.h"
#include "deskmate/gfx/display.h"  // testing only

#include "deskmate/arduino/sensors/si7021.h"


using deskmate::app::App;
// using deskmate::arduino::gfx::SharpMemDisplay;
// using deskmate::arduino::input::SetupButtonsInterruptHandler;
// using deskmate::arduino::input::SetupCrankInterruptHandler;
using deskmate::arduino::net::MQTTManager;
using deskmate::mqtt::MQTTMessage;
using deskmate::arduino::net::WiFiManager;

// Display.
using deskmate::credentials::kCSPin;
using deskmate::credentials::kDisplayHeight;
using deskmate::credentials::kDisplayWidth;
using deskmate::credentials::kMOSIPin;
using deskmate::credentials::kSCKPin;

// Wifi.
using deskmate::credentials::kWIFIPassword;
using deskmate::credentials::kWIFISSID;

// MQTT.
using deskmate::credentials::kMQTTClientId;
using deskmate::credentials::kMQTTConfigs;
using deskmate::credentials::kMQTTFloatingPointSensors;
using deskmate::credentials::kMQTTPassword;
using deskmate::credentials::kMQTTPort;
using deskmate::credentials::kMQTTServer;
using deskmate::credentials::kMQTTUser;
using deskmate::credentials::kMQTTWeatherConfigs;

using deskmate::arduino::sensors::si7021;
using deskmate::arduino::sensors::sensor;


// void setup() {
//   Serial.begin(9600);

//   std::string device  = "esp32";
//   std::string delim  = "-";
//   std::string location  = "kitchen";

//   std::string client = device.append(delim).append(location);

//   WiFiManager wifi_manager(kWIFISSID, kWIFIPassword);
//   MQTTManager mqtt_manager(kMQTTServer, kMQTTPort, kMQTTUser, kMQTTPassword,
//                            client.c_str());
//   if (!wifi_manager.Connect()) {
//     Serial.println("Unable to connect to WiFi.");
//     return;
//   }
//   Serial.print("Connected to WiFi: ");
//   Serial.println(kWIFISSID);
//   if (!mqtt_manager.Connect()) {
//     Serial.println("Unable to connect to the MQTT server.");
//     return;
//   }
//   Serial.print("Connected to the MQTT server ");
//   Serial.println(kMQTTServer);
   
//   Serial.println("trying test message");
//   MQTTMessage msg;
//   msg.topic="test";
//   msg.payload="hello from ESP32 - kitchen";
//   mqtt_manager.Publish(msg);


//   si7021 *sensor_si7021 = new si7021("bedroom"); 

//   App app( &mqtt_manager);
//   app.Init(kMQTTConfigs, kMQTTFloatingPointSensors, kMQTTWeatherConfigs);
//   app.addSensor(sensor_si7021);
//   while (true) {
//     wifi_manager.MaybeReconnect();
//     app.Tick();
//   }
// }

void setup() {
  Serial.begin(9600);

  std::string device  = "esp32";  
  std::string location  = "kitchen";


  si7021 *sensor_si7021 = new si7021(location); 

  App app(location,device);
  app.Init();
  app.addSensor(sensor_si7021);
  while (true) {
    //wifi_manager.MaybeReconnect();
    app.Tick();
  }
}
void loop() {}

