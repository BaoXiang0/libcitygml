#pragma once

#include "serializer/elementserializer.h"
#include <citygml/linearring.h>

namespace citygml {

    /**
     * @brief LinearRingSerializer负责序列化LinearRing对象
     */
    class LIBCITYGML_EXPORT LinearRingSerializer : public ElementSerializer {
    public:
        LinearRingSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger);
        
        void serialize(const LinearRing& linearRing);
        
    private:
        void serializePosList(const std::vector<TVec3d>& vertices);
        void serializePos(const TVec3d& vertex);
    };

}
