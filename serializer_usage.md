# CityGML序列化器

本文档介绍了libcitygml库中新实现的CityGML序列化器功能。

## 概述

CityGML序列化器提供了将内存中的CityModel对象序列化为符合CityGML 2.0标准的XML文件的功能。序列化器使用独立的类层次结构，与现有的parser目录对应，确保代码的清晰分离和易于维护。

## 主要特性

- **完整的CityGML 2.0支持**：输出符合CityGML 2.0标准的XML格式
- **命名空间支持**：正确处理所有CityGML命名空间（gml, bldg, core, frn, trans等）
- **完整数据序列化**：支持CityModel、CityObject、Geometry、Appearance、Material、Texture等所有数据类型
- **可配置选项**：支持多种序列化参数配置
- **错误处理**：完善的错误处理和日志记录
- **格式化输出**：支持可读的XML格式化输出

## API使用

### 基本用法

```cpp
#include <citygml/citygml.h>
#include <citygml/citygmllogger.h>

// 创建日志记录器
auto logger = std::make_shared<citygml::StdLogger>();

// 序列化参数
citygml::SerializerParams params;
params.prettyPrint = true;
params.includeAppearances = true;
params.includeAddresses = true;

// 序列化到文件
bool success = citygml::save(*model, "output.gml", params, logger);

// 序列化到流
std::ofstream stream("output.gml");
bool success = citygml::save(*model, stream, params, logger);
```

### 序列化参数

`SerializerParams`类提供了丰富的配置选项：

```cpp
citygml::SerializerParams params;

// 格式化选项
params.prettyPrint = true;           // 是否格式化输出（缩进、换行）
params.indentSize = 2;               // 缩进大小（空格数）

// 内容选项
params.includeAppearances = true;    // 是否包含外观信息（材质、纹理）
params.includeAddresses = true;      // 是否包含地址信息
params.includeExternalReferences = true; // 是否包含外部引用

// 编码选项
params.encoding = "UTF-8";          // XML编码格式

// 几何选项
params.includeEmptyGeometries = false; // 是否包含空的几何体
params.includeLODInfo = true;       // 是否包含LOD信息

// 属性选项
params.includeGenericAttributes = true; // 是否包含通用属性

// 验证选项
params.validateOutput = false;       // 是否验证输出XML

// 性能选项
params.optimizeForSize = false;      // 是否优化文件大小
```

## 架构设计

### 目录结构

```
include/serializer/
├── xmlwriter.h                    # XML写入辅助类
├── serializerparams.h             # 序列化参数类
├── elementserializer.h            # 序列化器基类
├── linearringserializer.h         # LinearRing序列化器
├── polygonserializer.h            # Polygon序列化器
├── linestringserializer.h         # LineString序列化器
├── geometryserializer.h           # Geometry序列化器
├── materialserializer.h           # Material序列化器
├── textureserializer.h            # Texture序列化器
├── appearanceserializer.h         # Appearance序列化器
├── addressserializer.h            # Address序列化器
├── cityobjectserializer.h         # CityObject序列化器
└── citygmlserializer.h            # 主序列化器类

src/serializer/
├── xmlwriter.cpp
├── serializerparams.cpp
├── elementserializer.cpp
├── linearringserializer.cpp
├── polygonserializer.cpp
├── linestringserializer.cpp
├── geometryserializer.cpp
├── materialserializer.cpp
├── textureserializer.cpp
├── appearanceserializer.cpp
├── addressserializer.cpp
├── cityobjectserializer.cpp
├── citygmlserializer.cpp
└── serializerxercesc.cpp          # 入口函数实现
```

### 核心类

1. **XMLWriter**：封装Xerces-C的DOM API，提供简洁的XML写入接口
2. **ElementSerializer**：所有序列化器的基类，提供通用功能
3. **CityGMLSerializer**：主序列化器类，负责整个CityModel的序列化
4. **各种专用序列化器**：负责特定数据类型的序列化

## 序列化流程

1. **初始化Xerces-C**：在CityGMLSerializer构造时初始化XML平台
2. **创建DOM文档**：设置根元素和命名空间
3. **递归序列化CityModel**：
   - 写入CityModel基本信息（id, envelope, srsName）
   - 写入CityModel属性和主题
   - 遍历并序列化所有root CityObjects
4. **序列化CityObject**：
   - 根据类型写入正确的XML元素名和命名空间
   - 写入属性（genericAttributes）
   - 序列化geometries、implicitGeometries
   - 递归序列化子对象
   - 序列化address、externalReference等
5. **序列化Geometry**：
   - 写入LOD信息
   - 序列化polygons和linestrings
   - 处理子geometries
6. **序列化Appearance**：
   - 序列化Materials和Textures
   - 写入theme信息
   - 写入target links

## 命名空间处理

序列化器正确输出CityGML标准的命名空间：

```xml
<CityModel xmlns="http://www.opengis.net/citygml/2.0"
           xmlns:gml="http://www.opengis.net/gml"
           xmlns:bldg="http://www.opengis.net/citygml/building/2.0"
           xmlns:core="http://www.opengis.net/citygml/2.0"
           xmlns:frn="http://www.opengis.net/citygml/cityfurniture/2.0"
           xmlns:trans="http://www.opengis.net/citygml/transportation/2.0"
           xmlns:veg="http://www.opengis.net/citygml/vegetation/2.0"
           xmlns:wtr="http://www.opengis.net/citygml/waterbody/2.0"
           xmlns:luse="http://www.opengis.net/citygml/landuse/2.0"
           xmlns:sub="http://www.opengis.net/citygml/tunnel/2.0"
           xmlns:brid="http://www.opengis.net/citygml/bridge/2.0"
           xmlns:grp="http://www.opengis.net/citygml/cityobjectgroup/2.0"
           xmlns:dem="http://www.opengis.net/citygml/relief/2.0"
           xmlns:gen="http://www.opengis.net/citygml/generics/2.0"
           xmlns:app="http://www.opengis.net/citygml/appearance/2.0"
           xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
```

## 错误处理

序列化器提供完善的错误处理机制：

- 使用CityGMLLogger进行日志记录
- 异常安全的设计
- 详细的错误信息
- 优雅的错误恢复

## 性能考虑

- 使用Xerces-C DOM API进行高效的XML构建
- 支持格式化输出的开关
- 可配置的文件大小优化选项
- 内存使用优化

## 测试建议

建议的测试流程：

1. 加载一个CityGML文件
2. 使用序列化器输出到新文件
3. 重新加载新文件
4. 对比数据完整性

## 注意事项

- 序列化器需要Xerces-C库支持
- 确保输入数据已经完成解析和处理
- 大型CityModel可能需要较多内存
- 建议在生产环境中启用日志记录

## 示例代码

完整的使用示例请参考 `examples/serializer_example.cpp` 文件。
