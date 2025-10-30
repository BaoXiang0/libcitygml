#pragma once

#include "serializer/elementserializer.h"
#include <citygml/texturetargetdefinition.h>
#include <citygml/texture.h> // 需要包含 Texture 的头文件
#include <memory>

namespace citygml {

    class TextureSerializer; // 前向声明，避免循环引用

    class LIBCITYGML_EXPORT TextureTargetDefinitionSerializer : public ElementSerializer {
    public:
        TextureTargetDefinitionSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger);

        void serialize(const TextureTargetDefinition& textureTargetDef);
    };

}