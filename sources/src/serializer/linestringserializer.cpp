#include "serializer/linestringserializer.h"
#include <citygml/linestring.h>
#include <sstream>

namespace citygml {

    LineStringSerializer::LineStringSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger)
        : ElementSerializer(writer, params, logger)
    {
    }

    void LineStringSerializer::serialize(const LineString& lineString) {
        // 开始LineString元素
        m_writer.startElement("LineString", GML_NS);
        
        // 写入ID属性
        if (!lineString.getId().empty()) {
            m_writer.writeAttribute("gml:id", lineString.getId());
        }
        
        // 序列化坐标点
        const std::vector<TVec3d>& vertices = lineString.getVertices3D();
        if (!vertices.empty()) {
            serializePosList(vertices);
        }
        
        // 结束LineString元素
        m_writer.endElement();
    }

    void LineStringSerializer::serializePosList(const std::vector<TVec3d>& vertices) {
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

}
