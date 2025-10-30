#include "serializer/geometryserializer.h"
#include "serializer/polygonserializer.h"
#include "serializer/linestringserializer.h"
#include <citygml/geometry.h>
#include <citygml/polygon.h>
#include <citygml/linestring.h>

namespace citygml {

    GeometrySerializer::GeometrySerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger)
        : ElementSerializer(writer, params, logger)
    {
    }

    void GeometrySerializer::serialize(const Geometry& geometry) {
        // 获取几何体元素名称
        std::string elementName = getGeometryElementName(geometry);
        
        // 开始几何体元素
        m_writer.startElement(elementName, GML_NS);
        
        // 写入ID属性
        if (!geometry.getId().empty()) {
            m_writer.writeAttribute("gml:id", geometry.getId());
        }

		bool hasExterior = elementName == "Solid"; // Geometry中未保留几何类型信息，暂时这样处理
		if (hasExterior) {
			m_writer.startElement("exterior", GML_NS);
		}
        
        // 序列化LOD信息
        //if (m_params.includeLODInfo) {
        //    serializeLODInfo(geometry);
        //}
        
        // 序列化多边形
        serializePolygons(geometry);
        
        // 序列化线串
        serializeLineStrings(geometry);
        
        // 序列化子几何体
        serializeChildGeometries(geometry);
        
		if (hasExterior) {
			m_writer.endElement();
		}

        // 结束几何体元素
        m_writer.endElement();
    }

    /*void GeometrySerializer::serializeLODInfo(const Geometry& geometry) {
        // 写入LOD属性
        m_writer.writeAttribute("lod", (int)geometry.getLOD());
        
        // 写入SRS名称
        if (!geometry.getSRSName().empty()) {
            m_writer.writeAttribute("srsName", geometry.getSRSName());
        }
    }*/

    void GeometrySerializer::serializePolygons(const Geometry& geometry) {
        unsigned int polygonCount = geometry.getPolygonsCount();
        for (unsigned int i = 0; i < polygonCount; ++i) {
            auto polygon = geometry.getPolygon(i);
            if (polygon) {
				m_writer.startElement("surfaceMember", GML_NS);
                PolygonSerializer polygonSerializer(m_writer, m_params, m_logger);
                polygonSerializer.serialize(*polygon);
				m_writer.endElement();
            }
        }
    }

    void GeometrySerializer::serializeLineStrings(const Geometry& geometry) {
        unsigned int lineStringCount = geometry.getLineStringCount();
        for (unsigned int i = 0; i < lineStringCount; ++i) {
            auto lineString = geometry.getLineString(i);
            if (lineString) {
                LineStringSerializer lineStringSerializer(m_writer, m_params, m_logger);
                lineStringSerializer.serialize(*lineString);
            }
        }
    }

    void GeometrySerializer::serializeChildGeometries(const Geometry& geometry) {
        unsigned int geometryCount = geometry.getGeometriesCount();
        for (unsigned int i = 0; i < geometryCount; ++i) {
            const Geometry& childGeometry = geometry.getGeometry(i);
            serialize(childGeometry);
        }
    }

    std::string GeometrySerializer::getGeometryElementName(const Geometry& geometry) const {
		// 判断几何体的GML元素类型

		// 检查是否有子几何体（如果是Solid，应该有子几何体）
		if (geometry.getGeometriesCount() > 0) {
			// 如果有子几何体，可能是Solid
			return "Solid";
		}

		// 检查多边形数量
		unsigned int polygonCount = geometry.getPolygonsCount();
		if (polygonCount > 1) {
			// 多个多边形，可能是CompositeSurface
			return "CompositeSurface";
		}
		else if (polygonCount == 1) {
			// 单个多边形
			return "MultiSurface";
		}

		// 检查线串数量
		unsigned int lineStringCount = geometry.getLineStringCount();
		if (lineStringCount > 1) {
			return "CompositeCurve";
		}
		else if (lineStringCount == 1) {
			return "LineString";
		}

		// 根据几何类型决定
		Geometry::GeometryType type = geometry.getType();
		switch (type) {
		case Geometry::GeometryType::GT_Roof:
		case Geometry::GeometryType::GT_Wall:
		case Geometry::GeometryType::GT_Ground:
		case Geometry::GeometryType::GT_Closure:
		case Geometry::GeometryType::GT_Floor:
		case Geometry::GeometryType::GT_InteriorWall:
		case Geometry::GeometryType::GT_Ceiling:
		case Geometry::GeometryType::GT_OuterCeiling:
		case Geometry::GeometryType::GT_OuterFloor:
			return "MultiSurface";

		case Geometry::GeometryType::GT_Tin:
			return "TINRelief";

		default:
			// 如果什么都没有，检查是否有内容
			if (polygonCount > 0 || lineStringCount > 0 || geometry.getGeometriesCount() > 0) {
				return "Geometry";
			}
			return "Geometry";
		}
    }

}
