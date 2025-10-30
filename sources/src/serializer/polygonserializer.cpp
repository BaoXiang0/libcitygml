#include "serializer/polygonserializer.h"
#include "serializer/linearringserializer.h"
#include <citygml/polygon.h>
#include <citygml/appearancetarget.h>

namespace citygml {

    PolygonSerializer::PolygonSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger)
        : ElementSerializer(writer, params, logger)
    {
    }

    void PolygonSerializer::serialize(const Polygon& polygon) {
        // 开始Polygon元素
        m_writer.startElement("Polygon", GML_NS);
        
        // 写入ID属性
        if (!polygon.getId().empty()) {
            m_writer.writeAttribute("gml:id", polygon.getId());
        }
        
        // 序列化外环
        if (polygon.exteriorRing()) {
            serializeExteriorRing(*polygon.exteriorRing());
        }
        
        // 序列化内环
        serializeInteriorRings(polygon.interiorRings());
        
        // 结束Polygon元素
        m_writer.endElement();
    }

    void PolygonSerializer::serializeExteriorRing(const LinearRing& exteriorRing) {
        m_writer.startElement("exterior", GML_NS);
        
        LinearRingSerializer ringSerializer(m_writer, m_params, m_logger);
        ringSerializer.serialize(exteriorRing);
        
        m_writer.endElement();
    }

    void PolygonSerializer::serializeInteriorRings(const std::vector<std::shared_ptr<LinearRing>>& interiorRings) {
        for (const auto& ring : interiorRings) {
            if (ring) {
                m_writer.startElement("interior", GML_NS);
                
                LinearRingSerializer ringSerializer(m_writer, m_params, m_logger);
                ringSerializer.serialize(*ring);
                
                m_writer.endElement();
            }
        }
    }
}
