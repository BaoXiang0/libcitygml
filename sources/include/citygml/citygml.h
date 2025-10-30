/* -*-c++-*- libcitygml - Copyright (c) 2010 Joachim Pouderoux, BRGM
*
* This file is part of libcitygml library
* http://code.google.com/p/libcitygml
*
* libcitygml is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 2.1 of the License, or
* (at your option) any later version.
*
* libcitygml is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*/

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <memory>
#include <functional>
#include <unordered_set>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#include <citygml/citygml_api.h>
#include <citygml/vecs.hpp>
#include <citygml/cityobject.h>
#include <citygml/envelope.h>
#include <citygml/tesselatorbase.h>

namespace citygml
{
    class CityModel;
    class CityGMLLogger;
    class Appearance;
    class Texture;
    class Material;
    class AppearanceManager;

    typedef EnumClassBitmask<CityObject::CityObjectsType> CityObjectsTypeMask;


    ///////////////////////////////////////////////////////////////////////////////
    // Parsing routines

    // Parameters:
    // objectsMask: a bit mask that defines which CityObjectsTypes are parsed
    //    examples: CityObject::CityObjectsType::COT_Building | CityObject::CityObjectsType::COT_Room <- parses only Building and Room objects"
    // minLOD: the minimal LOD that will be parsed
    // maxLOD: the maximal LOD that will be parsed
    // optimize: merge geometries & polygons that share the same appearance in the same object in order to reduce the global hierarchy
    // pruneEmptyObjects: remove the objects which do not contains any geometrical entity
    // tesselate: convert the interior & exteriors polygons to triangles
    // destSRS: the SRS (WKT, EPSG, OGC URN, etc.) where the coordinates must be transformed, default ("") is no transformation
    // srsSRS: the SRS (WKT, EPSG, OGC URN, etc.) to overrride the SRS in the CityGML data (if any), default ("") means no override or use included SRS

    class LIBCITYGML_EXPORT ParserParams
    {
    public:
        ParserParams()
            : objectsMask(CityObject::CityObjectsType::COT_All)
            , minLOD( 0 )
            , maxLOD( 4 )
            , optimize( false )
            , pruneEmptyObjects( false )
            , tesselate( true )
            , keepVertices ( false )
            , destSRS( "" )
            , srcSRS( "" )
        { }

    public:
        CityObjectsTypeMask objectsMask;
        unsigned int minLOD;
        unsigned int maxLOD;
        bool optimize;
        bool pruneEmptyObjects;
        bool tesselate;
        bool keepVertices;
        std::string destSRS;
        std::string srcSRS;
    };

    LIBCITYGML_EXPORT std::shared_ptr<const CityModel> load( std::istream& stream, const ParserParams& params, std::unique_ptr<TesselatorBase> tesselator, std::shared_ptr<CityGMLLogger> logger = nullptr);

    LIBCITYGML_EXPORT std::shared_ptr<const CityModel> load( const std::string& fileName, const ParserParams& params, std::unique_ptr<TesselatorBase> tesselator, std::shared_ptr<CityGMLLogger> logger = nullptr);

    ///////////////////////////////////////////////////////////////////////////////
    // Serialization routines

    /**
     * @brief SerializerParams类定义序列化器的配置参数
     */
	class LIBCITYGML_EXPORT SerializerParams {
	public:
		SerializerParams() {}

		// 格式化选项
		bool prettyPrint = true;           // 是否格式化输出（缩进、换行）
		int indentSize = 2;               // 缩进大小（空格数）

										  // 内容选项
		bool includeAppearances = true;   // 是否包含外观信息（材质、纹理）
		bool includeAddresses = true;     // 是否包含地址信息
		bool includeExternalReferences = true; // 是否包含外部引用

											   // 编码选项
		std::string encoding = "UTF-8";   // XML编码格式

										  // 几何选项
		bool includeEmptyGeometries = false; // 是否包含空的几何体
		bool includeLODInfo = true;       // 是否包含LOD信息

										  // 属性选项
		bool includeGenericAttributes = true; // 是否包含通用属性

											  // 验证选项
		bool validateOutput = false;      // 是否验证输出XML

										  // 性能选项
		bool optimizeForSize = false;     // 是否优化文件大小（移除不必要的空白）
	};

    /**
     * @brief 将CityModel序列化到文件
     * @param model 要序列化的CityModel
     * @param filename 输出文件名
     * @param params 序列化参数
     * @param logger 日志记录器
     * @return 成功返回true，失败返回false
     */
    LIBCITYGML_EXPORT bool save(
        const CityModel& model, 
        const std::string& filename,
        const SerializerParams& params = SerializerParams(),
        std::shared_ptr<CityGMLLogger> logger = nullptr
    );
    
    /**
     * @brief 将CityModel序列化到输出流
     * @param model 要序列化的CityModel
     * @param stream 输出流
     * @param params 序列化参数
     * @param logger 日志记录器
     * @return 成功返回true，失败返回false
     */
    LIBCITYGML_EXPORT bool save(
        const CityModel& model,
        std::ostream& stream,
        const SerializerParams& params = SerializerParams(),
        std::shared_ptr<CityGMLLogger> logger = nullptr
    );

}
