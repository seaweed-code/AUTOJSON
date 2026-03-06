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
#include <cstddef>
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

template <typename T>
struct _has_const_reflect_map_member_
{///either non-static member or static member, but must const
private:
    template <typename U>
    static char test(const ReflectMapType* const*p, typename std::enable_if<std::is_same<const ReflectMapType, decltype(U::reflect_map)>::value> ::type*);

    template <typename U>
    static long test(...);

public:
    static const bool value = sizeof(test<T>(nullptr, nullptr)) == sizeof(char);
};

template <typename T>
struct _has_reflect_map_static_member_
{///must static member, but either const or non-const
private:
    template <typename U, const ReflectMapType* = &U::reflect_map>
    static std::true_type test(int);

    template <typename>
    static std::false_type test(...);

public:
    static const bool value = decltype(test<T>(0))::value;
};

template <typename T>
struct is_reflect_type
{
    static const bool value = _has_const_reflect_map_member_<T>::value && _has_reflect_map_static_member_<T>::value;
};

template <typename  T>
struct  is_vector_type :std::false_type {};

template <typename  T>
struct  is_vector_type<std::vector<T>> :std::true_type {};


template<typename T, typename Encoding, typename Allocator>
inline T get_number(const rapidjson::GenericValue<Encoding, Allocator>& value) {
    if (value.IsInt64())   return static_cast<T>(value.GetInt64());
    if (value.IsUint64())  return static_cast<T>(value.GetUint64());
    if (value.IsDouble())  return static_cast<T>(value.GetDouble());
    if (value.IsBool())    return static_cast<T>(value.GetBool());
    if (value.IsString()) {
        try {
            if (std::is_same<T, bool>::value)
            {
                const char* s = value.GetString();
                return strcmp(s, "true") == 0 || strcmp(s, "1") == 0;
            }else if (std::is_integral<T>::value)
            {
                return static_cast<T>(std::stoll(value.GetString()));
            }
            else if(std::is_floating_point<T>::value)
            {
                return std::stod(value.GetString());
            }
        }
        catch (...) {
            return T{};
        }
    }
    return T{};
}

template <typename  T,typename Encoding, typename Allocator>
struct  adapter;

template <typename Encoding, typename Allocator>
struct  adapter<uint8_t,Encoding,Allocator>
{
    using Type = uint8_t;
    inline static Type get(const rapidjson::GenericValue<Encoding,Allocator>&value) { return get_number<Type>(value);}
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetInt(newValue);}
};


template <typename Encoding, typename Allocator>
struct  adapter<int8_t,Encoding,Allocator>
{
    using Type = int8_t;
    inline static Type get(const rapidjson::GenericValue<Encoding,Allocator>&value) { return get_number<Type>(value);}
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetInt(newValue);}
};


template <typename Encoding, typename Allocator>
struct  adapter<uint16_t,Encoding,Allocator>
{
    using Type = uint16_t;
    inline static Type get(const rapidjson::GenericValue<Encoding,Allocator>&value) { return get_number<Type>(value);}
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetInt(newValue);}
};


template <typename Encoding, typename Allocator>
struct  adapter<int16_t,Encoding,Allocator>
{
    using Type = int16_t;
    inline static Type get(const rapidjson::GenericValue<Encoding,Allocator>&value) { return get_number<Type>(value);}
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetInt(newValue);}
};

template <typename Encoding, typename Allocator>
struct  adapter<int32_t,Encoding,Allocator>
{
    using Type = int32_t;
    inline static Type get(const rapidjson::GenericValue<Encoding,Allocator>&value) { return get_number<Type>(value);}
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetInt(newValue);}
};

template <typename Encoding, typename Allocator>
struct  adapter<uint32_t,Encoding,Allocator>
{
    using Type = uint32_t;
    inline static Type get(const rapidjson::GenericValue<Encoding, Allocator>&value) { return get_number<Type>(value); }
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetUint(newValue);}
};

template <typename Encoding, typename Allocator>
struct  adapter<int64_t,Encoding,Allocator>
{
    using Type = int64_t;
    inline static Type get(const rapidjson::GenericValue<Encoding, Allocator>&value) { return get_number<Type>(value); }
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetInt64(newValue);}
};
template <typename Encoding, typename Allocator>
struct  adapter<uint64_t,Encoding,Allocator>
{
    using Type = uint64_t;
    inline static Type get(const rapidjson::GenericValue<Encoding, Allocator>&value) { return get_number<Type>(value); }
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetUint64(newValue);}
};
template <typename Encoding, typename Allocator>
struct  adapter<double,Encoding,Allocator>
{
    using Type = double;
    inline static Type get(const rapidjson::GenericValue<Encoding, Allocator>&value) { return get_number<Type>(value); }
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetDouble(newValue);}
};
template <typename Encoding, typename Allocator>
struct  adapter<float,Encoding,Allocator>
{
    using Type = float;
    inline static Type get(const rapidjson::GenericValue<Encoding, Allocator>&value) { return get_number<Type>(value); }
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetDouble(newValue);}
};
template <typename Encoding, typename Allocator>
struct  adapter<bool,Encoding,Allocator>
{
    using Type = bool;
    inline static Type get(const rapidjson::GenericValue<Encoding, Allocator>&value) { return get_number<Type>(value); }
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetBool(newValue);}
};
template <typename Encoding, typename Allocator>
struct  adapter<std::string,Encoding,Allocator>
{
    using Type = std::string;
    inline static Type get(const rapidjson::GenericValue<Encoding,Allocator>&value)
    {
        if (value.IsString())  return value.GetString();
        if (value.IsInt64())   return std::to_string(value.GetInt64());
        if (value.IsDouble())  return std::to_string(value.GetDouble());
        return {};
    }
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,const Type& newValue,Allocator&allocator){value.SetString(newValue.c_str(), allocator);}
};

template <typename  T,typename Enable = void>
struct transform;


template <typename  T>
struct transform<T, typename std::enable_if<is_reflect_type<T>::value>::type>
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
        if (json.IsObject() && json.HasMember(key)) {
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
    static  bool from_json(const char*key, const JsonLocation &json, void*pThis, OffsetType offset)
    {
        if (key == nullptr) {///array
            auto&& datas = json[static_cast<int>(offset)];
            if (datas.IsArray() && datas.Size() > 0) {
                auto && dest = *reinterpret_cast<Type*>(pThis);
                dest.resize(datas.Size());
                for (int i = 0; i < datas.Size(); i++) {
                    transform<T>::from_json(nullptr, datas, &dest[i], i);
                }
            }
            return true;
        }
        if (json.IsObject() && json.HasMember(key)) {
            auto&& datas = json[key];
            if (datas.IsArray() && datas.Size() > 0) {
                auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) + offset);
                dest.resize(datas.Size());
                for (int i = 0; i < datas.Size(); i++) {
                    transform<T>::from_json(nullptr, datas, &dest[i], i);
                }
                return  true;
            }
        }
        return false;
    }
    static  void to_json(const char*key, rapidjson::Value &value, rapidjson::Document::AllocatorType &allocator, void*pThis, OffsetType offset)
    {
        if (key == nullptr) {///array
            auto && dest = *reinterpret_cast<Type*>(pThis);
            for (auto&&element : dest)
            {
                rapidjson::Value e;
                if (is_reflect_type<T>::value) {
                    e.SetObject();
                }
                else if (is_vector_type<T>::value) {
                    e.SetArray();
                }
                transform<T>::to_json(nullptr, e, allocator, &element, 0);
                value.PushBack(e, allocator);
            }
            return;
        }
        auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) + offset);
        rapidjson::Value elements(rapidjson::kArrayType);
        for (auto&&element : dest)
        {
            rapidjson::Value e;
            if (is_reflect_type<T>::value) {
                e.SetObject();
            }
            else if (is_vector_type<T>::value) {
                e.SetArray();
            }
            transform<T>::to_json(nullptr, e, allocator, &element, 0);
            elements.PushBack(e, allocator);
        }
        rapidjson::Value k;
        k.SetString(key, allocator);
        value.AddMember(k, elements, allocator);
    }
};

template <typename  T>
struct transform<T, typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value || std::is_same<T,std::string>::value>::type>
{
    using Type = T;
    using Encoding = typename JsonLocation::EncodingType;
    using Allocator = typename JsonLocation::AllocatorType;
    
    static  bool from_json(const char*key,const JsonLocation &json,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///array
            auto && dest = *reinterpret_cast<Type*>(pThis);
            dest = adapter<T,Encoding,Allocator>::get(json[static_cast<int>(offset)]);
            return true;
        }
        auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) + offset);
        if (json.IsObject() && json.HasMember(key)) {
            dest = adapter<T,Encoding,Allocator>::get(json[key]);
            return  true;
        }
        return false;
    }
    
    static  void to_json(const char*key,rapidjson::Value &value,rapidjson::Document::AllocatorType &allocator,void*pThis,OffsetType offset)
    {
        if (key == nullptr) {///array
            auto && dest = *reinterpret_cast<Type*>(pThis);
            adapter<T,Encoding,Allocator>::set(value,dest,allocator);
            return;
        }

        auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) +  offset);
        rapidjson::Value k;
        k.SetString(key, allocator);

        rapidjson::Value v;
        adapter<T,Encoding,Allocator>::set(v,dest,allocator);
        value.AddMember(k, v, allocator);
    }
};


inline bool transform_from_json(void *pThis,const ReflectMapType &reflect_map ,const JsonLocation& json)
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

inline bool transform_from_json(void *pThis,const ReflectMapType &reflect_map ,const std::string& json)
{
    rapidjson::Document doc;
    doc.Parse(json.c_str());

    if (doc.HasParseError()) {
        rapidjson::ParseErrorCode code = doc.GetParseError();
        size_t offset = doc.GetErrorOffset();
        printf("Parse Json Error: %s at %zu\n", rapidjson::GetParseError_En(code), offset);
        return  false;
    }
    rapidjson::Value& root = doc;
    if (root.IsObject()) {
        return transform_from_json(pThis, reflect_map, root);
    }
    return  false;
}

template<typename T>
bool transform_from_json(std::vector<T>& dest, const std::string& json)
{
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError() || !doc.IsArray()) return false;

    dest.resize(doc.Size());
    for (rapidjson::SizeType i = 0; i < doc.Size(); i++) {
        transform<T>::from_json(nullptr, doc, &dest[i], i);
    }
    return true;
}

inline void transform_to_json(void *pThis,rapidjson::Value &value,rapidjson::Document::AllocatorType &allocator,const ReflectMapType &reflect_map)
{
    for (auto&& p:reflect_map)
    {
        p.second.to_json(p.first,value,allocator,pThis,p.second.offset);
    }
}

inline std::string transform_to_json(void *pThis,const ReflectMapType &reflect_map)
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


template<typename T>
std::string transform_to_json(const std::vector<T>& src)
{
    rapidjson::Document doc;
    doc.SetArray();
    auto& allocator = doc.GetAllocator();
    for (size_t i = 0; i < src.size(); i++) {
        rapidjson::Value e;
        if (is_reflect_type<T>::value)      e.SetObject();
        else if (is_vector_type<T>::value)  e.SetArray();
        transform<T>::to_json(nullptr, e, allocator,
            const_cast<void*>(static_cast<const void*>(&src[i])), 0);
        doc.PushBack(e, allocator);
    }
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
    doc.Accept(writer);
    return buf.GetString();
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


#define ___EXPAND___(x) x


#define ___COUNT___ARGS___(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10, _11,_12,_13,_14,_15,_16,_17,_18,_19,_20,N,...) N
#define _COUNT_ARGS_(...) ___EXPAND___(___COUNT___ARGS___(__VA_ARGS__, 20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1))


#define ___CONCAT___(a, b) a##b
#define _CONCAT_(a, b) ___CONCAT___(a, b)


#define IMPLEMENT__JSON__AUTO__TRANSFORM(C, ...) \
const  auto_json::ReflectMapType C::reflect_map = {\
  ___EXPAND___(_CONCAT_(__ADD__JSON__KEY__, _COUNT_ARGS_(__VA_ARGS__))(C, __VA_ARGS__))  \
};
#endif /* JsonTransformAuto.h */
