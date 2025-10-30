#pragma once

#include "serializer/elementserializer.h"
#include <citygml/cityobject.h>

namespace citygml {

    /**
     * @brief CityObjectSerializer负责序列化CityObject对象
     */
    class LIBCITYGML_EXPORT CityObjectSerializer : public ElementSerializer {
    public:
        CityObjectSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger);
        
        void serialize(const CityObject& cityObject);
        
		static std::string getNamespaceForCityObjectType(CityObject::CityObjectsType type);

    private:
		void serialize(const CityObject& cityObject, bool isRoot);
        void serializeGeometries(const CityObject& cityObject);
        void serializeImplicitGeometries(const CityObject& cityObject);
        void serializeChildCityObjects(const CityObject& cityObject);
        void serializeAddress(const CityObject& cityObject);
		void serializeAppearances(const CityObject& cityObject);
        void serializeExternalReference(const CityObject& cityObject);
        void serializeRectifiedGridCoverage(const CityObject& cityObject);

		/**
		* @brief 获取LOD容器的元素名
		* @param type CityObject类型
		* @param lod LOD级别
		* @return 容器元素名
		*/
		std::string getLODContainerName(CityObject::CityObjectsType type, unsigned int lod) const;
    };

}
