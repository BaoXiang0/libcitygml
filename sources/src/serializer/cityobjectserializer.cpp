#include "serializer/cityobjectserializer.h"
#include "serializer/geometryserializer.h"
#include "serializer/addressserializer.h"
#include "serializer/materialtargetdefinitionserializer.h"
#include "serializer/texturetargetdefinitionserializer.h"
#include <citygml/cityobject.h>
#include <citygml/geometry.h>
#include <citygml/implictgeometry.h>
#include <citygml/address.h>
#include <citygml/externalreference.h>
#include <citygml/rectifiedgridcoverage.h>
#include <citygml/texture.h>
#include <citygml/material.h>
#include <citygml/polygon.h>
#include <set>

namespace citygml {

    CityObjectSerializer::CityObjectSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger)
        : ElementSerializer(writer, params, logger)
    {
    }

	void CityObjectSerializer::serialize(const CityObject& cityObject) {
		serialize(cityObject, true);
	}

	void CityObjectSerializer::serialize(const CityObject& cityObject, bool isRoot)  {
        // 获取元素名称和命名空间
		std::string elementName = cityObjectsTypeToString(cityObject.getType());
        std::string namespaceURI = getNamespaceForCityObjectType(cityObject.getType());
        
        // 开始CityObject元素
        m_writer.startElement(elementName, namespaceURI);
        
        // 写入ID属性
        if (!cityObject.getId().empty()) {
            m_writer.writeAttribute("gml:id", cityObject.getId());
        }
        
        // 序列化属性
		serializeAttributes(cityObject.getAttributes());

		// 序列化外观信息
		if (m_params.includeAppearances && isRoot) {
			serializeAppearances(cityObject);
		}
        
        // 序列化几何体
        serializeGeometries(cityObject);
        
        // 序列化隐式几何体
        //serializeImplicitGeometries(cityObject); // 实现有问题
        
        // 序列化子CityObject
        serializeChildCityObjects(cityObject);
        
        // 序列化地址
        if (m_params.includeAddresses) {
            serializeAddress(cityObject);
        }
        
        // 序列化外部引用
        if (m_params.includeExternalReferences) {
            serializeExternalReference(cityObject);
            serializeRectifiedGridCoverage(cityObject);
        }
        
        // 结束CityObject元素
        m_writer.endElement();
    }

    void CityObjectSerializer::serializeGeometries(const CityObject& cityObject) {
        unsigned int geometryCount = cityObject.getGeometriesCount();
        for (unsigned int i = 0; i < geometryCount; ++i) {
            const Geometry& geometry = cityObject.getGeometry(i);
            
            // 检查是否包含空几何体
            if (!m_params.includeEmptyGeometries && 
                geometry.getPolygonsCount() == 0 && 
                geometry.getLineStringCount() == 0 &&
                geometry.getGeometriesCount() == 0) {
                continue;
            }
            
            //GeometrySerializer geometrySerializer(m_writer, m_params, m_logger);
            //geometrySerializer.serialize(geometry);

			// 根据CityObject类型和LOD级别确定容器元素名
			std::string containerName = getLODContainerName(cityObject.getType(), geometry.getLOD());
			if (containerName.empty()) continue;

			// 开始LOD几何容器
			m_writer.startElement(containerName, getNamespaceForCityObjectType(cityObject.getType()));

			GeometrySerializer geometrySerializer(m_writer, m_params, m_logger);
			geometrySerializer.serialize(geometry);

			// 结束LOD几何容器
			m_writer.endElement();
        }
    }

	std::string CityObjectSerializer::getLODContainerName(CityObject::CityObjectsType type, unsigned int lod) const {
		// 根据对象类型和LOD级别返回容器元素名
		switch (type) {
		case CityObject::CityObjectsType::COT_Building:
		case CityObject::CityObjectsType::COT_BuildingPart:
			switch (lod) {
			case 0: return "lod0FootPrint";
			case 1: return "lod1Solid";
			case 2: return "lod2Solid";
			case 3: return "lod3Solid";
			case 4: return "lod4Solid";
			default: return "";
			}
			break;
		case CityObject::CityObjectsType::COT_WallSurface:
		case CityObject::CityObjectsType::COT_RoofSurface:
		case CityObject::CityObjectsType::COT_GroundSurface:
		case CityObject::CityObjectsType::COT_ClosureSurface:
		case CityObject::CityObjectsType::COT_FloorSurface:
		case CityObject::CityObjectsType::COT_InteriorWallSurface:
		case CityObject::CityObjectsType::COT_CeilingSurface:
			switch (lod) {
			case 0: return "lod0FootPrint";
			case 1: return "lod1MultiSurface";
			case 2: return "lod2MultiSurface";
			case 3: return "lod3MultiSurface";
			case 4: return "lod4MultiSurface";
			default: return "";
			}
			break;
		default:
			return ""; // 其他类型不处理LOD容器
		}
	}

    void CityObjectSerializer::serializeImplicitGeometries(const CityObject& cityObject) {
        unsigned int implicitGeometryCount = cityObject.getImplicitGeometryCount();
        for (unsigned int i = 0; i < implicitGeometryCount; ++i) {
            const ImplicitGeometry& implicitGeometry = cityObject.getImplicitGeometry(i);
            
            // 开始ImplicitGeometry元素
            m_writer.startElement("ImplicitGeometry", CORE_NS);
            
            // 写入ID属性
            if (!implicitGeometry.getId().empty()) {
                m_writer.writeAttribute("gml:id", implicitGeometry.getId());
            }
            
            // 写入LOD属性
            //if (m_params.includeLODInfo) {
            //    m_writer.writeAttribute("lod", implicitGeometry.getLOD());
            //}
            
            // 写入SRS名称
            if (!implicitGeometry.getSRSName().empty()) {
                m_writer.writeAttribute("srsName", implicitGeometry.getSRSName());
            }
            
            // 序列化变换矩阵
            const TransformationMatrix& transformMatrix = implicitGeometry.getTransformMatrix();
            const double* matrix = transformMatrix.getMatrix();
            
            // 检查是否是单位矩阵
            bool isIdentity = true;
            for (int i = 0; i < 16; ++i) {
                if ((i % 5 == 0 && matrix[i] != 1.0) || (i % 5 != 0 && matrix[i] != 0.0)) {
                    isIdentity = false;
                    break;
                }
            }
            
            if (!isIdentity) {
                m_writer.startElement("transformationMatrix", CORE_NS);
                
                std::ostringstream matrixStream;
                for (int i = 0; i < 16; ++i) {
                    matrixStream << matrix[i];
                    if (i < 15) matrixStream << " ";
                }
                
                m_writer.writeTextContent(matrixStream.str());
                m_writer.endElement();
            }
            
            // 序列化参考点
            const TVec3d& referencePoint = implicitGeometry.getReferencePoint();
            if (referencePoint.x != 0.0 || referencePoint.y != 0.0 || referencePoint.z != 0.0) {
                m_writer.startElement("referencePoint", CORE_NS);
                
                std::ostringstream pointStream;
                pointStream << referencePoint.x << " " << referencePoint.y << " " << referencePoint.z;
                
                m_writer.writeTextContent(pointStream.str());
                m_writer.endElement();
            }
            
            // 序列化几何体
            unsigned int geometryCount = implicitGeometry.getGeometriesCount();
            for (unsigned int i = 0; i < geometryCount; ++i) {
                const Geometry& geometry = implicitGeometry.getGeometry(i);
                GeometrySerializer geometrySerializer(m_writer, m_params, m_logger);
                geometrySerializer.serialize(geometry);
            }
            
            // 结束ImplicitGeometry元素
            m_writer.endElement();
        }
    }

    void CityObjectSerializer::serializeChildCityObjects(const CityObject& cityObject) {
        unsigned int childCount = cityObject.getChildCityObjectsCount();
        for (unsigned int i = 0; i < childCount; ++i) {
			m_writer.startElement("boundedBy", BLDG_NS);
            const CityObject& childObject = cityObject.getChildCityObject(i);
            serialize(childObject, false);
			m_writer.endElement();
        }
    }

    void CityObjectSerializer::serializeAddress(const CityObject& cityObject) {
        const Address* address = cityObject.address();
        if (address) {
			m_writer.startElement("address", BLDG_NS);
            AddressSerializer addressSerializer(m_writer, m_params, m_logger);
            addressSerializer.serialize(*address);
			m_writer.endElement();
        }
    }

    void CityObjectSerializer::serializeExternalReference(const CityObject& cityObject) {
        const ExternalReference* externalRef = cityObject.externalReference();
        if (externalRef) {
            // 开始ExternalReference元素
            m_writer.startElement("ExternalReference", CORE_NS);
            
            // 写入ID属性
            if (!externalRef->getId().empty()) {
                m_writer.writeAttribute("gml:id", externalRef->getId());
            }
            
            // 序列化信息
            if (!externalRef->informationSystem.empty()) {
                m_writer.startElement("informationSystem", CORE_NS);
                m_writer.writeTextContent(externalRef->informationSystem);
                m_writer.endElement();
            }
            
            // 序列化外部对象引用
            if (!externalRef->externalObject.name.empty()) {
                m_writer.startElement("externalObject", CORE_NS);
                m_writer.startElement("name", CORE_NS);
                m_writer.writeTextContent(externalRef->externalObject.name);
                m_writer.endElement();
                m_writer.endElement();
            } else if (!externalRef->externalObject.uri.empty()) {
                m_writer.startElement("externalObject", CORE_NS);
                m_writer.startElement("uri", CORE_NS);
                m_writer.writeTextContent(externalRef->externalObject.uri);
                m_writer.endElement();
                m_writer.endElement();
            }
            
            // 结束ExternalReference元素
            m_writer.endElement();
        }
    }

    void CityObjectSerializer::serializeRectifiedGridCoverage(const CityObject& cityObject) {
        const RectifiedGridCoverage* gridCoverage = cityObject.rectifiedGridCoverage();
        if (gridCoverage) {
            // 开始RectifiedGridCoverage元素
            m_writer.startElement("RectifiedGridCoverage", CORE_NS);
            
            // 写入ID属性
            if (!gridCoverage->getId().empty()) {
                m_writer.writeAttribute("gml:id", gridCoverage->getId());
            }
            
            // RectifiedGridCoverage类比较简单，只序列化基本信息和envelope
            const Envelope& envelope = gridCoverage->getEnvelope();
            const TVec3d& lowerBound = envelope.getLowerBound();
            const TVec3d& upperBound = envelope.getUpperBound();
            if (lowerBound.x != 0.0 || lowerBound.y != 0.0 || lowerBound.z != 0.0 ||
                upperBound.x != 0.0 || upperBound.y != 0.0 || upperBound.z != 0.0) {
                m_writer.startElement("boundedBy", GML_NS);
                m_writer.startElement("Envelope", GML_NS);
                
                // 写入SRS名称
                if (!envelope.srsName().empty()) {
                    m_writer.writeAttribute("srsName", envelope.srsName());
                }
                
                // 序列化下界
                const TVec3d& lowerCorner = envelope.getLowerBound();
                m_writer.startElement("lowerCorner", GML_NS);
                
                std::ostringstream lowerStream;
                lowerStream << lowerCorner.x << " " << lowerCorner.y << " " << lowerCorner.z;
                
                m_writer.writeTextContent(lowerStream.str());
                m_writer.endElement();
                
                // 序列化上界
                const TVec3d& upperCorner = envelope.getUpperBound();
                m_writer.startElement("upperCorner", GML_NS);
                
                std::ostringstream upperStream;
                upperStream << upperCorner.x << " " << upperCorner.y << " " << upperCorner.z;
                
                m_writer.writeTextContent(upperStream.str());
                m_writer.endElement();
                
                m_writer.endElement(); // Envelope
                m_writer.endElement(); // boundedBy
            }
            
            // 结束RectifiedGridCoverage元素
            m_writer.endElement();
        }
    }

    std::string CityObjectSerializer::getNamespaceForCityObjectType(CityObject::CityObjectsType type) {
        switch (type) {
            case CityObject::CityObjectsType::COT_Building:
            case CityObject::CityObjectsType::COT_Room:
            case CityObject::CityObjectsType::COT_BuildingInstallation:
            case CityObject::CityObjectsType::COT_BuildingFurniture:
            case CityObject::CityObjectsType::COT_Door:
            case CityObject::CityObjectsType::COT_Window:
            case CityObject::CityObjectsType::COT_BuildingPart:
            case CityObject::CityObjectsType::COT_WallSurface:
            case CityObject::CityObjectsType::COT_RoofSurface:
            case CityObject::CityObjectsType::COT_GroundSurface:
            case CityObject::CityObjectsType::COT_ClosureSurface:
            case CityObject::CityObjectsType::COT_FloorSurface:
            case CityObject::CityObjectsType::COT_InteriorWallSurface:
            case CityObject::CityObjectsType::COT_CeilingSurface:
            case CityObject::CityObjectsType::COT_OuterCeilingSurface:
            case CityObject::CityObjectsType::COT_OuterFloorSurface:
            case CityObject::CityObjectsType::COT_IntBuildingInstallation:
                return BLDG_NS;
                
            case CityObject::CityObjectsType::COT_CityFurniture:
                return FRN_NS;
                
            case CityObject::CityObjectsType::COT_Track:
            case CityObject::CityObjectsType::COT_Road:
            case CityObject::CityObjectsType::COT_Railway:
            case CityObject::CityObjectsType::COT_Square:
            case CityObject::CityObjectsType::COT_TransportationObject:
                return TRANS_NS;
                
            case CityObject::CityObjectsType::COT_PlantCover:
            case CityObject::CityObjectsType::COT_SolitaryVegetationObject:
                return VEG_NS;
                
            case CityObject::CityObjectsType::COT_WaterBody:
                return WTR_NS;
                
            case CityObject::CityObjectsType::COT_LandUse:
                return LUSE_NS;
                
            case CityObject::CityObjectsType::COT_Tunnel:
                return SUB_NS;
                
            case CityObject::CityObjectsType::COT_Bridge:
            case CityObject::CityObjectsType::COT_BridgeConstructionElement:
            case CityObject::CityObjectsType::COT_BridgeInstallation:
            case CityObject::CityObjectsType::COT_BridgePart:
                return BRID_NS;
                
            case CityObject::CityObjectsType::COT_CityObjectGroup:
                return GRP_NS;
                
            case CityObject::CityObjectsType::COT_ReliefFeature:
            case CityObject::CityObjectsType::COT_ReliefComponent:
            case CityObject::CityObjectsType::COT_TINRelief:
            case CityObject::CityObjectsType::COT_MassPointRelief:
            case CityObject::CityObjectsType::COT_BreaklineRelief:
            case CityObject::CityObjectsType::COT_RasterRelief:
                return DEM_NS;
                
            case CityObject::CityObjectsType::COT_GenericCityObject:
            default:
                return GEN_NS;
        }
    }

	void CityObjectSerializer::serializeAppearances(const CityObject& cityObject) {
		std::map<std::string, std::vector<std::shared_ptr<const MaterialTargetDefinition>>> categorizedMaterialDefs;
		std::map<std::string, std::vector<std::shared_ptr<const TextureTargetDefinition>>> categorizedTextureDefs;

		// 递归函数，用于收集Geometry及其子Geometry、Polygon的外观定义，并按主题分类
		std::function<void(const AppearanceTarget&)> collectAppearanceDefs =
			[&](const AppearanceTarget& currentTarget) {
			// 收集材质目标定义
			std::vector<std::string> matThemesFront = currentTarget.getAllMaterialThemes(true);
			for (const std::string& theme : matThemesFront) {
				if (auto materialTarget = currentTarget.getMaterialTargetDefinitionForTheme(theme, true)) {
					categorizedMaterialDefs[theme].push_back(materialTarget);
				}
			}
			std::vector<std::string> matThemesBack = currentTarget.getAllMaterialThemes(false);
			for (const std::string& theme : matThemesBack) {
				if (auto materialTarget = currentTarget.getMaterialTargetDefinitionForTheme(theme, false)) {
					categorizedMaterialDefs[theme].push_back(materialTarget);
				}
			}

			// 收集纹理目标定义
			std::vector<std::string> texThemesFront = currentTarget.getAllTextureThemes(true);
			for (const std::string& theme : texThemesFront) {
				if (auto textureTarget = currentTarget.getTextureTargetDefinitionForTheme(theme, true)) {
					categorizedTextureDefs[theme].push_back(textureTarget);
				}
			}
			std::vector<std::string> texThemesBack = currentTarget.getAllTextureThemes(false);
			for (const std::string& theme : texThemesBack) {
				if (auto textureTarget = currentTarget.getTextureTargetDefinitionForTheme(theme, false)) {
					categorizedTextureDefs[theme].push_back(textureTarget);
				}
			}

			// 如果当前对象是 Geometry，则递归遍历其子 Geometry 和 Polygon
			if (const Geometry* geometry = dynamic_cast<const Geometry*>(&currentTarget)) {
				unsigned int childGeomCount = geometry->getGeometriesCount();
				for (unsigned int k = 0; k < childGeomCount; ++k) {
					collectAppearanceDefs(geometry->getGeometry(k));
				}

				unsigned int polygonCount = geometry->getPolygonsCount();
				for (unsigned int k = 0; k < polygonCount; ++k) {
					if (auto polygon = geometry->getPolygon(k)) {
						collectAppearanceDefs(*polygon);
					}
				}
			}
		};

		std::function<void(const CityObject&)> collectCityObjectAppearanceDefs =
			[&collectCityObjectAppearanceDefs, &collectAppearanceDefs](const CityObject& currentCityObject) {
			unsigned int geometryCount = currentCityObject.getGeometriesCount();
			for (unsigned int i = 0; i < geometryCount; ++i) {
				collectAppearanceDefs(currentCityObject.getGeometry(i));
			}

			unsigned int childCount = currentCityObject.getChildCityObjectsCount();
			for (unsigned int ichild = 0; ichild < childCount; ++ichild) {
				const CityObject& childObject = currentCityObject.getChildCityObject(ichild);
				collectCityObjectAppearanceDefs(childObject);
			}
		};

		collectCityObjectAppearanceDefs(cityObject);

		bool hasAnyAppearance = !categorizedMaterialDefs.empty() || !categorizedTextureDefs.empty();

		if (hasAnyAppearance) {
			m_writer.startElement("appearance", APPR_NS);

			// 获取所有唯一的主题
			std::set<std::string> uniqueThemes; // 使用 std::set 自动排序并去重
			for (const auto& pair : categorizedMaterialDefs) {
				uniqueThemes.insert(pair.first);
			}
			for (const auto& pair : categorizedTextureDefs) {
				uniqueThemes.insert(pair.first);
			}

			// 遍历每个唯一的主题，为每个主题创建一个 <app:Appearance> 块
			for (const std::string& theme : uniqueThemes) {
				m_writer.startElement("Appearance", APPR_NS);
				// 为 Appearance 元素生成一个唯一的 gml:id
				//m_writer.writeAttribute("gml:id", cityObject.getId() + "_app_" + theme);

				// 输出 <app:theme> 子元素
				m_writer.startElement("theme", APPR_NS);
				m_writer.writeTextContent(theme);
				m_writer.endElement(); // theme

				std::unordered_set<std::string> serializedAppearanceIDsForTheme; // 用于避免在当前主题内重复序列化 Appearance 对象

																				 // 序列化当前主题下的材质目标定义
				if (categorizedMaterialDefs.count(theme)) {
					for (const auto& materialTargetDef : categorizedMaterialDefs[theme]) {
						if (serializedAppearanceIDsForTheme.find(materialTargetDef->getAppearance()->getId()) == serializedAppearanceIDsForTheme.end()) {
							MaterialTargetDefinitionSerializer materialTargetDefSerializer(m_writer, m_params, m_logger);
							materialTargetDefSerializer.serialize(*materialTargetDef);
							serializedAppearanceIDsForTheme.insert(materialTargetDef->getAppearance()->getId());
						}
					}
				}

				// 序列化当前主题下的纹理目标定义
				if (categorizedTextureDefs.count(theme)) {
					for (const auto& textureTargetDef : categorizedTextureDefs[theme]) {
						if (serializedAppearanceIDsForTheme.find(textureTargetDef->getAppearance()->getId()) == serializedAppearanceIDsForTheme.end()) {
							TextureTargetDefinitionSerializer textureTargetDefSerializer(m_writer, m_params, m_logger);
							textureTargetDefSerializer.serialize(*textureTargetDef);
							serializedAppearanceIDsForTheme.insert(textureTargetDef->getAppearance()->getId());
						}
					}
				}
				m_writer.endElement(); // Appearance
			}
			m_writer.endElement(); // appearance
		}
	}
}
