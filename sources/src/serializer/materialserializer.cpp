#include "serializer/materialserializer.h"
#include <citygml/material.h>
#include <sstream>

namespace citygml {

    MaterialSerializer::MaterialSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger)
        : ElementSerializer(writer, params, logger)
    {
    }

    void MaterialSerializer::serialize(const Material& material) {
        // 开始Material元素
        m_writer.startElement("Material", APPR_NS);
        
        // 写入ID属性
        if (!material.getId().empty()) {
            m_writer.writeAttribute("gml:id", material.getId());
        }
        
        // 序列化主题
        const std::vector<std::string>& themes = material.getThemes();
        for (const std::string& theme : themes) {
            m_writer.writeAttribute("theme", theme);
        }
        
        // 序列化材质属性
        serializeDiffuseColor(material);
        serializeSpecularColor(material);
        serializeAmbientColor(material);
        serializeEmissionColor(material);
        serializeShininess(material);
        serializeTransparency(material);
        serializeIsSmooth(material);
        
        // 结束Material元素
        m_writer.endElement();
    }

    void MaterialSerializer::serializeDiffuseColor(const Material& material) {
        TVec3f diffuse = material.getDiffuse();
        if (diffuse.x != 0.0f || diffuse.y != 0.0f || diffuse.z != 0.0f) {
            m_writer.startElement("diffuseColor", APPR_NS);
            
            std::ostringstream colorStream;
            colorStream << diffuse.x << " " << diffuse.y << " " << diffuse.z;
            
            m_writer.writeTextContent(colorStream.str());
            m_writer.endElement();
        }
    }

    void MaterialSerializer::serializeSpecularColor(const Material& material) {
        TVec3f specular = material.getSpecular();
        if (specular.x != 0.0f || specular.y != 0.0f || specular.z != 0.0f) {
            m_writer.startElement("specularColor", APPR_NS);
            
            std::ostringstream colorStream;
            colorStream << specular.x << " " << specular.y << " " << specular.z;
            
            m_writer.writeTextContent(colorStream.str());
            m_writer.endElement();
        }
    }

    void MaterialSerializer::serializeAmbientColor(const Material& material) {
        float ambientIntensity = material.getAmbientIntensity();
        if (ambientIntensity != 0.0f) {
            m_writer.startElement("ambientIntensity", APPR_NS);
            m_writer.writeTextContent(ambientIntensity);
            m_writer.endElement();
        }
    }

    void MaterialSerializer::serializeEmissionColor(const Material& material) {
        TVec3f emissive = material.getEmissive();
        if (emissive.x != 0.0f || emissive.y != 0.0f || emissive.z != 0.0f) {
            m_writer.startElement("emissiveColor", APPR_NS);
            
            std::ostringstream colorStream;
            colorStream << emissive.x << " " << emissive.y << " " << emissive.z;
            
            m_writer.writeTextContent(colorStream.str());
            m_writer.endElement();
        }
    }

    void MaterialSerializer::serializeShininess(const Material& material) {
        float shininess = material.getShininess();
        if (shininess != 0.0f) {
            m_writer.startElement("shininess", APPR_NS);
            m_writer.writeTextContent(shininess);
            m_writer.endElement();
        }
    }

    void MaterialSerializer::serializeTransparency(const Material& material) {
        float transparency = material.getTransparency();
        if (transparency != 0.0f) {
            m_writer.startElement("transparency", APPR_NS);
            m_writer.writeTextContent(transparency);
            m_writer.endElement();
        }
    }

    void MaterialSerializer::serializeIsSmooth(const Material& material) {
        bool isSmooth = material.isSmooth();
        m_writer.startElement("isSmooth", APPR_NS);
        m_writer.writeTextContent(isSmooth ? "true" : "false");
        m_writer.endElement();
    }

}
