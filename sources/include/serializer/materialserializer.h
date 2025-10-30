#pragma once

#include "serializer/elementserializer.h"
#include <citygml/material.h>

namespace citygml {

    /**
     * @brief MaterialSerializer负责序列化Material对象
     */
    class LIBCITYGML_EXPORT MaterialSerializer : public ElementSerializer {
    public:
        MaterialSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger);
        
        void serialize(const Material& material);
        
    private:
        void serializeDiffuseColor(const Material& material);
        void serializeSpecularColor(const Material& material);
        void serializeAmbientColor(const Material& material);
        void serializeEmissionColor(const Material& material);
        void serializeShininess(const Material& material);
        void serializeTransparency(const Material& material);
        void serializeIsSmooth(const Material& material);
    };

}
