//
//  main.cpp
//  AUTOJSON
//
//  Created by Matthew on 2026/1/31.
//
#include "JsonTransformAuto.h"

struct AA
{
    int id;
    std::string name;
    bool online;
    DECLARE__JSON__AUTO__TRANSFORM
};

struct JOSONODEL {
    int a{};
    double b{};
    std::string url;
    std::string name;
    std::vector<int> dd;
    
    std::vector<std::string> boosaa;
    
    int id{};
    int a3{};
    int a4{};
    int a5{};
    int a6{};
    bool aaa000{};
    
    std::vector<AA> users;
    
    DECLARE__JSON__AUTO__TRANSFORM
};

IMPLEMENT__JSON__AUTO__TRANSFORM(JOSONODEL,a,b,url,dd,name,id,a3,a4,a5,a6,aaa000,users,boosaa)
IMPLEMENT__JSON__AUTO__TRANSFORM(AA,id,name,online)

int main(int argc, const char * argv[]) {
    std::string json = "{\"id\": 1,    \"name\": \"mmahew\", \"users\": [{ \"id\": 100, \"name\": \"Tom\", \"online\": true },    { \"id\": 101, \"name\": \"Jerry\", \"online\": false } ],  \"boosaa\": [\"---8.8\", \"ppp9.9\", \"kkk0.800\"],  \"url\": \"www.runoob.com\"}";
    
    JOSONODEL aa;
    
    aa.transform_from_json(json);
    
    
    
    std::cout << "Hello, World!\n";
    return 0;
}
