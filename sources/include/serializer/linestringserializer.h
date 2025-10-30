#pragma once

#include "serializer/elementserializer.h"
#include <citygml/linestring.h>

namespace citygml {

    /**
     * @brief LineStringSerializer负责序列化LineString对象
     */
    class LIBCITYGML_EXPORT LineStringSerializer : public ElementSerializer {
    public:
        LineStringSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger);
        
        void serialize(const LineString& lineString);
        
    private:
        void serializePosList(const std::vector<TVec3d>& vertices);
    };

}
