#include "serializer/addressserializer.h"
#include <citygml/address.h>

namespace citygml {

    AddressSerializer::AddressSerializer(XMLWriter& writer, const SerializerParams& params, std::shared_ptr<CityGMLLogger> logger)
        : ElementSerializer(writer, params, logger)
    {
    }

    void AddressSerializer::serialize(const Address& address) {
        // 开始Address元素
        m_writer.startElement("Address", CITYGML_NS_URI);
        
        // 写入ID属性
        //if (!address.getId().empty()) {
        //    m_writer.writeAttribute("gml:id", address.getId());
        //}
        
        // 序列化地址详细信息
        serializeAddressDetails(address);
        
        // 结束Address元素
        m_writer.endElement();
    }

    void AddressSerializer::serializeAddressDetails(const Address& address) {
		// 开始xalAddress元素
		m_writer.startElement("xalAddress");

		// 开始xAL:AddressDetails元素
		m_writer.startElement("AddressDetails", XAL_NS);

		// 序列化国家
		if (!address.country().empty()) {
			m_writer.startElement("Country", XAL_NS);
			m_writer.startElement("CountryName", XAL_NS);
			m_writer.writeTextContent(address.country());
			m_writer.endElement();

			// 序列化城市信息
			if (!address.locality().empty()) {
				m_writer.startElement("Locality", XAL_NS);
				m_writer.writeAttribute("Type", std::string("City"));

				m_writer.startElement("LocalityName", XAL_NS);
				m_writer.writeTextContent(address.locality());
				m_writer.endElement();

				// 序列化街道信息
				if (!address.thoroughfareName().empty() || !address.thoroughfareNumber().empty()) {
					m_writer.startElement("Thoroughfare", XAL_NS);
					m_writer.writeAttribute("Type", std::string("Street"));

					if (!address.thoroughfareNumber().empty()) {
						m_writer.startElement("ThoroughfareNumber", XAL_NS);
						m_writer.writeTextContent(address.thoroughfareNumber());
						m_writer.endElement();
					}

					if (!address.thoroughfareName().empty()) {
						m_writer.startElement("ThoroughfareName", XAL_NS);
						m_writer.writeTextContent(address.thoroughfareName());
						m_writer.endElement();
					}

					m_writer.endElement(); // Thoroughfare
				}

				// 序列化邮政编码
				if (!address.postalCode().empty()) {
					m_writer.startElement("PostalCode", XAL_NS);
					m_writer.startElement("PostalCodeNumber", XAL_NS);
					m_writer.writeTextContent(address.postalCode());
					m_writer.endElement();
					m_writer.endElement(); // PostalCode
				}

				m_writer.endElement(); // Locality
			}

			m_writer.endElement(); // Country
		}

		m_writer.endElement(); // AddressDetails
		m_writer.endElement(); // xalAddress
    }

    void AddressSerializer::serializeMultiPoint(const Address& address) {
        // 如果地址有几何信息，可以在这里序列化
        // 目前Address类没有几何信息，所以这个方法暂时为空
    }

}
