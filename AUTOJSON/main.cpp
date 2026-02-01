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
    //float b0000{};
    std::string url;
    std::string name;
    std::vector<int> dd;
    double score;
    std::vector<std::string> boosaa;
    
    int id{};
    int a3{};
    int a4{};
    int a5{};
    int a6{};
    bool aaa000{};
    AA pppp;
    std::vector<AA> users;
    
    

    std::vector<double>  intts;
    
    
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(JOSONODEL,id,url,name,score,pppp,intts)
//IMPLEMENT__JSON__AUTO__TRANSFORM(JOSONODEL,a,b,url,dd,name,id,a3,a4,a5,a6,aaa000,users,boosaa)
IMPLEMENT__JSON__AUTO__TRANSFORM(AA,id,name,online)

int main(int argc, const char * argv[]) {
    std::string json = "{\"id\": 1, \"intts\":[9.1, 8.2, 7.3, 6.4, 5.5], \"pppp\":{ \"id\": 91, \"name\": \"iiuuuu\", \"online\": false }, \"score\": 9.71,    \"name\": \"mmahew\", \"users\": [{ \"id\": 100, \"name\": \"Tom\", \"online\": true },    { \"id\": 101, \"name\": \"Jerry\", \"online\": false } ],  \"boosaa\": [\"---8.8\", \"ppp9.9\", \"kkk0.800\"],  \"url\": \"www.runoob.com\"}";
    
    JOSONODEL aa;
    
    aa.transform_from_json(json);
    
    auto sss = aa.transform_to_json();
    
    //生成Json串
       rapidjson::Document jsonDoc;    //生成一个dom元素Document
       rapidjson::Document::AllocatorType &allocator = jsonDoc.GetAllocator(); //获取分配器
       jsonDoc.SetObject();    //将当前的Document设置为一个object，也就是说，整个Document是一个Object类型的dom元素

       //添加属性
       jsonDoc.AddMember("name", "qfl", allocator);    //添加字符串值
       jsonDoc.AddMember("age", 20, allocator);
    
    std::cout << "Hello, World!\n";
    return 0;
}
