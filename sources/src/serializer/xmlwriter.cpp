#include "serializer/xmlwriter.h"
#include <citygml/citygmllogger.h>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/TransService.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace citygml {

    XMLWriter::XMLWriter(std::shared_ptr<CityGMLLogger> logger)
        : m_document(nullptr)
        , m_currentElement(nullptr)
        , m_prettyPrint(true)
        , m_indentSize(2)
        , m_logger(logger)
        , m_hasError(false)
    {
        try {
            // 初始化Xerces-C平台
            if (!xercesc::XMLPlatformUtils::fgMemoryManager) {
                xercesc::XMLPlatformUtils::Initialize();
            }
        } catch (const xercesc::XMLException& e) {
            m_lastError = "Failed to initialize Xerces-C: " + toStdString(e.getMessage());
            m_hasError = true;
            CITYGML_LOG_ERROR(m_logger, m_lastError);
        }
    }

    XMLWriter::~XMLWriter() {
        // Xerces-C会自动管理DOM对象的生命周期
    }

    std::string XMLWriter::toStdString(const XMLCh* const wstr) {
        if (wstr == nullptr) {
            return "";
        }
        char* tmp = xercesc::XMLString::transcode(wstr);
        std::string str(tmp);
        xercesc::XMLString::release(&tmp);
        return str;
    }

    std::shared_ptr<XMLCh> XMLWriter::toXercesString(const std::string& str) {
        XMLCh* conv = xercesc::XMLString::transcode(str.c_str());
        return std::shared_ptr<XMLCh>(conv, [=](XMLCh* str) {
            xercesc::XMLString::release(&str);
        });
    }

    std::string XMLWriter::escapeXmlText(const std::string& text) {
        std::string result = text;
        // 替换XML特殊字符
        size_t pos = 0;
        while ((pos = result.find("&", pos)) != std::string::npos) {
            if (result.substr(pos, 5) != "&amp;" && 
                result.substr(pos, 4) != "&lt;" && 
                result.substr(pos, 4) != "&gt;" && 
                result.substr(pos, 6) != "&quot;" && 
                result.substr(pos, 6) != "&apos;") {
                result.replace(pos, 1, "&amp;");
                pos += 5;
            } else {
                pos += 1;
            }
        }
        
        pos = 0;
        while ((pos = result.find("<", pos)) != std::string::npos) {
            result.replace(pos, 1, "&lt;");
            pos += 4;
        }
        
        pos = 0;
        while ((pos = result.find(">", pos)) != std::string::npos) {
            result.replace(pos, 1, "&gt;");
            pos += 4;
        }
        
        return result;
    }

    std::string XMLWriter::escapeXmlAttribute(const std::string& text) {
        std::string result = escapeXmlText(text);
        
        size_t pos = 0;
        while ((pos = result.find("\"", pos)) != std::string::npos) {
            result.replace(pos, 1, "&quot;");
            pos += 6;
        }
        
        pos = 0;
        while ((pos = result.find("'", pos)) != std::string::npos) {
            result.replace(pos, 1, "&apos;");
            pos += 6;
        }
        
        return result;
    }

    bool XMLWriter::createDocument(const std::string& rootElementName, const std::string& rootNamespace) {
        try {
            // 获取支持LS的实现
            std::shared_ptr<XMLCh> features = toXercesString("LS");
            xercesc::DOMImplementation* impl = xercesc::DOMImplementationRegistry::getDOMImplementation(features.get());
            if (!impl) {
                throw std::runtime_error("Xerces-C DOMImplementation with LS not available");
            }
            std::shared_ptr<XMLCh> rootName = toXercesString(rootElementName);
            
            m_document = impl->createDocument(
                rootNamespace.empty() ? nullptr : toXercesString(rootNamespace).get(),
                rootName.get(),
                nullptr
            );
            
            m_currentElement = m_document->getDocumentElement();
            m_elementStack.push_back(m_currentElement);
            
            // 添加命名空间声明
            for (const auto& ns : m_namespaces) {
                std::string attrName = ns.first.empty() ? "xmlns" : "xmlns:" + ns.first;
                m_currentElement->setAttribute(
                    toXercesString(attrName).get(),
                    toXercesString(ns.second).get()
                );
            }
            
            //if (!m_defaultNamespace.empty()) {
            //    m_currentElement->setAttribute(
            //        toXercesString("xmlns").get(),
            //        toXercesString(m_defaultNamespace).get()
            //    );
            //}

			if (!m_schemaLocation.empty()) {
				m_currentElement->setAttribute(
					toXercesString("xsi:schemaLocation").get(),
					toXercesString(m_schemaLocation).get()
				);
			}
            
            return true;
        } catch (const xercesc::XMLException& e) {
            m_lastError = "Failed to create document: " + toStdString(e.getMessage());
            m_hasError = true;
            CITYGML_LOG_ERROR(m_logger, m_lastError);
            return false;
        } catch (const xercesc::DOMException& e) {
            m_lastError = "DOM error creating document: " + std::to_string(e.code);
            m_hasError = true;
            CITYGML_LOG_ERROR(m_logger, m_lastError);
            return false;
        }
    }

    void XMLWriter::startElement(const std::string& elementName, const std::string& namespaceURI) {
        if (!m_document || m_hasError) return;
        
        try {
			std::shared_ptr<XMLCh> name;
			std::shared_ptr<XMLCh> ns;
			if (m_namespaces.count(namespaceURI) > 0) {
				name = toXercesString(namespaceURI + ":" + elementName);
				ns = toXercesString(m_namespaces[namespaceURI]);
			} else {
				name = toXercesString(elementName);
				ns = namespaceURI.empty() ? nullptr : toXercesString(namespaceURI);
			}
            
			xercesc::DOMElement* element = nullptr;
			if (ns == nullptr) {
				element = m_document->createElement(name.get());
			}
			else {
				element = m_document->createElementNS(ns.get(), name.get());
			}
            m_currentElement->appendChild(element);
            
            m_currentElement = element;
            m_elementStack.push_back(element);
        } catch (const xercesc::DOMException& e) {
            m_lastError = "DOM error creating element: " + std::to_string(e.code);
            m_hasError = true;
            CITYGML_LOG_ERROR(m_logger, m_lastError);
        }
    }

    void XMLWriter::endElement() {
        if (m_elementStack.empty() || m_hasError) return;
        
        m_elementStack.pop_back();
        if (!m_elementStack.empty()) {
            m_currentElement = m_elementStack.back();
        }
    }

    void XMLWriter::writeAttribute(const std::string& name, const std::string& value, const std::string& namespaceURI) {
        if (!m_currentElement || m_hasError) return;
        
        try {
            std::shared_ptr<XMLCh> attrName = toXercesString(name);
            std::shared_ptr<XMLCh> attrValue = toXercesString(escapeXmlAttribute(value));
            std::shared_ptr<XMLCh> ns = namespaceURI.empty() ? nullptr : toXercesString(namespaceURI);
			
			if (ns == nullptr) {
				m_currentElement->setAttribute(attrName.get(), attrValue.get());
			} else {
				m_currentElement->setAttributeNS(ns.get(), attrName.get(), attrValue.get());
			}
        } catch (const xercesc::DOMException& e) {
            m_lastError = "DOM error setting attribute: " + std::to_string(e.code);
            m_hasError = true;
            CITYGML_LOG_ERROR(m_logger, m_lastError);
        }
    }

    void XMLWriter::writeAttribute(const std::string& name, int value, const std::string& namespaceURI) {
        writeAttribute(name, std::to_string(value), namespaceURI);
    }

    void XMLWriter::writeAttribute(const std::string& name, double value, const std::string& namespaceURI) {
        writeAttribute(name, std::to_string(value), namespaceURI);
    }

    void XMLWriter::writeAttribute(const std::string& name, bool value, const std::string& namespaceURI) {
        writeAttribute(name, value ? "true" : "false", namespaceURI);
    }

    void XMLWriter::writeTextContent(const std::string& text) {
        if (!m_currentElement || m_hasError) return;
        
        try {
            std::shared_ptr<XMLCh> content = toXercesString(escapeXmlText(text));
            xercesc::DOMText* textNode = m_document->createTextNode(content.get());
            m_currentElement->appendChild(textNode);
        } catch (const xercesc::DOMException& e) {
            m_lastError = "DOM error creating text content: " + std::to_string(e.code);
            m_hasError = true;
            CITYGML_LOG_ERROR(m_logger, m_lastError);
        }
    }

    void XMLWriter::writeTextContent(int value) {
        writeTextContent(std::to_string(value));
    }

    void XMLWriter::writeTextContent(double value) {
        writeTextContent(std::to_string(value));
    }

    void XMLWriter::addNamespaceDeclaration(const std::string& prefix, const std::string& uri) {
        m_namespaces[prefix] = uri;
    }

    void XMLWriter::setDefaultNamespace(const std::string& uri) {
        m_defaultNamespace = uri;
    }

	void XMLWriter::setSchemaLocation(const std::string& schemaLocation) {
		m_schemaLocation = schemaLocation;
	}

    void XMLWriter::setPrettyPrint(bool enabled) {
        m_prettyPrint = enabled;
    }

    void XMLWriter::setIndentSize(int size) {
        m_indentSize = size;
    }

    bool XMLWriter::hasError() const {
        return m_hasError;
    }

    std::string XMLWriter::getLastError() const {
        return m_lastError;
    }

    bool XMLWriter::saveToFile(const std::string& filename) {
        if (!m_document || m_hasError) return false;
        
        try {
            // 获取支持LS的实现
            std::shared_ptr<XMLCh> features = toXercesString("LS");
            xercesc::DOMImplementation* impl = xercesc::DOMImplementationRegistry::getDOMImplementation(features.get());
            xercesc::DOMImplementationLS* implLS = dynamic_cast<xercesc::DOMImplementationLS*>(impl);
            if (!implLS) {
                throw std::runtime_error("Xerces-C DOMImplementationLS not available");
            }

            std::unique_ptr<xercesc::DOMLSSerializer, void(*)(xercesc::DOMLSSerializer*)> serializer(
                implLS->createLSSerializer(), [](xercesc::DOMLSSerializer* p){ if(p) p->release(); }
            );

            if (m_prettyPrint) {
                serializer->getDomConfig()->setParameter(xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true);
            }

            std::unique_ptr<xercesc::LocalFileFormatTarget> target(new xercesc::LocalFileFormatTarget(toXercesString(filename).get()));
            std::unique_ptr<xercesc::DOMLSOutput, void(*)(xercesc::DOMLSOutput*)> output(
                implLS->createLSOutput(), [](xercesc::DOMLSOutput* p){ if(p) p->release(); }
            );
            output->setByteStream(target.get());
            serializer->write(m_document, output.get());
            return true;
        } catch (const xercesc::XMLException& e) {
            m_lastError = "Failed to save file: " + toStdString(e.getMessage());
            m_hasError = true;
            CITYGML_LOG_ERROR(m_logger, m_lastError);
            return false;
        } catch (const xercesc::DOMException& e) {
            m_lastError = "DOM error saving file: " + std::to_string(e.code);
            m_hasError = true;
            CITYGML_LOG_ERROR(m_logger, m_lastError);
            return false;
        }
    }

    bool XMLWriter::saveToStream(std::ostream& stream) {
        if (!m_document || m_hasError) return false;
        
        try {
            // 获取支持LS的实现
            std::shared_ptr<XMLCh> features = toXercesString("LS");
            xercesc::DOMImplementation* impl = xercesc::DOMImplementationRegistry::getDOMImplementation(features.get());
            xercesc::DOMImplementationLS* implLS = dynamic_cast<xercesc::DOMImplementationLS*>(impl);
            if (!implLS) {
                throw std::runtime_error("Xerces-C DOMImplementationLS not available");
            }

            std::unique_ptr<xercesc::DOMLSSerializer, void(*)(xercesc::DOMLSSerializer*)> serializer(
                implLS->createLSSerializer(), [](xercesc::DOMLSSerializer* p){ if(p) p->release(); }
            );
            if (m_prettyPrint) {
                serializer->getDomConfig()->setParameter(xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true);
            }

            // 使用内存缓冲区
            xercesc::MemBufFormatTarget target;
            std::unique_ptr<xercesc::DOMLSOutput, void(*)(xercesc::DOMLSOutput*)> output(
                implLS->createLSOutput(), [](xercesc::DOMLSOutput* p){ if(p) p->release(); }
            );
            output->setByteStream(&target);
            serializer->write(m_document, output.get());

            // 将缓冲区内容写入流
            stream.write(reinterpret_cast<const char*>(target.getRawBuffer()), target.getLen());
            return true;
        } catch (const xercesc::XMLException& e) {
            m_lastError = "Failed to save to stream: " + toStdString(e.getMessage());
            m_hasError = true;
            CITYGML_LOG_ERROR(m_logger, m_lastError);
            return false;
        } catch (const xercesc::DOMException& e) {
            m_lastError = "DOM error saving to stream: " + std::to_string(e.code);
            m_hasError = true;
            CITYGML_LOG_ERROR(m_logger, m_lastError);
            return false;
        }
    }

}
