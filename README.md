# C++ 自动JSON 转换
 基于`rapidjson`

为类注入运行时反射机制，使得可以自动像高级语言一样，自动JSON转成模型，模型转JSON，而不需逐个字段手动转，支持：

- `double`    `float`   ` bool`   `int ` `unsigned int`  `int64_t`   `uint64_t` `std::string`
- `std::vector< T >` ，T 可以是上面的基础类型，或者嵌套的模型，但不支持：`std::vector< bool >` 因为C++模版库对`std::vector< bool > `进行了特化
- 支持模型嵌套
- 暂时不支持数组内嵌套数组

```c++
struct AA
{
    unsigned id;
    std::string name;
    bool online;
  
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(AA,id,name,online)
  
struct JOSONODEL {
    int a{};
    float b{};
    //float b0000{};
    std::string url;
    std::string name;//////支持：字符串转换
    std::vector<int> dd;
    double score;
    std::vector<std::string> boosaa;
    
    int id{};
    int a3{};
    int a4{};
    int a5{};
    int a6{};
    bool aaa000{};///支持：普通基础类型
    AA pppp; ///支持：模型嵌套 模型
    std::vector<AA> users; ///支持：嵌套模型数组
    std::vector<double>  intts; ///支持：普通类型数组 
    
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(JOSONODEL,a,b,url,dd,users,name,id,a3,a4,a5,a6,score,aaa000,boosaa,pppp,intts)


int main(int argc, const char * argv[]) {
    std::string json = "{\"id\": 1, \"b\":0.7, \"aaa000\":true, \"intts\":[9.1, 8.2, 7.3, 6.4, 5.5], \"pppp\":{ \"id\": 91, \"name\": \"iiuuuu\", \"online\": false }, \"score\": 9.71,    \"name\": \"mmahew\", \"users\": [{ \"id\": 100, \"name\": \"Tom\", \"online\": true },    { \"id\": 101, \"name\": \"Jerry\", \"online\": false } ],  \"boosaa\": [\"---8.8\", \"ppp9.9\", \"kkk0.800\"],  \"url\": \"www.runoob.com\"}";
    
    JOSONODEL aa;
    
    aa.transform_from_json(json); ///自动JONS 转模型
    
    auto sss = aa.transform_to_json();///自动模型 转JSON
  
    std::cout << "Hello, World!\n";
    return 0;
}
```

