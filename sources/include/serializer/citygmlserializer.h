#pragma once
#include <set>
#include "serializer/cityobjectserializer.h"
#include <citygml/citymodel.h>
#include <citygml/citygmllogger.h>

namespace citygml {

    /**
     * @brief CityGMLSerializer是主要的序列化器类，负责序列化整个CityModel
     */
    class LIBCITYGML_EXPORT CityGMLSerializer {
    public:
        CityGMLSerializer(const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger);
        ~CityGMLSerializer();
        
        // 主要序列化方法
        bool serialize(const CityModel& model, const std::string& filename);
        bool serialize(const CityModel& model, std::ostream& stream);
        
    private:
		void setupNamespaces();
		void setupSchemaLocation();
        void serializeCityModel(const CityModel& model);
        void serializeEnvelope(const CityModel& model);
        void serializeRootCityObjects(const CityModel& model);

		/**
		* @brief 收集使用的数据类型
		* @param model CityModel对象
		*/
		void collectUsedTypes(const CityModel& model);

		/**
		* @brief 收集对象的类型信息
		* @param cityObject CityObject对象
		*/
		void collectObjectTypes(const CityObject& cityObject);

		/**
		* @brief 收集外观类型信息
		*/
		void collectAppearanceTypes();
        
        // 序列化器实例
        XMLWriter m_writer;
        const SerializerParams& m_params;
        std::shared_ptr<CityGMLLogger> m_logger;
        std::unique_ptr<CityObjectSerializer> m_cityObjectSerializer;

		// 用于跟踪使用的数据类型
		std::set<std::string> m_usedNamespaces;
		std::set<std::string> m_usedSchemaLocations;
    };

}
