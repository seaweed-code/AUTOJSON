//
//  main.cpp
//  AUTOJSON
//
//  Created by Matthew on 2026/1/31.
//
#include "JsonTransformAuto.h"

struct AA
{
    int aa;
    DECLARE__JSON__AUTO__TRANSFORM
};

struct JOSONODEL {
    int a{};
    double b{};
    std::string url;
    std::string name;
    std::vector<int> dd;
    
    int id{};
    int a3{};
    int a4{};
    int a5{};
    int a6{};
    bool aaa000;
    
    AA custom;
    
    
    
    DECLARE__JSON__AUTO__TRANSFORM
};

IMPLEMENT__JSON__AUTO__TRANSFORM(JOSONODEL,a,b,url,dd,name,id,a3,a4,a5,a6,aaa000,custom)
IMPLEMENT__JSON__AUTO__TRANSFORM(AA,aa)

int main(int argc, const char * argv[]) {
    std::string json = "{\"id\": 1,    \"name\": \"mmahew\",    \"url\": \"www.runoob.com\"}";
    
    JOSONODEL aa;
    
    aa.transform_from_json(json);
    
    
    {
        const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
       rapidjson::  Document d;
          d.Parse(json);

          // 2. Modify it by DOM.
        rapidjson::  Value& s = d["stars"];
          s.SetInt(s.GetInt() + 1);

          // 3. Stringify the DOM
        rapidjson:: StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
          d.Accept(writer);

          // Output {"project":"rapidjson","stars":11}
          std::cout << buffer.GetString() << std::endl;
    }
    
    
    std::cout << "Hello, World!\n";
    return 0;
}
