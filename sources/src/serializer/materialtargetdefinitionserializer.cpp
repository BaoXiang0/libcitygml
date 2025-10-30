#include "serializer/materialtargetdefinitionserializer.h"
#include "serializer/materialserializer.h"
#include "serializer/serializerconstants.h"

namespace citygml {

    MaterialTargetDefinitionSerializer::MaterialTargetDefinitionSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger)
        : ElementSerializer(writer, params, logger)
    {
    }

    void MaterialTargetDefinitionSerializer::serialize(const MaterialTargetDefinition& materialTargetDef) {
        m_writer.startElement("surfaceDataMember", APPR_NS);

        if (auto material = materialTargetDef.getAppearance()) {
			MaterialSerializer materialSerializer(m_writer, m_params, m_logger);
			materialSerializer.serialize(*material);

            m_writer.startElement("target", APPR_NS);
            m_writer.writeAttribute("uri", "#" + materialTargetDef.getTargetID());
            m_writer.endElement(); // target
        }

        m_writer.endElement(); // surfaceDataMember
    }

}