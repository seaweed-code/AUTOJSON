//
//  main.cpp
//  AUTOJSON
//
//  Created by Matthew on 2026/1/31.
//

#include <iostream>
#include <unordered_map>
#include <vector>



#define __ADD__JSON__KEY__1(MODEL,_1)  {#_1,{offsetof(MODEL, _1),(auto_json::pFuncParse)auto_json::transform<decltype(MODEL::_1)>::from_json}},


#define __ADD__JSON__KEY__2(MODEL,_1,_2)      __ADD__JSON__KEY__1(MODEL,_1) __ADD__JSON__KEY__1(MODEL,_2)
#define __ADD__JSON__KEY__3(MODEL,_1,_2,_3)   __ADD__JSON__KEY__2(MODEL,_1,_2) __ADD__JSON__KEY__1(MODEL,_3)
#define __ADD__JSON__KEY__4(MODEL,_1,_2,_3,_4)   __ADD__JSON__KEY__3(MODEL,_1,_2,_3) __ADD__JSON__KEY__1(MODEL,_4)


namespace auto_json {

using OffsetType = size_t;
using pFuncParse = bool(*)(const char*key,const std::string &json,void*pThis,OffsetType offset);

using ReflectMapType = std::unordered_map<const char*,std::pair<OffsetType,pFuncParse>>;

template <typename  T>
struct transform;

template <typename  T>
struct transform<std::vector<T>>
{
    using Type = std::vector<T>;
    static  bool from_json(const char*key,const std::string &json,void*pThis,OffsetType offset)
    {
        auto && dest = *reinterpret_cast<Type*>(static_cast<char*>(pThis) + offset);
       
        for (int i=0; i<4; i++) {
            dest.push_back(T{});
        }
        
        std::cout << key << "--bool-" << json << "---"  << pThis  << "--"  << offset   << std::endl;
        return true;
    }
};

template <>
struct transform<bool>
{
    static  bool from_json(const char*key,const std::string &json,void*pThis,OffsetType offset)
    {
        auto && dest = *reinterpret_cast<bool*>(static_cast<char*>(pThis) + offset);
        dest = true;
        //int *pDest = (int*)(((char*)pThis) + offset);
        std::cout << key << "--bool-" << json << "---"  << pThis  << "--"  << offset   << std::endl;
        return true;
    }
};

template <>
struct transform<int>
{
    static  bool from_json(const char*key,const std::string &json,void*pThis,OffsetType offset)
    {
        auto && dest = *reinterpret_cast<int*>(static_cast<char*>(pThis) + offset);
        dest = 1;
        //int *pDest = (int*)(((char*)pThis) + offset);
        std::cout << key << "--int-" << json << "---"  << pThis  << "--"  << offset   << std::endl;
        return true;
    }
};


template <>
struct transform<double>
{
    static  bool from_json(const char*key,const std::string &json,void*pThis,OffsetType offset)
    {
        auto && dest = *reinterpret_cast<double*>(static_cast<char*>(pThis) + offset);
        dest = 9.9;
        std::cout << key << "--double-" << json << "---"  << pThis  << "--"  << offset   << std::endl;
        return true;
    }
};


template <>
struct transform<std::string>
{
    static  bool from_json(const char*key,const std::string &json,void*pThis,OffsetType offset)
    {
        auto && dest = *reinterpret_cast<std::string*>(static_cast<char*>(pThis) +  offset);
        dest = "9099009llkk";
        std::cout << key << "-string--" << json << "---"  << pThis  << "--"  << offset   << std::endl;
        return true;
    }
};



bool transform_from_json(void *pThis,const auto_json::ReflectMapType &reflect_map ,const std::string&json)
{
    for (auto&& p:reflect_map)
    {
       auto success =  p.second.second(p.first,json,pThis,p.second.first);
        if (!success)
        {
        }
    }
    return  true;
}

};

#define DECLARE__JSON__AUTO__TRANSFORM  \
const static auto_json::ReflectMapType reflect_map;  \
inline bool transform_from_json(const std::string &json){return auto_json::transform_from_json(this, reflect_map, json);}





#define IMPLEMENT__JSON__AUTO__TRANSFORM(MODEL,...)

struct JOSONODEL {
    int a{};
    double b{};
    std::string cc;

    std::vector<int> dd;
    
    DECLARE__JSON__AUTO__TRANSFORM;
};

//IMPLEMENT__JSON__AUTO__TRANSFORM(JOSONODEL,a,b,cc,ddddddd);

const  auto_json::ReflectMapType JOSONODEL::reflect_map = {
    __ADD__JSON__KEY__4(JOSONODEL,a,b,cc,dd)
};




int main(int argc, const char * argv[]) {
    std::string json = "{\"id\": \"1\",    \"name\": \"mmahew\",    \"url\": \"www.runoob.com\"}";
    
    
    JOSONODEL aa;
    
    aa.transform_from_json(json);
    
    
    
    std::cout << "Hello, World!\n";
    return 0;
}
