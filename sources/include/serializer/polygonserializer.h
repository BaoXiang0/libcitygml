#pragma once

#include "serializer/elementserializer.h"
#include <citygml/polygon.h>

namespace citygml {

    /**
     * @brief PolygonSerializer负责序列化Polygon对象
     */
    class LIBCITYGML_EXPORT PolygonSerializer : public ElementSerializer {
    public:
        PolygonSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger);
        
        void serialize(const Polygon& polygon);
        
    private:
        void serializeExteriorRing(const LinearRing& exteriorRing);
        void serializeInteriorRings(const std::vector<std::shared_ptr<LinearRing>>& interiorRings);
    };

}
