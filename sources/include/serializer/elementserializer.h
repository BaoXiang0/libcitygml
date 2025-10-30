#pragma once

#include <memory>
#include <citygml/citygml_api.h>
#include <citygml/citygml.h>
#include <citygml/citygmllogger.h>
#include <citygml/cityobject.h>
#include <citygml/geometry.h>
#include "serializer/xmlwriter.h"
#include "serializer/serializerconstants.h"

namespace citygml {

    /**
     * @brief ElementSerializer是所有序列化器的基类
     * 提供通用的序列化功能和接口
     */
    class LIBCITYGML_EXPORT ElementSerializer {
    public:
        ElementSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger);
        virtual ~ElementSerializer() = default;

        // 通用序列化方法
        void serializeAttributes(const AttributesMap& attributes);
        
        // 辅助方法
        //std::string getElementNameForGeometryType(Geometry::GeometryType type) const;
        
        // 错误处理
        bool hasError() const;
        std::string getLastError() const;

		// 通用序列化方法
		static void serializeAttributes(XMLWriter& writer, bool includeGenericAttributes, const AttributesMap& attributes);
		// 序列化单个通用属性
		static void serializeGenericAttribute(XMLWriter& writer, const std::string& name, const AttributeValue& value);

    protected:
        XMLWriter& m_writer;
        const SerializerParams& m_params;
        std::shared_ptr<CityGMLLogger> m_logger;
        
        // 错误处理
        void setError(const std::string& error);
        
    private:
        std::string m_lastError;
        bool m_hasError;
    };

}
