#include "serializer/elementserializer.h"
#include <citygml/cityobject.h>
#include <citygml/geometry.h>
#include <citygml/attributesmap.h>
#include <parser/nodetypes.h>

namespace citygml {

    ElementSerializer::ElementSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger)
        : m_writer(writer)
        , m_params(params)
        , m_logger(logger)
        , m_hasError(false)
    {
    }

	void ElementSerializer::serializeAttributes(const AttributesMap& attributes) {
		serializeAttributes(m_writer, m_params.includeGenericAttributes, attributes);
	}

    /*std::string ElementSerializer::getElementNameForGeometryType(Geometry::GeometryType type) const {
        switch (type) {
            case Geometry::GeometryType::GT_Roof:
                return "RoofSurface";
            case Geometry::GeometryType::GT_Wall:
                return "WallSurface";
            case Geometry::GeometryType::GT_Ground:
                return "GroundSurface";
            case Geometry::GeometryType::GT_Closure:
                return "ClosureSurface";
            case Geometry::GeometryType::GT_Floor:
                return "FloorSurface";
            case Geometry::GeometryType::GT_InteriorWall:
                return "InteriorWallSurface";
            case Geometry::GeometryType::GT_Ceiling:
                return "CeilingSurface";
            case Geometry::GeometryType::GT_OuterCeiling:
                return "OuterCeilingSurface";
            case Geometry::GeometryType::GT_OuterFloor:
                return "OuterFloorSurface";
            case Geometry::GeometryType::GT_Tin:
                return "TINRelief";
            default:
                return "Geometry";
        }
    }*/

    bool ElementSerializer::hasError() const {
        return m_hasError || m_writer.hasError();
    }

    std::string ElementSerializer::getLastError() const {
        if (m_hasError) {
            return m_lastError;
        }
        return m_writer.getLastError();
    }

    void ElementSerializer::setError(const std::string& error) {
        m_lastError = error;
        m_hasError = true;
        CITYGML_LOG_ERROR(m_logger, error);
    }

	void ElementSerializer::serializeAttributes(XMLWriter& writer, bool includeGenericAttributes, const AttributesMap& attributes)
	{
		for (const auto& attr : attributes) {
			const NodeType::XMLNode& node = NodeType::getXMLNodeFor(attr.first);
			if (node.valid()) {
				// 开始元素
				writer.startElement(node.baseName(), node.prefix());

				// 根据类型写入值
				const AttributeValue& value = attr.second;
				switch (value.getType()) {
				case AttributeType::String:
					writer.writeTextContent(value.asString());
					break;
				case AttributeType::Double:
					writer.writeTextContent(value.asDouble());
					break;
				case AttributeType::Integer:
					writer.writeTextContent(value.asInteger());
					break;
				case AttributeType::Date:
				case AttributeType::Uri:
					writer.writeTextContent(value.asString());
					break;
				}

				// 结束元素
				writer.endElement();
			}
			else {
				if (includeGenericAttributes) {
					serializeGenericAttribute(writer, attr.first, attr.second);
				}
			}
		}
	}
	
	void ElementSerializer::serializeGenericAttribute(XMLWriter& writer, const std::string& name, const AttributeValue& value)
	{
		// 根据属性类型选择正确的元素名称
		std::string elementName;
		switch (value.getType()) {
		case AttributeType::String:
			elementName = "stringAttribute";
			break;
		case AttributeType::Double:
			elementName = "doubleAttribute";
			break;
		case AttributeType::Integer:
			elementName = "intAttribute";
			break;
		case AttributeType::Date:
			elementName = "dateAttribute";
			break;
		case AttributeType::Uri:
			elementName = "uriAttribute";
			break;
		default:
			elementName = "stringAttribute";
			break;
		}

		// 开始属性元素
		writer.startElement(elementName, GEN_NS);

		// 写入name属性
		writer.writeAttribute("name", name);

		// 开始value元素
		writer.startElement("value", GEN_NS);

		// 根据类型写入值
		switch (value.getType()) {
		case AttributeType::String:
			writer.writeTextContent(value.asString());
			break;
		case AttributeType::Double:
			writer.writeTextContent(value.asDouble());
			break;
		case AttributeType::Integer:
			writer.writeTextContent(value.asInteger());
			break;
		case AttributeType::Date:
		case AttributeType::Uri:
			writer.writeTextContent(value.asString());
			break;
		}

		// 结束value元素
		writer.endElement();

		// 结束属性元素
		writer.endElement();
	}

}
