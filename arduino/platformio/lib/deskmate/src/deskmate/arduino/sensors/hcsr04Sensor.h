#pragma once

#include "sensor.h"
#include "hcsr04.h"

namespace deskmate {
    namespace arduino {
        namespace sensors {
             class hcsr04Sensor: public sensor{
                public:
                    hcsr04Sensor();
                    hcsr04Sensor(std::string _location);
                    void read(deskmate::mqtt::MQTTMessageBuffer *mqtt_buffer) override;
                 
                private:
                    // HCSR04 hcsr04;
                    // bool InitSensor();
                    std::string location;
                    std::string getTopic(std::string metric);
            };
        }
    }
}
