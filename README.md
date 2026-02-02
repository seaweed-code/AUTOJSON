# C++ 自动JSON 转换
 基于`rapidjson`

为类注入运行时反射机制，使得可以自动把JSON转成模型，模型转JSON，而不需逐个字段手动转

```c++

struct AA
{
    unsigned id;
    std::string name;
    bool online;
    DECLARE__JSON__AUTO__TRANSFORM
};

struct JOSONODEL {
    int a{};
    float b{};
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
//IMPLEMENT__JSON__AUTO__TRANSFORM(JOSONODEL,users)
IMPLEMENT__JSON__AUTO__TRANSFORM(JOSONODEL,a,b,url,dd,users,name,id,a3,a4,a5,a6,score,aaa000,boosaa,pppp,intts)
IMPLEMENT__JSON__AUTO__TRANSFORM(AA,id,name,online)

int main(int argc, const char * argv[]) {
    std::string json = "{\"id\": 1, \"b\":0.7, \"aaa000\":true, \"intts\":[9.1, 8.2, 7.3, 6.4, 5.5], \"pppp\":{ \"id\": 91, \"name\": \"iiuuuu\", \"online\": false }, \"score\": 9.71,    \"name\": \"mmahew\", \"users\": [{ \"id\": 100, \"name\": \"Tom\", \"online\": true },    { \"id\": 101, \"name\": \"Jerry\", \"online\": false } ],  \"boosaa\": [\"---8.8\", \"ppp9.9\", \"kkk0.800\"],  \"url\": \"www.runoob.com\"}";
    
    JOSONODEL aa;
    
    aa.transform_from_json(json);
    
    auto sss = aa.transform_to_json();
  
    
    std::cout << "Hello, World!\n";
    return 0;
}
```

