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

using ReflectMapType = std::unordered_map<const char*,std::pair<OffsetType,pFuncParse>>;

template <typename  T,typename Enable = void>
struct transform;


template <typename  T>
struct transform<T,std::enable_if_t<std::is_same_v<decltype(T::reflect_map),const ReflectMapType>>>
{
    static  bool from_json(const char*key,const JsonLocation &json,void*pThis,OffsetType offset)
    {
        if (json.HasMember(key)) {
            auto&& obj = json[key];
            if (obj.IsObject()) {
                auto && dest = *reinterpret_cast<T*>(static_cast<char*>(pThis) + offset);
                return  dest.transform_from_json(obj);
            }
        }
        return false;
    }
};
/*
template <typename  T>
struct transform<std::vector<T>,std::enable_if_t<std::is_same_v<decltype(T::reflect_map),const ReflectMapType>>>
{
    using Type = std::vector<T>;
    static  bool from_json(const char*key,const JsonLocation &json,void*pThis,OffsetType offset)
    {
        if (json.HasMember(key)) {
            auto&& datas = json[key];
            if (datas.IsArray() && datas.Size() > 0) {
                auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) + offset);
                dest.resize(datas.Size());
                for (int i=0; i<datas.Size(); i++) 
                {
                    auto && data = datas[i];
                    if (data.IsObject()) 
                    {
                        
                    }
                }
                return  true;
            }
        }
        return false;
    }
};*/

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
};

template <>
struct transform<bool>
{
    static  bool from_json(const char*key,const JsonLocation &json,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///unsupport
            return  false;
        }
        if (json.HasMember(key)) {
            auto && dest = *reinterpret_cast<bool*>(static_cast<char*>(pThis) + offset);
            dest =   json[key].GetBool();
            return  true;
        }
        return false;
    }
};

template <>
struct transform<int>
{
    static  bool from_json(const char*key,const JsonLocation &json,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///array
            auto && dest = *reinterpret_cast<int*>(pThis);
            dest = json[static_cast<int>(offset)].GetInt();
            return true;
        }
        auto && dest = *reinterpret_cast<int*>(static_cast<char*>(pThis) + offset);
        if (json.HasMember(key)) {
            dest =   json[key].GetInt();
            return  true;
        }
        return false;
    }
};


template <>
struct transform<double>
{
    static  bool from_json(const char*key,const JsonLocation &json,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///array
            auto && dest = *reinterpret_cast<double*>(pThis);
            dest = json[static_cast<int>(offset)].GetDouble();
            return true;
        }
       
        if (json.HasMember(key)) {
            auto && dest = *reinterpret_cast<double*>(static_cast<char*>(pThis) + offset);
            dest =   json[key].GetDouble();
            return  true;
        }
        return false;
    }
};


template <>
struct transform<std::string>
{
    static  bool from_json(const char*key,const JsonLocation &json,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///array
            auto && dest = *reinterpret_cast<std::string*>(pThis);
            dest = json[static_cast<int>(offset)].GetString();
            return true;
        }
      
        if (json.HasMember(key)) {
            auto && dest = *reinterpret_cast<std::string*>(static_cast<char*>(pThis) +  offset);
            dest =   json[key].GetString();
            return  true;
        }
        return false;
    }
};



bool transform_from_json(void *pThis,const ReflectMapType &reflect_map ,const JsonLocation& json)
{
    unsigned int total{};
    for (auto&& p:reflect_map)
    {
       auto success =  p.second.second(p.first,json,pThis,p.second.first);
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

};


#define __ADD__JSON__KEY__1(C,_1)  {#_1,{offsetof(C, _1),(auto_json::pFuncParse)auto_json::transform<decltype(C::_1)>::from_json}},
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
inline bool transform_from_json(const auto_json::JsonLocation &json){return auto_json::transform_from_json(this, reflect_map, json);}


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
