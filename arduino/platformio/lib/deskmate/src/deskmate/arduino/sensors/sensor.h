#include "deskmate/mqtt/mqtt.h"


namespace deskmate {
    namespace arduino {
        namespace sensors {

            class sensor{
                public:
                    virtual void read(deskmate::mqtt::MQTTMessageBuffer *mqtt_buffer);
                private:
                    // std::string location;
                    deskmate::mqtt::MQTTMessageBuffer *mqtt_buffer_;
            };


            //  class dummy: public sensor{
            //     public:
            //     void read() override;
            // };
        }
    }
}