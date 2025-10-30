#pragma once

#include "serializer/elementserializer.h"
#include <citygml/texture.h>

namespace citygml {

    /**
     * @brief TextureSerializer负责序列化Texture对象
     */
	class LIBCITYGML_EXPORT TextureSerializer : public ElementSerializer {
	public:
		TextureSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger);

		// 新增一个只序列化Texture内容的方法，不再生成 <Texture> 根元素
		void serializeTextureContent(const Texture& texture);

	protected:
		void serializeImageURI(const Texture& texture);
		void serializeMimeType(const Texture& texture); // 新增
		void serializeTextureType(const Texture& texture); // 新增
		void serializeWrapMode(const Texture& texture);
		void serializeBorderColor(const Texture& texture);
		void serializeRepeat(const Texture& texture);
		std::string wrapModeToString(Texture::WrapMode mode) const;
	};

}
