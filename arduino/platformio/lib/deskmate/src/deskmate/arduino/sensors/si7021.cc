#include "deskmate/arduino/sensors/si7021.h"
#include "Arduino.h"
#include "Adafruit_Si7021.h"
#include <string>  
#include <sstream>

using deskmate::mqtt::MQTTMessage;


namespace deskmate {
    namespace arduino {
        namespace sensors {

          struct readings
            {
                std::string temperature;
                std::string humidity;
            };    
        char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
            char fmt[20];
            sprintf(fmt, "%%%d.%df", width, prec);
            sprintf(sout, fmt, val);
            return sout;
        }

        bool si7021::InitSensor(){
            Serial.println("Si7021 test!");
            
            if (!sensor->begin()) {
                Serial.println("Did not find Si7021 sensor!");
                return false;
            }

            Serial.print("Found model ");
            switch(sensor->getModel()) {
                case SI_Engineering_Samples:
                Serial.print("SI engineering samples"); break;
                case SI_7013:
                Serial.print("Si7013"); break;
                case SI_7020:
                Serial.print("Si7020"); break;
                case SI_7021:
                Serial.print("Si7021"); break;
                case SI_UNKNOWN:
                default:
                Serial.print("Unknown");
            }
            Serial.print(" Rev(");
            Serial.print(sensor->getRevision());
            Serial.print(")");
            Serial.print(" Serial #"); Serial.print(sensor->sernum_a, HEX); Serial.println(sensor->sernum_b, HEX);
            return true;
            }

        si7021::si7021(){
            sensorType="si7021";
            si7021("kitchen");
        }
        si7021::si7021(std::string _location){
            Serial.print("Initialising Si7101 sensor for ");
            Serial.println(_location.c_str());

            location=_location;
            sensorType="si7021";
            sensor = new Adafruit_Si7021();    
            
            _isConnected= InitSensor();
        }
        std::string si7021::getTopic(std::string metric){
            std::string mode = "home";
            std::string topic =mode + "/tele/" + metric +"/" + location;
            Serial.println(topic.c_str());
            return topic;
        }
 

         void si7021::read(deskmate::mqtt::MQTTMessageBuffer *mqtt_buffer_ )  {
            if (_isConnected) { 
            // Serial.println("dummy si7021 reading");
                float temperature = 0;
                float humidity = 0;
                temperature=sensor->readTemperature();
                char tempString[8];
                dtostrf(temperature, 1, 2, tempString);    


                char buffer[5];
                String t = dtostrf(temperature, 1, 4, buffer);
                Serial.print("Converted Temperature: ");
                Serial.println(t);

                humidity=sensor->readHumidity();
                char humString[8];
                dtostrf(humidity, 1, 2, humString);

                // std::string thisTemp (std::to_string(temperature));
                // std::stringstream sstream;
                // sstream << temperature;
                // std::string tmp_str = sstream.str();

                // string num_str1(std::to_string(n1));

                Serial.print("Temperature: ");
                Serial.print(temperature);
                Serial.print("\t:Humidity ");
                Serial.println(humidity);

                readings thisReading;
                thisReading.temperature=tempString;
                thisReading.humidity=humString;
                
                std::string metricTemp = "temperature";
                std::string metricHumidity = "humidity";

                MQTTMessage temperatureMessage;
                temperatureMessage.topic = getTopic(metricTemp);
                temperatureMessage.payload = thisReading.temperature;

                MQTTMessage humidityMessage;
                humidityMessage.topic=getTopic(metricHumidity);
                humidityMessage.payload =thisReading.humidity;  
                
                
                // mqtt_buffer_->Tick();
                mqtt_buffer_->Publish(temperatureMessage);
                mqtt_buffer_->Publish(humidityMessage);
            }
            else
            {
                Serial.println("not connected");
            }
                };
            }
        }


    }
