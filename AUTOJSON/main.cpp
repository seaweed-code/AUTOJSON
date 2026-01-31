//
//  main.cpp
//  AUTOJSON
//
//  Created by Matthew on 2026/1/31.
//

#include <iostream>
#include <unordered_map>
#include <vector>
#define TO_STRING(x) #x

namespace auto_json {

using OffsetType = size_t;
using pFuncParse = bool(*)(const char*key,const std::string &json,void*pThis,void*offset);

using ReflectMapType = std::unordered_map<const char*,std::pair<OffsetType,pFuncParse>>;

template <typename  T>
struct transform;

template <typename  T>
struct transform<std::vector<T>>
{
    static  bool from_json(const char*key,const std::string &json,void*pThis,std::vector<T> * offset)
    {
        auto && dest = *reinterpret_cast<std::vector<T>*>(static_cast<char*>(pThis) + reinterpret_cast<OffsetType>(offset));
       // dest = true;
        //int *pDest = (int*)(((char*)pThis) + offset);
        std::cout << key << "--bool-" << json << "---"  << pThis  << "--"  << offset   << std::endl;
        return true;
    }
};

template <>
struct transform<bool>
{
    static  bool from_json(const char*key,const std::string &json,void*pThis,bool * offset)
    {
        auto && dest = *reinterpret_cast<bool*>(static_cast<char*>(pThis) + reinterpret_cast<OffsetType>(offset));
        dest = true;
        //int *pDest = (int*)(((char*)pThis) + offset);
        std::cout << key << "--bool-" << json << "---"  << pThis  << "--"  << offset   << std::endl;
        return true;
    }
};

template <>
struct transform<int>
{
    static  bool from_json(const char*key,const std::string &json,void*pThis,int * offset)
    {
        auto && dest = *reinterpret_cast<int*>(static_cast<char*>(pThis) + reinterpret_cast<OffsetType>(offset));
        dest = 1;
        //int *pDest = (int*)(((char*)pThis) + offset);
        std::cout << key << "--int-" << json << "---"  << pThis  << "--"  << offset   << std::endl;
        return true;
    }
};


template <>
struct transform<double>
{
  
    static  bool from_json(const char*key,const std::string &json,void*pThis,double * offset)
    {
        auto && dest = *reinterpret_cast<double*>(static_cast<char*>(pThis) + reinterpret_cast<OffsetType>(offset));
        dest = 9.9;
        std::cout << key << "--double-" << json << "---"  << pThis  << "--"  << offset   << std::endl;
        return true;
    }
};


template <>
struct transform<std::string>
{
    static  bool from_json(const char*key,const std::string &json,void*pThis,std::string * offset)
    {
        auto && dest = *reinterpret_cast<std::string*>(static_cast<char*>(pThis) + reinterpret_cast<OffsetType>(offset));
        dest = "9099009llkk";
        std::cout << key << "-string--" << json << "---"  << pThis  << "--"  << offset   << std::endl;
        return true;
    }
};



bool transform_from_json(void *pThis,const auto_json::ReflectMapType &reflect_map ,const std::string&json)
{
    for (auto&& p:reflect_map)
    {
       auto success =  p.second.second(p.first,json,pThis,reinterpret_cast<void*>(p.second.first));
        if (!success)
        {
        }
    }
    return  true;
}

};

struct JOSONODEL {
    int a{};
    double b{};
    std::string cc;

    std::vector<int> dd;

    const static auto_json::ReflectMapType reflect_map;
    inline bool transform_from_json(const std::string &json){return auto_json::transform_from_json(this, reflect_map, json);}
    
};

const  auto_json::ReflectMapType JOSONODEL::reflect_map = {
    {TO_STRING(a),{offsetof(JOSONODEL, a),(auto_json::pFuncParse)auto_json::transform<int>::from_json}},
    {TO_STRING(b),{offsetof(JOSONODEL, b),(auto_json::pFuncParse)auto_json::transform<double>::from_json}},
    {TO_STRING(cc),{offsetof(JOSONODEL, cc),(auto_json::pFuncParse)auto_json::transform<std::string>::from_json}},
    {TO_STRING(dd),{offsetof(JOSONODEL, dd),(auto_json::pFuncParse)auto_json::transform<std::vector<int>>::from_json}},
};

int main(int argc, const char * argv[]) {
    std::string json = "{\"id\": \"1\",    \"name\": \"mmahew\",    \"url\": \"www.runoob.com\"}";
    
    
    JOSONODEL aa;
    aa.transform_from_json(json);
    
    
    
    std::cout << "Hello, World!\n";
    return 0;
}
