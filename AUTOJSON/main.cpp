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
    std::string cc;

    std::vector<int> dd;
    
    int a2{};
    int a3{};
    int a4{};
    int a5{};
    int a6{};
    bool aaa000;
    
    AA custom;
    DECLARE__JSON__AUTO__TRANSFORM
};

IMPLEMENT__JSON__AUTO__TRANSFORM(JOSONODEL,a,b,cc,dd,a2,a3,a4,a5,a6,aaa000,custom)
IMPLEMENT__JSON__AUTO__TRANSFORM(AA,aa)

int main(int argc, const char * argv[]) {
    std::string json = "{\"id\": \"1\",    \"name\": \"mmahew\",    \"url\": \"www.runoob.com\"}";
    
  //  auto aa00 = auto_json::HasReflectType<AA>::value;
    //auto bb = auto_json::HasReflectType<JOSONODEL>::value;
    
    JOSONODEL aa;
    
    aa.transform_from_json(json);
    
    
    
    std::cout << "Hello, World!\n";
    return 0;
}
