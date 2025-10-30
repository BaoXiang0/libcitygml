#include "serializer/texturetargetdefinitionserializer.h"
#include "serializer/textureserializer.h"
#include <citygml/texturecoordinates.h>
#include "serializer/serializerconstants.h"
#include <sstream> 

namespace citygml {

	TextureTargetDefinitionSerializer::TextureTargetDefinitionSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger)
		: ElementSerializer(writer, params, logger)
	{
	}

	void TextureTargetDefinitionSerializer::serialize(const TextureTargetDefinition& textureTargetDef) {
		m_writer.startElement("surfaceDataMember", APPR_NS);

		if (auto texture = textureTargetDef.getAppearance()) {
			m_writer.startElement("ParameterizedTexture", APPR_NS);

			// 调用 TextureSerializer 的新方法来序列化 Texture 的内容
			TextureSerializer textureContentSerializer(m_writer, m_params, m_logger);
			textureContentSerializer.serializeTextureContent(*texture);

			m_writer.startElement("target", APPR_NS);
			m_writer.writeAttribute("uri", "#" + textureTargetDef.getTargetID());

			// 序列化纹理坐标
			if (textureTargetDef.getTextureCoordinatesCount() > 0) {
				m_writer.startElement("TexCoordList", APPR_NS);
				for (unsigned int k = 0; k < textureTargetDef.getTextureCoordinatesCount(); ++k) {
					auto texCoords = textureTargetDef.getTextureCoordinates(k);
					m_writer.startElement("textureCoordinates", APPR_NS);
					m_writer.writeAttribute("ring", "#" + texCoords->getTargetLinearRingID());
					
					std::ostringstream coordsStream;
					const std::vector<TVec2f>& coords = texCoords->getCoords();
					for (size_t l = 0; l < coords.size(); ++l) {
						coordsStream << coords[l].x << " " << coords[l].y;
						if (l < coords.size() - 1) {
							coordsStream << " ";
						}
					}
					m_writer.writeTextContent(coordsStream.str());
					m_writer.endElement(); // textureCoordinates
				}
				m_writer.endElement(); // TexCoordList
			}
			m_writer.endElement(); // target
			m_writer.endElement(); // ParameterizedTexture
		}

		m_writer.endElement(); // surfaceDataMember
	}

}