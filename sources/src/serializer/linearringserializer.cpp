#include "serializer/linearringserializer.h"
#include <citygml/linearring.h>
#include <sstream>

namespace citygml {

    LinearRingSerializer::LinearRingSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger)
        : ElementSerializer(writer, params, logger)
    {
    }

    void LinearRingSerializer::serialize(const LinearRing& linearRing) {
        // 开始LinearRing元素
        m_writer.startElement("LinearRing", GML_NS);
        
        // 写入ID属性
        if (!linearRing.getId().empty()) {
            m_writer.writeAttribute("gml:id", linearRing.getId());
        }
        
        // 序列化坐标点
        const std::vector<TVec3d>& vertices = linearRing.getVertices();
        if (!vertices.empty()) {
            serializePosList(vertices);
        }
        
        // 结束LinearRing元素
        m_writer.endElement();
    }

    void LinearRingSerializer::serializePosList(const std::vector<TVec3d>& vertices) {
        m_writer.startElement("posList", GML_NS);
        
		// 写入srsDimension属性
		m_writer.writeAttribute("srsDimension", 3);

        // 构建坐标字符串
        std::ostringstream coordStream;
		coordStream.precision(std::numeric_limits<double>::digits10);
        for (size_t i = 0; i < vertices.size(); ++i) {
            const TVec3d& vertex = vertices[i];
            coordStream << vertex.x << " " << vertex.y << " " << vertex.z;
            if (i < vertices.size() - 1) {
                coordStream << " ";
            }
        }
        
        m_writer.writeTextContent(coordStream.str());
        m_writer.endElement();
    }

    void LinearRingSerializer::serializePos(const TVec3d& vertex) {
        m_writer.startElement("pos", GML_NS);

		// 写入srsDimension属性
		m_writer.writeAttribute("srsDimension", 3);
        
        std::ostringstream coordStream;
		coordStream.precision(std::numeric_limits<double>::digits10);
        coordStream << vertex.x << " " << vertex.y << " " << vertex.z;
        
        m_writer.writeTextContent(coordStream.str());
        m_writer.endElement();
    }

}
