#include "serializer/citygmlserializer.h"
#include "serializer/serializerconstants.h"
#include "serializer/cityobjectserializer.h"
#include "serializer/elementserializer.h"
#include <citygml/citymodel.h>
#include <citygml/envelope.h>
#include <citygml/appearancemanager.h>
#include <citygml/implictgeometry.h>
#include <citygml/transformmatrix.h>
#include <fstream>

namespace citygml {

    CityGMLSerializer::CityGMLSerializer(const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger)
        : m_writer(logger)
        , m_params(params)
        , m_logger(logger)
        , m_cityObjectSerializer(std::make_unique<CityObjectSerializer>(m_writer, params, logger))
    {
		// 初始化使用的数据类型集合
		m_usedNamespaces.clear();
		m_usedSchemaLocations.clear();
    }

    CityGMLSerializer::~CityGMLSerializer() {
    }

	void CityGMLSerializer::setupNamespaces() {
		// 动态设置命名空间 - 只设置实际使用的数据类型命名空间
		if (m_usedNamespaces.count(GML_NS) > 0) {
			m_writer.addNamespaceDeclaration(GML_NS, GML_NS_URI);
		}
		if (m_usedNamespaces.count(BLDG_NS) > 0) {
			m_writer.addNamespaceDeclaration(BLDG_NS, BLDG_NS_URI);
		}
		if (m_usedNamespaces.count(CORE_NS) > 0) {
			m_writer.addNamespaceDeclaration(CORE_NS, CORE_NS_URI);
		}
		if (m_usedNamespaces.count(FRN_NS) > 0) {
			m_writer.addNamespaceDeclaration(FRN_NS, FRN_NS_URI);
		}
		if (m_usedNamespaces.count(TRANS_NS) > 0) {
			m_writer.addNamespaceDeclaration(TRANS_NS, TRANS_NS_URI);
		}
		if (m_usedNamespaces.count(VEG_NS) > 0) {
			m_writer.addNamespaceDeclaration(VEG_NS, VEG_NS_URI);
		}
		if (m_usedNamespaces.count(WTR_NS) > 0) {
			m_writer.addNamespaceDeclaration(WTR_NS, WTR_NS_URI);
		}
		if (m_usedNamespaces.count(LUSE_NS) > 0) {
			m_writer.addNamespaceDeclaration(LUSE_NS, LUSE_NS_URI);
		}
		if (m_usedNamespaces.count(SUB_NS) > 0) {
			m_writer.addNamespaceDeclaration(SUB_NS, SUB_NS_URI);
		}
		if (m_usedNamespaces.count(BRID_NS) > 0) {
			m_writer.addNamespaceDeclaration(BRID_NS, BRID_NS_URI);
		}
		if (m_usedNamespaces.count(GRP_NS) > 0) {
			m_writer.addNamespaceDeclaration(GRP_NS, GRP_NS_URI);
		}
		if (m_usedNamespaces.count(DEM_NS) > 0) {
			m_writer.addNamespaceDeclaration(DEM_NS, DEM_NS_URI);
		}
		if (m_usedNamespaces.count(GEN_NS) > 0) {
			m_writer.addNamespaceDeclaration(GEN_NS, GEN_NS_URI);
		}
		if (m_usedNamespaces.count(APPR_NS) > 0) {
			m_writer.addNamespaceDeclaration(APPR_NS, APPR_NS_URI);
		}

		// 总是需要的命名空间
		m_writer.addNamespaceDeclaration(XAL_NS, XAL_NS_URI);
		m_writer.addNamespaceDeclaration(XSI_NS, XSI_NS_URI);
		m_writer.addNamespaceDeclaration(XLINK_NS, XLINK_NS_URI);

		// 设置默认命名空间
		m_writer.setDefaultNamespace(CITYGML_NS_URI);

		// 设置格式化选项
		m_writer.setPrettyPrint(m_params.prettyPrint);
		m_writer.setIndentSize(m_params.indentSize);

		// 设置schemaLocation
		setupSchemaLocation();
	}

	void CityGMLSerializer::setupSchemaLocation() {
		if (m_usedSchemaLocations.empty()) {
			return;
		}

		std::string schemaLocation;

		// 总是包含的基本schema
		//if (m_usedNamespaces.count("gml") > 0 || m_usedSchemaLocations.count("gml") > 0) {
		//	schemaLocation += GML_NS + " " + GML_NS_LOCATION + " ";
		//}

		// 根据使用的数据类型添加对应的schema
		if (m_usedSchemaLocations.count(BLDG_NS) > 0) {
			schemaLocation += BLDG_NS_URI + " " + BLDG_NS_LOCATION + " ";
		}
		if (m_usedSchemaLocations.count(FRN_NS) > 0) {
			schemaLocation += FRN_NS_URI + " " + FRN_NS_LOCATION + " ";
		}
		if (m_usedSchemaLocations.count(TRANS_NS) > 0) {
			schemaLocation += TRANS_NS_URI + " " + TRANS_NS_LOCATION + " ";
		}
		if (m_usedSchemaLocations.count(VEG_NS) > 0) {
			schemaLocation += VEG_NS_URI + " " + VEG_NS_LOCATION + " ";
		}
		if (m_usedSchemaLocations.count(WTR_NS) > 0) {
			schemaLocation += WTR_NS_URI + " " + WTR_NS_LOCATION + " ";
		}
		if (m_usedSchemaLocations.count(LUSE_NS) > 0) {
			schemaLocation += LUSE_NS_URI + " " + LUSE_NS_LOCATION + " ";
		}
		if (m_usedSchemaLocations.count(SUB_NS) > 0) {
			schemaLocation += SUB_NS_URI + " " + SUB_NS_LOCATION + " ";
		}
		if (m_usedSchemaLocations.count(BRID_NS) > 0) {
			schemaLocation += BRID_NS_URI + " " + BRID_NS_LOCATION + " ";
		}
		if (m_usedSchemaLocations.count(GRP_NS) > 0) {
			schemaLocation += GRP_NS_URI + " " + GRP_NS_LOCATION + " ";
		}
		if (m_usedSchemaLocations.count(DEM_NS) > 0) {
			schemaLocation += DEM_NS_URI + " " + DEM_NS_LOCATION + " ";
		}
		if (m_usedSchemaLocations.count(GEN_NS) > 0) {
			schemaLocation += GEN_NS_URI + " " + GEN_NS_LOCATION + " ";
		}
		if (m_usedSchemaLocations.count(APPR_NS) > 0) {
			schemaLocation += APPR_NS_URI + " " + APPR_NS_LOCATION + " ";
		}

		if (!schemaLocation.empty()) {
			m_writer.setSchemaLocation(schemaLocation);
		}
	}

    bool CityGMLSerializer::serialize(const CityModel& model, const std::string& filename) {
        try {
			// 收集使用的数据类型
			collectUsedTypes(model);
			collectAppearanceTypes();

			// 动态设置命名空间
			setupNamespaces();

            // 创建XML文档
            if (!m_writer.createDocument("CityModel", CITYGML_NS_URI)) {
                CITYGML_LOG_ERROR(m_logger, "Failed to create XML document");
                return false;
            }
            
            // 序列化CityModel
            serializeCityModel(model);
            
            // 保存到文件
            if (!m_writer.saveToFile(filename)) {
                CITYGML_LOG_ERROR(m_logger, "Failed to save XML to file: " + filename);
                return false;
            }
            
            CITYGML_LOG_INFO(m_logger, "Successfully serialized CityModel to: " + filename);
            return true;
            
        } catch (const std::exception& e) {
            CITYGML_LOG_ERROR(m_logger, "Exception during serialization: " + std::string(e.what()));
            return false;
        }
    }

    bool CityGMLSerializer::serialize(const CityModel& model, std::ostream& stream) {
        try {
			// 收集使用的数据类型
			collectUsedTypes(model);
			collectAppearanceTypes();

			// 动态设置命名空间
			setupNamespaces();

            // 创建XML文档
            if (!m_writer.createDocument("CityModel", CITYGML_NS_URI)) {
                CITYGML_LOG_ERROR(m_logger, "Failed to create XML document");
                return false;
            }
            
            // 序列化CityModel
            serializeCityModel(model);
            
            // 保存到流
            if (!m_writer.saveToStream(stream)) {
                CITYGML_LOG_ERROR(m_logger, "Failed to save XML to stream");
                return false;
            }
            
            CITYGML_LOG_INFO(m_logger, "Successfully serialized CityModel to stream");
            return true;
            
        } catch (const std::exception& e) {
            CITYGML_LOG_ERROR(m_logger, "Exception during serialization: " + std::string(e.what()));
            return false;
        }
    }

    void CityGMLSerializer::serializeCityModel(const CityModel& model) {
        // 写入ID属性
        //if (!model.getId().empty()) {
        //    m_writer.writeAttribute("gml:id", model.getId());
        //}
        
        // 写入SRS名称
        if (!model.getSRSName().empty()) {
            m_writer.writeAttribute("srsName", model.getSRSName());
        }
        
        // 序列化通用属性
		ElementSerializer::serializeAttributes(m_writer, m_params.includeGenericAttributes, model.getAttributes());
        
        // 序列化envelope
        serializeEnvelope(model);
        
        // 序列化主题
        //const std::vector<std::string>& themes = model.themes();
        //for (const std::string& theme : themes) {
        //    m_writer.startElement("theme", CITYGML_NS);
        //    m_writer.writeTextContent(theme);
        //    m_writer.endElement();
        //}
        
        // 序列化根CityObject
        serializeRootCityObjects(model);
    }

    void CityGMLSerializer::serializeEnvelope(const CityModel& model) {
        const Envelope& envelope = model.getEnvelope();
		const TVec3d& lowerCorner = envelope.getLowerBound(); // 序列化下界
		const TVec3d& upperCorner = envelope.getUpperBound(); // 序列化上界
		
		if (std::isnan(lowerCorner.x) || std::isnan(lowerCorner.y) || std::isnan(lowerCorner.z)
			|| std::isnan(upperCorner.x) || std::isnan(upperCorner.y) || std::isnan(upperCorner.z)) {
			return;
		}
        
        m_writer.startElement("boundedBy", GML_NS);
        m_writer.startElement("Envelope", GML_NS);
        
        // 写入SRS名称
        if (!envelope.srsName().empty()) {
            m_writer.writeAttribute("srsName", envelope.srsName());
        }
        
        // 序列化下界
        //const TVec3d& lowerCorner = envelope.getLowerBound();
        m_writer.startElement("lowerCorner", GML_NS);
        
        std::ostringstream lowerStream;
        lowerStream << lowerCorner.x << " " << lowerCorner.y << " " << lowerCorner.z;
        
        m_writer.writeTextContent(lowerStream.str());
        m_writer.endElement();
        
        // 序列化上界
        //const TVec3d& upperCorner = envelope.getUpperBound();
        m_writer.startElement("upperCorner", GML_NS);
        
        std::ostringstream upperStream;
        upperStream << upperCorner.x << " " << upperCorner.y << " " << upperCorner.z;
        
        m_writer.writeTextContent(upperStream.str());
        m_writer.endElement();
        
        m_writer.endElement(); // Envelope
        m_writer.endElement(); // boundedBy
    }

    void CityGMLSerializer::serializeRootCityObjects(const CityModel& model) {
        const ConstCityObjects& rootObjects = model.getRootCityObjects();
        
        for (const CityObject* cityObject : rootObjects) {
            if (cityObject) {
				m_writer.startElement("cityObjectMember", CITYGML_NS_URI);
                m_cityObjectSerializer->serialize(*cityObject);
				m_writer.endElement();
            }
        }
    }

	void CityGMLSerializer::collectUsedTypes(const CityModel& model) {
		// 收集根对象的类型
		const ConstCityObjects& rootObjects = model.getRootCityObjects();
		for (const CityObject* cityObject : rootObjects) {
			if (cityObject) {
				collectObjectTypes(*cityObject);
			}
		}

		// 总是需要的命名空间
		m_usedNamespaces.insert(GML_NS);
		m_usedNamespaces.insert(CORE_NS);
		m_usedNamespaces.insert(GEN_NS);

		m_usedNamespaces.insert(XSI_NS);
		m_usedNamespaces.insert(XAL_NS);
		m_usedNamespaces.insert(XLINK_NS);

		m_usedSchemaLocations.insert(GEN_NS);
	}

	void CityGMLSerializer::collectObjectTypes(const CityObject& cityObject) {
		// 根据对象类型添加对应的命名空间
		CityObject::CityObjectsType type = cityObject.getType();
		std::string namespaceURI = m_cityObjectSerializer->getNamespaceForCityObjectType(type);
		m_usedNamespaces.insert(namespaceURI);
		m_usedSchemaLocations.insert(namespaceURI);

		// 递归收集子对象
		unsigned int childCount = cityObject.getChildCityObjectsCount();
		for (unsigned int i = 0; i < childCount; ++i) {
			const CityObject& childObject = cityObject.getChildCityObject(i);
			collectObjectTypes(childObject);
		}

		// 检查是否有地址信息
		if (cityObject.address()) {
			m_usedNamespaces.insert(XAL_NS);
		}
	}

	void CityGMLSerializer::collectAppearanceTypes() {
		// 检查是否有外观信息
		m_usedNamespaces.insert(APPR_NS);
		m_usedSchemaLocations.insert(APPR_NS);
	}
}
