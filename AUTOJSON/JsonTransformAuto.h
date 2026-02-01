//
//  JsonTransformAuto.h
//  JsonTransformAuto
//
//  Created by Matthew on 2026/1/31.
//

#ifndef __AUTO__TRANSFORM_JSON__H__
#define __AUTO__TRANSFORM_JSON__H__
#include <iostream>
#include <unordered_map>
#include <vector>
#include <type_traits>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

namespace auto_json {
using JsonLocation = rapidjson::Value;
using OffsetType = size_t;
using pFuncParse = bool(*)(const char*key,const JsonLocation &json,void*pThis,OffsetType offset);
using pFuncParseToJson = void(*)(const char*key,rapidjson::Value &value,rapidjson::Document::AllocatorType &allocator,void*pThis,OffsetType offset);

struct ReflectInfo
{
    OffsetType offset;
    pFuncParse from_json;
    pFuncParseToJson to_json;
};

using ReflectMapType = std::unordered_map<const char*,ReflectInfo>;

template <typename  T,typename Enable = void>
struct transform;


template <typename  T>
struct transform<T,std::enable_if_t<std::is_same_v<decltype(T::reflect_map),const ReflectMapType>>>
{
    using Type = T;
    static  bool from_json(const char*key,const JsonLocation &json,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///array
            auto&& obj = json[static_cast<int>(offset)];
            if (obj.IsObject()) {
                auto && dest = *reinterpret_cast<Type*>(pThis);
                return  dest.transform_from_json(obj);
            }
            return false;
        }
        if (json.HasMember(key)) {
            auto&& obj = json[key];
            if (obj.IsObject()) {
                auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) + offset);
                return  dest.transform_from_json(obj);
            }
        }
        return false;
    }
    
    static  void to_json(const char*key,rapidjson::Value &value,rapidjson::Document::AllocatorType &allocator,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///array
            auto && dest = *reinterpret_cast<Type*>(pThis);
            dest.transform_to_json(value,allocator);
            return;
        }
        rapidjson::Value k;
        k.SetString(key, allocator);
        auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) +  offset);
        rapidjson::Value obj(rapidjson::kObjectType);
        dest.transform_to_json(obj,allocator);
        value.AddMember(k, obj, allocator);
    }
};


template <typename  T>
struct transform<std::vector<T>>
{
    using Type = std::vector<T>;
    static  bool from_json(const char*key,const JsonLocation &json,void*pThis,OffsetType offset)
    {
        if (json.HasMember(key)) {
            auto&& datas = json[key];
            if (datas.IsArray() && datas.Size() > 0) {
                auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) + offset);
                dest.resize(datas.Size());
                for (int i=0; i<datas.Size(); i++) {
                   transform<T>::from_json(nullptr,datas, &dest[i],i);
                }
                return  true;
            }
        }
        return false;
    }
    static  void to_json(const char*key,rapidjson::Value &value,rapidjson::Document::AllocatorType &allocator,void*pThis,OffsetType offset)
    {
        auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) +  offset);
        rapidjson::Value elements(rapidjson::kArrayType);//创建一个Array类型的元素
        for(auto&&element :dest)
        {
            rapidjson::Value e;
            transform<T>::to_json(nullptr,e,allocator,&element,0);
            elements.PushBack(e, allocator);
        }
        rapidjson::Value k;
        k.SetString(key, allocator);
        value.AddMember(k, elements, allocator);//添加数组
    }
};

template <>
struct transform<bool>
{
    using Type = bool;
    static  bool from_json(const char*key,const JsonLocation &json,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///unsupport
            printf("std::vector<bool> is not supported \n");
            return  false;
        }
        if (json.HasMember(key)) {
            auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) + offset);
            dest =   json[key].GetBool();
            return  true;
        }
        return false;
    }
    
    static  void to_json(const char*key,rapidjson::Value &value,rapidjson::Document::AllocatorType &allocator,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///array
            printf("std::vector<bool> is not supported \n");
            return;
        }
        auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) +  offset);
        rapidjson::Value k;
        k.SetString(key, allocator);

        rapidjson::Value v;
        v.SetBool(dest);
        value.AddMember(k, v, allocator);
    }
};

template <>
struct transform<int>
{
    using Type = int;
    static  bool from_json(const char*key,const JsonLocation &json,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///array
            auto && dest = *reinterpret_cast<Type*>(pThis);
            dest = json[static_cast<int>(offset)].GetInt();
            return true;
        }
        auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) + offset);
        if (json.HasMember(key)) {
            dest =   json[key].GetInt();
            return  true;
        }
        return false;
    }
    
    static  void to_json(const char*key,rapidjson::Value &value,rapidjson::Document::AllocatorType &allocator,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///array
            auto && dest = *reinterpret_cast<Type*>(pThis);
            value.SetInt(dest);
            return;
        }

        auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) +  offset);
        rapidjson::Value k;
        k.SetString(key, allocator);

        rapidjson::Value v;
        v.SetInt(dest);
        value.AddMember(k, v, allocator);
    }
};


template <>
struct transform<double>
{
    using Type = double;
    static  bool from_json(const char*key,const JsonLocation &json,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///array
            auto && dest = *reinterpret_cast<Type*>(pThis);
            dest = json[static_cast<int>(offset)].GetDouble();
            return true;
        }
       
        if (json.HasMember(key)) {
            auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) + offset);
            dest =   json[key].GetDouble();
            return  true;
        }
        return false;
    }
    
    static  void to_json(const char*key,rapidjson::Value &value,rapidjson::Document::AllocatorType &allocator,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///array
            auto && dest = *reinterpret_cast<Type*>(pThis);
            value.SetDouble(dest);
            return;
        }
        auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) +  offset);
        rapidjson::Value k;
        k.SetString(key, allocator);

        rapidjson::Value v;
        v.SetDouble(dest);
        value.AddMember(k, v, allocator);
    }
};


template <>
struct transform<std::string>
{
    using Type = std::string;
    static  bool from_json(const char*key,const JsonLocation &json,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///array
            auto && dest = *reinterpret_cast<Type*>(pThis);
            dest = json[static_cast<int>(offset)].GetString();
            return true;
        }
      
        if (json.HasMember(key)) {
            auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) +  offset);
            dest =   json[key].GetString();
            return  true;
        }
        return false;
    }
    
    static  void to_json(const char*key,rapidjson::Value &value,rapidjson::Document::AllocatorType &allocator,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///array
            auto && dest = *reinterpret_cast<Type*>(pThis);
            value.SetString(dest.c_str(),allocator);
            return;
        }
        auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) +  offset);
        rapidjson::Value k;
        k.SetString(key, allocator);

        rapidjson::Value v;
        v.SetString(dest.c_str(), allocator);
        value.AddMember(k, v, allocator);
    }
};



bool transform_from_json(void *pThis,const ReflectMapType &reflect_map ,const JsonLocation& json)
{
    unsigned int total{};
    for (auto&& p:reflect_map)
    {
       auto success =  p.second.from_json(p.first,json,pThis,p.second.offset);
        if (!success)
        {
            total ++;
        }
    }
    return  total == 0;
}

bool transform_from_json(void *pThis,const ReflectMapType &reflect_map ,const std::string& json)
{
    rapidjson::Document doc;
    rapidjson::ParseResult ok = doc.Parse(json.c_str());
    if (!ok) 
    {
        printf("Parse Json Error: %s at %zu\n",rapidjson::GetParseError_En(ok.Code()),ok.Offset());
        return  false;
    }
    rapidjson::Value& root = doc;
    return transform_from_json(pThis, reflect_map, root);
}

void transform_to_json(void *pThis,rapidjson::Value &value,rapidjson::Document::AllocatorType &allocator,const ReflectMapType &reflect_map)
{
    for (auto&& p:reflect_map)
    {
        p.second.to_json(p.first,value,allocator,pThis,p.second.offset);
    }
}

std::string transform_to_json(void *pThis,const ReflectMapType &reflect_map)
{
    rapidjson::Document doc;
    auto &allocator = doc.GetAllocator();
    doc.SetObject();
    transform_to_json(pThis, doc,allocator, reflect_map);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return  buffer.GetString();
}

};


#define __ADD__JSON__KEY__1(C,_1)  {#_1,{offsetof(C, _1),(auto_json::pFuncParse)auto_json::transform<decltype(C::_1)>::from_json,(auto_json::pFuncParseToJson)auto_json::transform<decltype(C::_1)>::to_json}},
#define __ADD__JSON__KEY__2(C,_1,_2)      __ADD__JSON__KEY__1(C,_1) __ADD__JSON__KEY__1(C,_2)
#define __ADD__JSON__KEY__3(C,_1,_2,_3)   __ADD__JSON__KEY__2(C,_1,_2) __ADD__JSON__KEY__1(C,_3)
#define __ADD__JSON__KEY__4(C,_1,_2,_3,_4)   __ADD__JSON__KEY__3(C,_1,_2,_3) __ADD__JSON__KEY__1(C,_4)
#define __ADD__JSON__KEY__5(C,_1,_2,_3,_4,_5)   __ADD__JSON__KEY__4(C,_1,_2,_3,_4) __ADD__JSON__KEY__1(C,_5)
#define __ADD__JSON__KEY__6(C,_1,_2,_3,_4,_5,_6)   __ADD__JSON__KEY__5(C,_1,_2,_3,_4,_5) __ADD__JSON__KEY__1(C,_6)
#define __ADD__JSON__KEY__7(C,_1,_2,_3,_4,_5,_6,_7)   __ADD__JSON__KEY__6(C,_1,_2,_3,_4,_5,_6) __ADD__JSON__KEY__1(C,_7)
#define __ADD__JSON__KEY__8(C,_1,_2,_3,_4,_5,_6,_7,_8)   __ADD__JSON__KEY__7(C,_1,_2,_3,_4,_5,_6,_7) __ADD__JSON__KEY__1(C,_8)
#define __ADD__JSON__KEY__9(C,_1,_2,_3,_4,_5,_6,_7,_8,_9)   __ADD__JSON__KEY__8(C,_1,_2,_3,_4,_5,_6,_7,_8) __ADD__JSON__KEY__1(C,_9)
#define __ADD__JSON__KEY__10(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10)   __ADD__JSON__KEY__9(C,_1,_2,_3,_4,_5,_6,_7,_8,_9) __ADD__JSON__KEY__1(C,_10)
#define __ADD__JSON__KEY__11(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11)   __ADD__JSON__KEY__10(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10) __ADD__JSON__KEY__1(C,_11)
#define __ADD__JSON__KEY__12(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12)   __ADD__JSON__KEY__11(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11) __ADD__JSON__KEY__1(C,_12)
#define __ADD__JSON__KEY__13(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13)   __ADD__JSON__KEY__12(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12) __ADD__JSON__KEY__1(C,_13)
#define __ADD__JSON__KEY__14(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14)   __ADD__JSON__KEY__13(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13) __ADD__JSON__KEY__1(C,_14)
#define __ADD__JSON__KEY__15(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15)   __ADD__JSON__KEY__14(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14) __ADD__JSON__KEY__1(C,_15)
#define __ADD__JSON__KEY__16(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16)   __ADD__JSON__KEY__15(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15) __ADD__JSON__KEY__1(C,_16)
#define __ADD__JSON__KEY__17(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17)   __ADD__JSON__KEY__16(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16) __ADD__JSON__KEY__1(C,_17)
#define __ADD__JSON__KEY__18(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18)   __ADD__JSON__KEY__17(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17) __ADD__JSON__KEY__1(C,_18)
#define __ADD__JSON__KEY__19(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19)   __ADD__JSON__KEY__18(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18) __ADD__JSON__KEY__1(C,_19)
#define __ADD__JSON__KEY__20(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20)   __ADD__JSON__KEY__19(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19) __ADD__JSON__KEY__1(C,_20)



#define DECLARE__JSON__AUTO__TRANSFORM  \
const static auto_json::ReflectMapType reflect_map;  \
inline bool transform_from_json(const std::string &json){return auto_json::transform_from_json(this, reflect_map, json);}\
inline bool transform_from_json(const auto_json::JsonLocation &json){return auto_json::transform_from_json(this, reflect_map, json);}\
inline std::string transform_to_json(){return auto_json::transform_to_json(this, reflect_map);}\
inline void transform_to_json(rapidjson::Value &value,rapidjson::Document::AllocatorType &allocator){auto_json::transform_to_json(this, value,allocator,reflect_map);}


#ifdef _MSC_VER ///VS2015


#define __EXPAND__(X)  X
#define __MACRO__ARG__COUNT__(_0, _1, _2, _3, _4, _5, _6, _7, _8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19, COUNT, ...)  COUNT
#define _MACROARGCOUNT_(...) __EXPAND__(__MACRO__ARG__COUNT__(__VA_ARGS__,20,19,18,17,16,15,14,13,12,11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1))

#else //GCC  4.8.5


#define __MACRO__ARG__COUNT__(_0, _1, _2, _3, _4, _5, _6, _7, _8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19, COUNT, ...)  COUNT
#define _MACROARGCOUNT_(...) __MACRO__ARG__COUNT__(__VA_ARGS__,20,19,18,17,16,15,14,13,12,11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#endif

#define __CONCAT__(A,B) ____CONCAT____(A,B)
#define ____CONCAT____(A,B)   A##B

#define __CONCATFUNCTION__(...)  __CONCAT__(__ADD__JSON__KEY__ , _MACROARGCOUNT_(__VA_ARGS__))

#define IMPLEMENT__JSON__AUTO__TRANSFORM(C,...)   \
const  auto_json::ReflectMapType C::reflect_map = {\
    __CONCATFUNCTION__(__VA_ARGS__)(C,__VA_ARGS__)  \
};



#endif /* JsonTransformAuto.h */
