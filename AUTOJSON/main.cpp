//
//  main.cpp
//  AUTOJSON
//
//  Created by Marvel on 2026/1/31.
//

#include <iostream>
#include <unordered_map>

#define TO_STRING(x) #x

namespace auto_json {


using OffsetType = size_t;

using Transform_From_FUNC = bool(*)(const char*key,const std::string &json,void*pThis,void*PType);

template <typename  T>
bool transfom_from_json(const char*key,const std::string &json,void*pThis,T * offset);

template <>
bool transfom_from_json<int>(const char*key,const std::string &json,void*pThis,int * offset)
{
    auto && dest = *reinterpret_cast<int*>(static_cast<char*>(pThis) + reinterpret_cast<OffsetType>(offset));
    dest = 1;
    //int *pDest = (int*)(((char*)pThis) + offset);
    std::cout << key << "--int-" << json << "---"  << pThis  << "--"  << offset   << std::endl;
    return true;
}

template <>
bool transfom_from_json<double>(const char*key,const std::string &json,void*pThis,double * offset)
{
    auto && dest = *reinterpret_cast<double*>(static_cast<char*>(pThis) + reinterpret_cast<OffsetType>(offset));
    dest = 9.9;
    std::cout << key << "--double-" << json << "---"  << pThis  << "--"  << offset   << std::endl;
    return true;
}

template <>
bool transfom_from_json<std::string>(const char*key,const std::string &json,void*pThis,std::string * offset)
{
    auto && dest = *reinterpret_cast<std::string*>(static_cast<char*>(pThis) + reinterpret_cast<OffsetType>(offset));
    dest = "9099009llkk";
    std::cout << key << "-string--" << json << "---"  << pThis  << "--"  << offset   << std::endl;
    return true;
}


};

struct JOSONODEL {
    int a{};
    double b{};
    std::string cc;
  

    const static std::unordered_map<const char*,std::pair<auto_json::OffsetType, auto_json::Transform_From_FUNC>> reflect_map;
    JOSONODEL(const std::string &json){
        for (auto&& p:reflect_map)
        {
           auto success =  p.second.second(p.first,json,this,reinterpret_cast<void*>(p.second.first));
            if (!success)
            {
            }
        }
    }
    
};

const  std::unordered_map<const char*,std::pair<auto_json::OffsetType, auto_json::Transform_From_FUNC>> JOSONODEL::reflect_map = {
    {TO_STRING(a),{offsetof(JOSONODEL, a),(auto_json::Transform_From_FUNC)auto_json::transfom_from_json<int>}},
    {TO_STRING(b),{offsetof(JOSONODEL, b),(auto_json::Transform_From_FUNC)auto_json::transfom_from_json<double>}},
    {TO_STRING(cc),{offsetof(JOSONODEL, cc),(auto_json::Transform_From_FUNC)auto_json::transfom_from_json<std::string >}},
};

int main(int argc, const char * argv[]) {
    std::string json = "{\"id\": \"1\",    \"name\": \"mmahew\",    \"url\": \"www.runoob.com\"}";
    
    
    JOSONODEL aa(json);
    
    
    std::cout << "Hello, World!\n";
    return 0;
}
