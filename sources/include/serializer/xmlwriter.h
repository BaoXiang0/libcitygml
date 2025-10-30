#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

#include <citygml/citygml_api.h>
#include <citygml/citygmllogger.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/XMLString.hpp>

namespace citygml {

    /**
     * @brief XMLWriter类封装Xerces-C的DOM API，提供简洁的XML写入接口
     */
    class LIBCITYGML_EXPORT XMLWriter {
    public:
        XMLWriter(std::shared_ptr<CityGMLLogger> logger);
        ~XMLWriter();

        // 文档操作
        bool createDocument(const std::string& rootElementName, const std::string& rootNamespace = "");
        bool saveToFile(const std::string& filename);
        bool saveToStream(std::ostream& stream);

        // 元素操作
        void startElement(const std::string& elementName, const std::string& namespaceURI = "");
        void endElement();
        
        // 属性操作
        void writeAttribute(const std::string& name, const std::string& value, const std::string& namespaceURI = "");
        void writeAttribute(const std::string& name, int value, const std::string& namespaceURI = "");
        void writeAttribute(const std::string& name, double value, const std::string& namespaceURI = "");
        void writeAttribute(const std::string& name, bool value, const std::string& namespaceURI = "");

        // 文本内容
        void writeTextContent(const std::string& text);
        void writeTextContent(int value);
        void writeTextContent(double value);

        // 命名空间管理
        void addNamespaceDeclaration(const std::string& prefix, const std::string& uri);
        void setDefaultNamespace(const std::string& uri);
		void setSchemaLocation(const std::string& schemaLocation);

        // 格式化选项
        void setPrettyPrint(bool enabled);
        void setIndentSize(int size);

        // 错误检查
        bool hasError() const;
        std::string getLastError() const;

    private:
        std::string toStdString(const XMLCh* const wstr);
        std::shared_ptr<XMLCh> toXercesString(const std::string& str);
        std::string escapeXmlText(const std::string& text);
        std::string escapeXmlAttribute(const std::string& text);

        xercesc::DOMDocument* m_document;
        xercesc::DOMElement* m_currentElement;
        std::vector<xercesc::DOMElement*> m_elementStack;
        
        std::map<std::string, std::string> m_namespaces;
        std::string m_defaultNamespace;
		std::string m_schemaLocation;
        
        bool m_prettyPrint;
        int m_indentSize;
        
        std::shared_ptr<CityGMLLogger> m_logger;
        std::string m_lastError;
        bool m_hasError;
    };

}
