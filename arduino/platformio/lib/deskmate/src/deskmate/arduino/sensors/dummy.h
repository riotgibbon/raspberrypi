#include "sensor.h"

namespace deskmate {
    namespace arduino {
        namespace sensors {


             class dummy: public sensor{
                public:
                void read(deskmate::mqtt::MQTTMessageBuffer *mqtt_buffer) override;
            };
        }
    }
}