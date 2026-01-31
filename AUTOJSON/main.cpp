//
//  main.cpp
//  AUTOJSON
//
//  Created by Matthew on 2026/1/31.
//
#include "AutoJson.h"

struct JOSONODEL {
    int a{};
    double b{};
    std::string cc;

    std::vector<int> dd;
    
    DECLARE__JSON__AUTO__TRANSFORM
};

IMPLEMENT__JSON__AUTO__TRANSFORM(JOSONODEL,a,b,cc,dd)


int main(int argc, const char * argv[]) {
    std::string json = "{\"id\": \"1\",    \"name\": \"mmahew\",    \"url\": \"www.runoob.com\"}";
    
    
    JOSONODEL aa;
    
    aa.transform_from_json(json);
    
    
    
    std::cout << "Hello, World!\n";
    return 0;
}
