#include "serializer/textureserializer.h"
#include <citygml/texture.h>
#include <sstream>

namespace citygml {
	TextureSerializer::TextureSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger)
		: ElementSerializer(writer, params, logger)
	{
	}

	void TextureSerializer::serializeTextureContent(const Texture& texture) {
		// 不再输出 <Texture> 根元素，也不输出 gml:id 和 theme
		// 只序列化 Texture 的内部属性，以匹配 <ParameterizedTexture> 的内容

		serializeImageURI(texture);
		//serializeMimeType(texture);
		//serializeTextureType(texture);
		serializeWrapMode(texture);
		//serializeBorderColor(texture);
		//serializeRepeat(texture);
	}

	void TextureSerializer::serializeImageURI(const Texture& texture) {
		std::string url = texture.getUrl();
		if (!url.empty()) {
			m_writer.startElement("imageURI", APPR_NS);
			m_writer.writeTextContent(url);
			m_writer.endElement();
		}
	}

	void TextureSerializer::serializeMimeType(const Texture& texture) {
		std::string mimeType; // = texture.getMimeType(); // 假设有此方法
		if (mimeType.empty()) {
			mimeType = "image/jpg"; // 默认值，与 building_.gml 示例一致
		}
		m_writer.startElement("mimeType", APPR_NS);
		m_writer.writeTextContent(mimeType);
		m_writer.endElement();
	}

	void TextureSerializer::serializeTextureType(const Texture& texture) {
		std::string typeStr = texture.getType();
		m_writer.startElement("textureType", APPR_NS);
		m_writer.writeTextContent(typeStr);
		m_writer.endElement();
	}

	void TextureSerializer::serializeWrapMode(const Texture& texture) {
		Texture::WrapMode wrapMode = texture.getWrapMode();
		std::string wrapModeStr = wrapModeToString(wrapMode);

		m_writer.startElement("wrapMode", APPR_NS);
		m_writer.writeTextContent(wrapModeStr);
		m_writer.endElement();
	}

	void TextureSerializer::serializeBorderColor(const Texture& texture) {
		TVec4f borderColor = texture.getBorderColor();
		if (borderColor.x != 0.0f || borderColor.y != 0.0f || borderColor.z != 0.0f || borderColor.w != 0.0f) {
			m_writer.startElement("borderColor", APPR_NS);

			std::ostringstream colorStream;
			colorStream << borderColor.x << " " << borderColor.y << " " << borderColor.z << " " << borderColor.w;

			m_writer.writeTextContent(colorStream.str());
			m_writer.endElement();
		}
	}

	void TextureSerializer::serializeRepeat(const Texture& texture) {
		bool repeat = texture.getRepeat();
		m_writer.startElement("repeat", APPR_NS);
		m_writer.writeTextContent(repeat ? "true" : "false");
		m_writer.endElement();
	}

	std::string TextureSerializer::wrapModeToString(Texture::WrapMode mode) const {
		switch (mode) {
		case Texture::WrapMode::WM_NONE:
			return "WM_NONE";
		case Texture::WrapMode::WM_WRAP:
			return "wrap";
		case Texture::WrapMode::WM_MIRROR:
			return "mirror";
		case Texture::WrapMode::WM_CLAMP:
			return "clamp";
		case Texture::WrapMode::WM_BORDER:
			return "border";
		default:
			return "unknown";
		}
	}

}