//
//  main.cpp
//  AUTOJSON
//
//  Created by Marvel on 2026/1/31.
//

#include <iostream>
#include <unordered_map>

#define TO_STRING(x) #x


using OffsetType = size_t;

using FuncPtr = bool(*)(const char*key,const std::string &json,void*pThis,void*PType);

template <typename  T>
bool ttt(const char*key,const std::string &json,void*pThis,T * offset);

template <>
bool ttt<int>(const char*key,const std::string &json,void*pThis,int * offset)
{
    auto && dest = *reinterpret_cast<int*>(static_cast<char*>(pThis) + reinterpret_cast<OffsetType>(offset));
    dest = 1;
    //int *pDest = (int*)(((char*)pThis) + offset);
    std::cout << key << "--int-" << json << "---"  << pThis  << "--"  << offset   << std::endl;
    return true;
}

template <>
bool ttt<double>(const char*key,const std::string &json,void*pThis,double * offset)
{
    std::cout << key << "--double-" << json << "---"  << pThis  << "--"  << offset   << std::endl;
    return true;
}

template <>
bool ttt<std::string>(const char*key,const std::string &json,void*pThis,std::string * offset)
{
    std::cout << key << "-string--" << json << "---"  << pThis  << "--"  << offset   << std::endl;
    return true;
}

struct JOSONODEL {
    int a{};
    double b{};
    std::string cc;
  

    const static std::unordered_map<const char*,std::pair<OffsetType, FuncPtr>> key_fn;
    JOSONODEL(const std::string &json){
        for (auto&& p:key_fn)
        {
           auto success =  p.second.second(p.first,json,this,(void*)p.second.first);
            if (!success)
            {
            }
        }
    }
    
};

const  std::unordered_map<const char*,std::pair<OffsetType, FuncPtr>> JOSONODEL::key_fn = {
    {TO_STRING(a),{offsetof(JOSONODEL, a),(FuncPtr)ttt<int>}},
    {TO_STRING(b),{offsetof(JOSONODEL, b),(FuncPtr)ttt<double>}},
    {TO_STRING(cc),{offsetof(JOSONODEL, cc),(FuncPtr)ttt<std::string >}},
};

int main(int argc, const char * argv[]) {
    std::string json = "{\"id\": \"1\",    \"name\": \"mmahew\",    \"url\": \"www.runoob.com\"}";
    
    
    JOSONODEL aa(json);
    
    
    std::cout << "Hello, World!\n";
    return 0;
}
