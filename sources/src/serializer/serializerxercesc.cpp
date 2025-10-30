#include "citygml/citygml.h"
#include "citygml/citygmllogger.h"
#include "serializer/citygmlserializer.h"

#include <fstream>
#include <memory>

namespace citygml {

	class StdLogger : public CityGMLLogger {
	public:

		StdLogger(LOGLEVEL level = LOGLEVEL::LL_ERROR) :CityGMLLogger(level) {

		};

		virtual void log(LOGLEVEL level, const std::string& message, const char* file, int line) const
		{
			std::ostream& stream = level == LOGLEVEL::LL_ERROR ? std::cerr : std::cout;

			switch (level) {
			case LOGLEVEL::LL_DEBUG:
				stream << "DEBUG";
				break;
			case LOGLEVEL::LL_WARNING:
				stream << "WARNING";
				break;
			case LOGLEVEL::LL_TRACE:
				stream << "TRACE";
				break;
			case LOGLEVEL::LL_ERROR:
				stream << "ERROR";
				break;
			case LOGLEVEL::LL_INFO:
				stream << "INFO";
				break;
			}

			if (file) {
				stream << " [" << file;
				if (line > -1) {
					stream << ":" << line;
				}
				stream << "]";
			}

			stream << " " << message << std::endl;
		}
	};

    // 序列化器入口函数实现
    bool save(const CityModel& model, const std::string& filename, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger) {
        if (!logger) {
            // 创建默认日志记录器
            logger = std::make_shared<StdLogger>();
        }
        
        try {
            CityGMLSerializer serializer(params, logger);
            return serializer.serialize(model, filename);
        } catch (const std::exception& e) {
            CITYGML_LOG_ERROR(logger, "Exception in save(): " + std::string(e.what()));
            return false;
        }
    }
    
    bool save(const CityModel& model, std::ostream& stream, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger) {
        if (!logger) {
            // 创建默认日志记录器
            logger = std::make_shared<StdLogger>();
        }
        
        try {
            CityGMLSerializer serializer(params, logger);
            return serializer.serialize(model, stream);
        } catch (const std::exception& e) {
            CITYGML_LOG_ERROR(logger, "Exception in save(): " + std::string(e.what()));
            return false;
        }
    }

}
