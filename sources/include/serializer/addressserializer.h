#pragma once

#include "serializer/elementserializer.h"
#include <citygml/address.h>

namespace citygml {

    /**
     * @brief AddressSerializer负责序列化Address对象
     */
    class LIBCITYGML_EXPORT AddressSerializer : public ElementSerializer {
    public:
        AddressSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger);
        
        void serialize(const Address& address);
        
    private:
        void serializeAddressDetails(const Address& address);
        void serializeMultiPoint(const Address& address);
    };

}
