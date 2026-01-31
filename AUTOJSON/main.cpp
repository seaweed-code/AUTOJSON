//
//  main.cpp
//  AUTOJSON
//
//  Created by Matthew on 2026/1/31.
//
#include "JsonTransformAuto.h"

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
    
    DECLARE__JSON__AUTO__TRANSFORM
};

IMPLEMENT__JSON__AUTO__TRANSFORM(JOSONODEL,a,b,cc,dd,a2,a3,a4,a5,a6,aaa000)


int main(int argc, const char * argv[]) {
    std::string json = "{\"id\": \"1\",    \"name\": \"mmahew\",    \"url\": \"www.runoob.com\"}";
    
    
    JOSONODEL aa;
    
    aa.transform_from_json(json);
    
    
    
    std::cout << "Hello, World!\n";
    return 0;
}
