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
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"
#include "error/en.h"

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

template<typename T>
using _is_reflect_type_ =typename std::enable_if<std::is_same<decltype(T::reflect_map),const ReflectMapType>::value, ReflectMapType>::type;

template <typename T>
struct is_reflect_type
{
private:
	template <typename U, const ReflectMapType* = &U::reflect_map>
	static std::true_type test(int);

	template <typename>
	static std::false_type test(...);

public:
	static const bool value = decltype(test<T>(0))::value;
};




template <typename  T,typename Encoding, typename Allocator>
struct  adapter;

template <typename Encoding, typename Allocator>
struct  adapter<int,Encoding,Allocator>
{
    using Type = int;
    inline static Type get(const rapidjson::GenericValue<Encoding,Allocator>&value)  {return value.GetInt();}
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetInt(newValue);}
};

template <typename Encoding, typename Allocator>
struct  adapter<unsigned,Encoding,Allocator>
{
    using Type = unsigned;
    inline static Type get(const rapidjson::GenericValue<Encoding,Allocator>&value)  {return value.GetUint();}
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetUint(newValue);}
};

template <typename Encoding, typename Allocator>
struct  adapter<int64_t,Encoding,Allocator>
{
    using Type = int64_t;
   inline static Type get(const rapidjson::GenericValue<Encoding,Allocator>&value)  {return value.GetInt64();}
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetInt64(newValue);}
};
template <typename Encoding, typename Allocator>
struct  adapter<uint64_t,Encoding,Allocator>
{
    using Type = uint64_t;
   inline static Type get(const rapidjson::GenericValue<Encoding,Allocator>&value)  {return value.GetUint64();}
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetUint64(newValue);}
};
template <typename Encoding, typename Allocator>
struct  adapter<double,Encoding,Allocator>
{
    using Type = double;
   inline static Type get(const rapidjson::GenericValue<Encoding,Allocator>&value)  {return value.GetDouble();}
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetDouble(newValue);}
};
template <typename Encoding, typename Allocator>
struct  adapter<float,Encoding,Allocator>
{
    using Type = float;
   inline static Type get(const rapidjson::GenericValue<Encoding,Allocator>&value)  {return static_cast<Type>(value.GetDouble());}///rapidjson did not support float for this version
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetDouble(newValue);}
};
template <typename Encoding, typename Allocator>
struct  adapter<bool,Encoding,Allocator>
{
    using Type = bool;
    inline static Type get(const rapidjson::GenericValue<Encoding,Allocator>&value)  {return value.GetBool();}
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,Type newValue,Allocator&){value.SetBool(newValue);}
};
template <typename Encoding, typename Allocator>
struct  adapter<std::string,Encoding,Allocator>
{
    using Type = std::string;
    inline static Type get(const rapidjson::GenericValue<Encoding,Allocator>&value)  {return value.GetString();}
    inline static void set(rapidjson::GenericValue<Encoding,Allocator>&value,const Type newValue,Allocator&allocator){value.SetString(newValue.c_str(), allocator);}
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
            if (is_reflect_type<T>::value) {
                e.SetObject();
            }
            transform<T>::to_json(nullptr,e,allocator,&element,0);
            elements.PushBack(e, allocator);
        }
        rapidjson::Value k;
        k.SetString(key, allocator);
        value.AddMember(k, elements, allocator);//添加数组
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
        if (json.HasMember(key)) {
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
	doc.Parse(json.c_str());

	if (doc.HasParseError()) {
		rapidjson::ParseErrorCode code = doc.GetParseError();
		size_t offset = doc.GetErrorOffset();
		printf("Parse Json Error: %s at %zu\n", rapidjson::GetParseError_En(code), offset);
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



#define IMPLEMENT__JSON__AUTO__TRANSFORM_1(C,_1)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__1(C,_1)  \
};

#define IMPLEMENT__JSON__AUTO__TRANSFORM_2(C,_1,_2)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__2(C,_1,_2)  \
};


#define IMPLEMENT__JSON__AUTO__TRANSFORM_3(C,_1,_2,_3)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__3(C,_1,_2,_3)  \
};

#define IMPLEMENT__JSON__AUTO__TRANSFORM_4(C,_1,_2,_3,_4)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__4(C,_1,_2,_3,_4) \
};

#define IMPLEMENT__JSON__AUTO__TRANSFORM_5(C,_1,_2,_3,_4,_5)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__5(C,_1,_2,_3,_4,_5) \
};

#define IMPLEMENT__JSON__AUTO__TRANSFORM_6(C,_1,_2,_3,_4,_5,_6)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__6(C,_1,_2,_3,_4,_5,_6)  \
};

#define IMPLEMENT__JSON__AUTO__TRANSFORM_7(C,_1,_2,_3,_4,_5,_6,_7)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__7(C,_1,_2,_3,_4,_5,_6,_7)  \
};

#define IMPLEMENT__JSON__AUTO__TRANSFORM_8(C,_1,_2,_3,_4,_5,_6,_7,_8)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__8(C,_1,_2,_3,_4,_5,_6,_7,_8)  \
};
#define IMPLEMENT__JSON__AUTO__TRANSFORM_9(C,_1,_2,_3,_4,_5,_6,_7,_8,_9)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__9(C,_1,_2,_3,_4,_5,_6,_7,_8,_9)  \
};
#define IMPLEMENT__JSON__AUTO__TRANSFORM_10(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__10(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10)  \
};
#define IMPLEMENT__JSON__AUTO__TRANSFORM_11(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11)   \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__11(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11)   \
};
#define IMPLEMENT__JSON__AUTO__TRANSFORM_12(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__12(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12)  \
};
#define IMPLEMENT__JSON__AUTO__TRANSFORM_13(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__13(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13)  \
};
#define IMPLEMENT__JSON__AUTO__TRANSFORM_14(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__14(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14)  \
};
#define IMPLEMENT__JSON__AUTO__TRANSFORM_15(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__15(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15) \
};
#define IMPLEMENT__JSON__AUTO__TRANSFORM_16(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__16(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16)  \
};
#define IMPLEMENT__JSON__AUTO__TRANSFORM_17(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__17(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17)  \
};
#define IMPLEMENT__JSON__AUTO__TRANSFORM_18(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__18(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18)  \
};
#define IMPLEMENT__JSON__AUTO__TRANSFORM_19(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19)   \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__19(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19)  \
};
#define IMPLEMENT__JSON__AUTO__TRANSFORM_20(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20)  \
const  auto_json::ReflectMapType C::reflect_map = {\
    __ADD__JSON__KEY__20(C,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20)  \
};
#endif /* JsonTransformAuto.h */
