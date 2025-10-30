#pragma once

#include "serializer/elementserializer.h"
#include <citygml/materialtargetdefinition.h>
#include <citygml/material.h> 
#include <memory>

namespace citygml {

    class MaterialSerializer;

    class LIBCITYGML_EXPORT MaterialTargetDefinitionSerializer : public ElementSerializer {
    public:
        MaterialTargetDefinitionSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger);

        void serialize(const MaterialTargetDefinition& materialTargetDef);
    };

}