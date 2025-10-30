#pragma once

#include "serializer/elementserializer.h"
#include <citygml/geometry.h>

namespace citygml {

    /**
     * @brief GeometrySerializer负责序列化Geometry对象
     */
    class LIBCITYGML_EXPORT GeometrySerializer : public ElementSerializer {
    public:
        GeometrySerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger);
        
        void serialize(const Geometry& geometry);
        
    private:
        //void serializeLODInfo(const Geometry& geometry);
        void serializePolygons(const Geometry& geometry);
        void serializeLineStrings(const Geometry& geometry);
        void serializeChildGeometries(const Geometry& geometry);
        
        std::string getGeometryElementName(const Geometry& geometry) const;
    };

}
