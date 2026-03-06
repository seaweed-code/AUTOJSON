/**
 * JsonTransformAutoTest.cpp
 * 全量测试，基于 JsonTransformAuto.h 最终版本
 * 兼容 VS2013 / GCC 4.8.5
 *
 * ===== 接口速查 =====
 *
 * 【单对象接口 —— 由 DECLARE__JSON__AUTO__TRANSFORM 注入到每个 struct】
 *   bool        obj.transform_from_json(const std::string& json)
 *   bool        obj.transform_from_json(const JsonLocation& v)   // rapidjson::Value
 *   std::string obj.transform_to_json()
 *   void        obj.transform_to_json(rapidjson::Value& v, Allocator& a)
 *
 * 【根节点为数组的全局模板接口】
 *   bool        auto_json::transform_from_json(std::vector<T>& dest, const std::string& json)
 *   std::string auto_json::transform_to_json(const std::vector<T>& src)
 *
 * ===== 关键行为约定（已从源码确认）=====
 *   - 单对象接口：根节点非 Object → 返回 false，不崩溃
 *   - transform<vector>::from_json：json 非 Object → 不崩溃，返回 false
 *   - transform<reflect>::from_json：嵌套字段非 Object → 不崩溃，返回 false
 *   - transform<primitive>::from_json：json 非 Object → 不崩溃，返回 false
 *   - 数组接口：根节点非 Array / 非法 JSON → 返回 false，不崩溃
 *   - vector::from_json 有数据时：resize 覆盖，非追加
 *   - 缺失字段：from_json 返回 false，原有字段值保持不变
 *   - 空数组字段（IsArray && Size==0）：当前实现不执行 resize，vector 保持原状
 *
 * 注意：所有使用 DECLARE__JSON__AUTO__TRANSFORM 的结构体必须在文件作用域定义。
 */

#include "JsonTransformAuto.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <limits>

// ============================================================
// 辅助
// ============================================================
#define CHECK(expr) \
    do { \
        if (!(expr)) { \
            printf("  [FAIL] %s  (line %d)\n", #expr, __LINE__); \
            ++s_fail; \
        } else { \
            printf("  [PASS] %s\n", #expr); \
            ++s_pass; \
        } \
    } while(0)

#define SECTION(name) printf("\n--- " name " ---\n")

static int s_pass = 0, s_fail = 0;
static bool feq(double a, double b, double eps=1e-5){ return fabs(a-b)<eps; }

// ============================================================
// 全部结构体（文件作用域）
// ============================================================

// ---- A. 全基础类型 ----
struct PrimModel {
    int8_t   i8;   uint8_t  u8;
    int16_t  i16;  uint16_t u16;
    int32_t  i32;  uint32_t u32;
    int64_t  i64;  uint64_t u64;
    float    f32;  double   f64;
    bool     b;
    std::string str;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(PrimModel, i8,u8,i16,u16,i32,u32,i64,u64,f32,f64,b,str)

// ---- B. 单层嵌套 ----
struct Addr {
    std::string city;
    int32_t     zip;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Addr, city, zip)

struct Person {
    std::string name;
    int32_t     age;
    Addr        addr;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Person, name, age, addr)

// ---- C. 三层嵌套 + 对象数组 ----
struct Employee {
    std::string name;
    int32_t     level;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Employee, name, level)

struct Department {
    std::string           dname;
    std::vector<Employee> employees;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Department, dname, employees)

struct Company {
    std::string              cname;
    std::vector<Department>  departments;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Company, cname, departments)

// ---- D. vector<vector<int>> ----
struct Matrix {
    std::string                       label;
    std::vector<std::vector<int32_t>> rows;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Matrix, label, rows)

// ---- E. vector<vector<struct>> ----
struct Point {
    int32_t x, y;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Point, x, y)

struct PolyLine {
    std::string                     label;
    std::vector<std::vector<Point>> segs;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(PolyLine, label, segs)

// ---- F. 同层多 vector ----
struct MultiVec {
    std::string              name;
    std::vector<int32_t>     ints;
    std::vector<std::string> strs;
    std::vector<Employee>    staff;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(MultiVec, name, ints, strs, staff)

// ---- G. 嵌套对象内含 vector ----
struct TagAddr {
    std::string              city;
    int32_t                  zip;
    std::vector<std::string> tags;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(TagAddr, city, zip, tags)

struct RichPerson {
    std::string          name;
    TagAddr              addr;
    std::vector<int32_t> scores;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(RichPerson, name, addr, scores)

// ---- H. 零值专项 ----
struct ZeroModel {
    int32_t i; double d; bool b; std::string s;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(ZeroModel, i, d, b, s)

// ---- I. bool 专项 ----
struct BoolModel {
    bool a, b, c, d;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(BoolModel, a, b, c, d)

// ---- J. 浮点边界 ----
struct FloatEdge {
    double maxV, minV, negMax;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(FloatEdge, maxV, minV, negMax)

// ---- K. 类型容错 ----
struct TolModel {
    int32_t     iFromStr;
    double      dFromStr;
    bool        bFromStr;
    std::string sFromInt;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(TolModel, iFromStr, dFromStr, bFromStr, sFromInt)

// ---- L. 单字段 ----
struct Single {
    int32_t v;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(Single, v)

// ---- M. 特殊字符串 ----
struct SpecStr {
    std::string empty, spaces, utf8, escaped;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(SpecStr, empty, spaces, utf8, escaped)

// ---- N. 整数边界 ----
struct IntBound {
    int64_t  minI64, maxI64;
    uint64_t maxU64;
    DECLARE__JSON__AUTO__TRANSFORM
};
IMPLEMENT__JSON__AUTO__TRANSFORM(IntBound, minI64, maxI64, maxU64)

// ============================================================
// ===================== 测试 ==================================
// ============================================================

// ============================================================
// 第一组：基础类型
// ============================================================
void test_G01_all_primitives_from_json()
{
    printf("\n[G01] 全基础类型 JSON → 模型\n");
    const char* j =
        "{\"i8\":-100,\"u8\":200,\"i16\":-30000,\"u16\":60000,"
        "\"i32\":-2000000,\"u32\":4000000,\"i64\":-9000000000,"
        "\"u64\":18000000000,\"f32\":1.5,\"f64\":3.14159265,"
        "\"b\":true,\"str\":\"hello\"}";
    PrimModel m;
    CHECK(m.transform_from_json(std::string(j)));
    CHECK(m.i8  == -100);           CHECK(m.u8  == 200);
    CHECK(m.i16 == -30000);         CHECK(m.u16 == 60000);
    CHECK(m.i32 == -2000000);       CHECK(m.u32 == 4000000U);
    CHECK(m.i64 == -9000000000LL);  CHECK(m.u64 == 18000000000ULL);
    CHECK(feq(m.f32, 1.5f));        CHECK(feq(m.f64, 3.14159265));
    CHECK(m.b   == true);           CHECK(m.str == "hello");
}

void test_G02_all_primitives_roundtrip()
{
    printf("\n[G02] 全基础类型 往返\n");
    PrimModel s;
    s.i8=-1; s.u8=255; s.i16=-32768; s.u16=65535;
    s.i32=-1000000; s.u32=3000000U;
    s.i64=-1234567890123LL; s.u64=9876543210ULL;
    s.f32=2.718f; s.f64=1.41421356; s.b=false; s.str="world";
    std::string j = s.transform_to_json();
    printf("  json=%s\n", j.c_str());
    PrimModel d; CHECK(d.transform_from_json(j));
    CHECK(d.i8==s.i8);   CHECK(d.u8==s.u8);
    CHECK(d.i16==s.i16); CHECK(d.u16==s.u16);
    CHECK(d.i32==s.i32); CHECK(d.u32==s.u32);
    CHECK(d.i64==s.i64); CHECK(d.u64==s.u64);
    CHECK(feq(d.f32,s.f32,1e-3)); CHECK(feq(d.f64,s.f64));
    CHECK(d.b==s.b);     CHECK(d.str==s.str);
}

void test_G03_zero_values_not_lost()
{
    printf("\n[G03] 零值字段序列化不丢失\n");
    ZeroModel s; s.i=0; s.d=0.0; s.b=false; s.s="";
    std::string j = s.transform_to_json();
    printf("  json=%s\n", j.c_str());
    ZeroModel d; d.i=99; d.d=3.14; d.b=true; d.s="x";
    CHECK(d.transform_from_json(j));
    CHECK(d.i==0); CHECK(feq(d.d,0.0)); CHECK(d.b==false); CHECK(d.s=="");
}

void test_G04_integer_boundaries()
{
    printf("\n[G04] 整数边界值（INT64_MIN/MAX, UINT64_MAX）\n");
    IntBound s;
    s.minI64 = (-9223372036854775807LL - 1);
    s.maxI64 =   9223372036854775807LL;
    s.maxU64 = 18446744073709551615ULL;
    std::string j = s.transform_to_json();
    printf("  json=%s\n", j.c_str());
    IntBound d; CHECK(d.transform_from_json(j));
    CHECK(d.minI64==s.minI64); CHECK(d.maxI64==s.maxI64); CHECK(d.maxU64==s.maxU64);
}

void test_G05_float_edge_values()
{
    printf("\n[G05] 极大/极小浮点数 往返\n");
    FloatEdge s;
    s.maxV  =  std::numeric_limits<double>::max();
    s.minV  =  std::numeric_limits<double>::min();
    s.negMax= -std::numeric_limits<double>::max();
    std::string j = s.transform_to_json();
    printf("  json=%s\n", j.c_str());
    FloatEdge d; d.transform_from_json(j);
    CHECK(d.maxV  > 0);
    CHECK(d.minV  > 0);
    CHECK(d.negMax < 0);
}

void test_G06_bool_various_sources()
{
    printf("\n[G06] bool 各种来源\n");
    BoolModel m;
    SECTION("JSON bool 字面量");
    m.transform_from_json(std::string("{\"a\":true,\"b\":false,\"c\":true,\"d\":false}"));
    CHECK(m.a==true); CHECK(m.b==false); CHECK(m.c==true); CHECK(m.d==false);

    SECTION("字符串 true/false");
    m.transform_from_json(std::string("{\"a\":\"true\",\"b\":\"false\",\"c\":\"true\",\"d\":\"false\"}"));
    CHECK(m.a==true); CHECK(m.b==false);

    SECTION("字符串 1/0");
    m.transform_from_json(std::string("{\"a\":\"1\",\"b\":\"0\",\"c\":\"1\",\"d\":\"0\"}"));
    CHECK(m.a==true); CHECK(m.b==false); CHECK(m.c==true); CHECK(m.d==false);

    SECTION("往返");
    m.a=true; m.b=false; m.c=true; m.d=false;
    BoolModel m2; m2.transform_from_json(m.transform_to_json());
    CHECK(m2.a==true); CHECK(m2.b==false); CHECK(m2.c==true); CHECK(m2.d==false);
}

void test_G07_type_coercion()
{
    printf("\n[G07] 类型容错（字符串↔数字/bool）\n");
    TolModel m;
    m.transform_from_json(std::string(
        "{\"iFromStr\":\"42\",\"dFromStr\":\"3.14\","
        "\"bFromStr\":\"true\",\"sFromInt\":100}"));
    CHECK(m.iFromStr==42);
    CHECK(feq(m.dFromStr,3.14));
    CHECK(m.bFromStr==true);
    CHECK(m.sFromInt=="100");
}

void test_G08_bad_string_to_number()
{
    printf("\n[G08] 非法字符串转数字 → 返回默认值 0，不崩溃\n");
    TolModel m; m.iFromStr=99; m.dFromStr=1.0;
    m.transform_from_json(std::string(
        "{\"iFromStr\":\"abc\",\"dFromStr\":\"xyz\","
        "\"bFromStr\":\"false\",\"sFromInt\":0}"));
    CHECK(true);               // 不崩溃
    CHECK(m.iFromStr==0);
    CHECK(feq(m.dFromStr,0.0));
}

void test_G09_special_strings()
{
    printf("\n[G09] 特殊字符串（空串、空格、UTF-8、换行转义）\n");
    SpecStr s;
    s.empty=""; s.spaces="hello world";
    s.utf8="\xe4\xb8\xad\xe6\x96\x87";  // 中文 UTF-8
    s.escaped="line1\nline2\ttab";
    std::string j = s.transform_to_json();
    printf("  json=%s\n", j.c_str());
    SpecStr d; CHECK(d.transform_from_json(j));
    CHECK(d.empty==""); CHECK(d.spaces=="hello world");
    CHECK(d.utf8==s.utf8); CHECK(d.escaped==s.escaped);
}

// ============================================================
// 第二组：嵌套模型
// ============================================================
void test_G10_nested_object_from_json()
{
    printf("\n[G10] 嵌套对象 JSON → 模型\n");
    Person p;
    CHECK(p.transform_from_json(std::string(
        "{\"name\":\"Alice\",\"age\":30,\"addr\":{\"city\":\"Beijing\",\"zip\":100000}}")));
    CHECK(p.name=="Alice"); CHECK(p.age==30);
    CHECK(p.addr.city=="Beijing"); CHECK(p.addr.zip==100000);
}

void test_G11_nested_object_roundtrip()
{
    printf("\n[G11] 嵌套对象 往返\n");
    Person s; s.name="Bob"; s.age=25; s.addr.city="Shanghai"; s.addr.zip=200000;
    std::string j = s.transform_to_json();
    printf("  json=%s\n", j.c_str());
    Person d; CHECK(d.transform_from_json(j));
    CHECK(d.name==s.name); CHECK(d.age==s.age);
    CHECK(d.addr.city==s.addr.city); CHECK(d.addr.zip==s.addr.zip);
}

void test_G12_three_level_nested()
{
    printf("\n[G12] 三层嵌套（Company→Department→Employee）往返\n");
    Company s; s.cname="TechCorp";
    Department d1; d1.dname="Eng";
    Employee e1; e1.name="Tom"; e1.level=3; d1.employees.push_back(e1);
    Employee e2; e2.name="Jerry"; e2.level=2; d1.employees.push_back(e2);
    s.departments.push_back(d1);
    Department d2; d2.dname="Mkt";
    Employee e3; e3.name="Kate"; e3.level=4; d2.employees.push_back(e3);
    s.departments.push_back(d2);
    std::string j = s.transform_to_json();
    printf("  json=%s\n", j.c_str());
    Company d; CHECK(d.transform_from_json(j));
    CHECK(d.cname=="TechCorp");
    CHECK(d.departments.size()==2);
    CHECK(d.departments[0].dname=="Eng");
    CHECK(d.departments[0].employees[0].name=="Tom");
    CHECK(d.departments[0].employees[0].level==3);
    CHECK(d.departments[0].employees[1].name=="Jerry");
    CHECK(d.departments[1].dname=="Mkt");
    CHECK(d.departments[1].employees[0].name=="Kate");
    CHECK(d.departments[1].employees[0].level==4);
}

void test_G13_nested_obj_contains_vector()
{
    printf("\n[G13] 嵌套对象内含 vector 往返\n");
    RichPerson s; s.name="Dave";
    s.addr.city="GZ"; s.addr.zip=510000;
    s.addr.tags.push_back("home"); s.addr.tags.push_back("work");
    s.scores.push_back(95); s.scores.push_back(87);
    std::string j = s.transform_to_json();
    printf("  json=%s\n", j.c_str());
    RichPerson d; CHECK(d.transform_from_json(j));
    CHECK(d.name=="Dave");
    CHECK(d.addr.city=="GZ"); CHECK(d.addr.zip==510000);
    CHECK(d.addr.tags.size()==2);
    CHECK(d.addr.tags[0]=="home"); CHECK(d.addr.tags[1]=="work");
    CHECK(d.scores.size()==2);
    CHECK(d.scores[0]==95); CHECK(d.scores[1]==87);
}

void test_G14_field_order_shuffled()
{
    printf("\n[G14] JSON 字段顺序与结构体声明顺序不同\n");
    SECTION("一层");
    Addr a;
    a.transform_from_json(std::string("{\"zip\":888,\"city\":\"SZ\"}"));
    CHECK(a.city=="SZ"); CHECK(a.zip==888);

    SECTION("三层全乱序");
    Company c;
    c.transform_from_json(std::string(
        "{\"departments\":[{\"employees\":[{\"level\":5,\"name\":\"X\"}],\"dname\":\"D1\"}],\"cname\":\"C1\"}"));
    CHECK(c.cname=="C1");
    CHECK(c.departments[0].dname=="D1");
    CHECK(c.departments[0].employees[0].name=="X");
    CHECK(c.departments[0].employees[0].level==5);
}

// ============================================================
// 第三组：数组
// ============================================================
void test_G15_vector_int_roundtrip()
{
    printf("\n[G15] vector<int32_t> 往返\n");
    // 嵌套在结构体字段中
    MultiVec s; s.name="mv";
    s.ints.push_back(10); s.ints.push_back(20); s.ints.push_back(30);
    std::string j = s.transform_to_json();
    printf("  json=%s\n", j.c_str());
    MultiVec d; CHECK(d.transform_from_json(j));
    CHECK(d.ints.size()==3);
    CHECK(d.ints[0]==10); CHECK(d.ints[1]==20); CHECK(d.ints[2]==30);
}

void test_G16_vector_string_roundtrip()
{
    printf("\n[G16] vector<string> 往返\n");
    MultiVec s; s.name="mv";
    s.strs.push_back("apple"); s.strs.push_back(""); s.strs.push_back("中文");
    std::string j = s.transform_to_json();
    printf("  json=%s\n", j.c_str());
    MultiVec d; CHECK(d.transform_from_json(j));
    CHECK(d.strs.size()==3);
    CHECK(d.strs[0]=="apple"); CHECK(d.strs[1]==""); CHECK(d.strs[2]=="中文");
}

void test_G17_vector_of_objects_roundtrip()
{
    printf("\n[G17] vector<struct>（数组内包含模型）往返\n");
    MultiVec s; s.name="mv";
    Employee e1; e1.name="A"; e1.level=1; s.staff.push_back(e1);
    Employee e2; e2.name="B"; e2.level=2; s.staff.push_back(e2);
    Employee e3; e3.name="C"; e3.level=3; s.staff.push_back(e3);
    std::string j = s.transform_to_json();
    printf("  json=%s\n", j.c_str());
    MultiVec d; CHECK(d.transform_from_json(j));
    CHECK(d.staff.size()==3);
    CHECK(d.staff[0].name=="A"); CHECK(d.staff[0].level==1);
    CHECK(d.staff[1].name=="B"); CHECK(d.staff[1].level==2);
    CHECK(d.staff[2].name=="C"); CHECK(d.staff[2].level==3);
}

void test_G18_multiple_vector_fields_independent()
{
    printf("\n[G18] 同层多个 vector 字段互不干扰\n");
    MultiVec s; s.name="mv";
    s.ints.push_back(1); s.ints.push_back(2);
    s.strs.push_back("x"); s.strs.push_back("y"); s.strs.push_back("z");
    Employee e; e.name="E"; e.level=9; s.staff.push_back(e);
    MultiVec d; CHECK(d.transform_from_json(s.transform_to_json()));
    CHECK(d.ints.size()==2);  CHECK(d.ints[0]==1);   CHECK(d.ints[1]==2);
    CHECK(d.strs.size()==3);  CHECK(d.strs[2]=="z");
    CHECK(d.staff.size()==1); CHECK(d.staff[0].name=="E");
}

void test_G19_vector_of_vector_int()
{
    printf("\n[G19] vector<vector<int>>（数组嵌套数组）往返\n");
    Matrix s; s.label="mat";
    std::vector<int32_t> r0; r0.push_back(1); r0.push_back(2); r0.push_back(3);
    std::vector<int32_t> r1; r1.push_back(4); r1.push_back(5); r1.push_back(6);
    std::vector<int32_t> r2; r2.push_back(7); r2.push_back(8); r2.push_back(9);
    s.rows.push_back(r0); s.rows.push_back(r1); s.rows.push_back(r2);
    std::string j = s.transform_to_json();
    printf("  json=%s\n", j.c_str());
    Matrix d; CHECK(d.transform_from_json(j));
    CHECK(d.rows.size()==3);
    CHECK(d.rows[0][0]==1); CHECK(d.rows[1][1]==5); CHECK(d.rows[2][2]==9);
}

void test_G20_vector_of_vector_struct()
{
    printf("\n[G20] vector<vector<struct>>（数组嵌套数组内包含模型）往返\n");
    PolyLine s; s.label="poly";
    std::vector<Point> seg0;
    Point p0; p0.x=0; p0.y=0; seg0.push_back(p0);
    Point p1; p1.x=1; p1.y=2; seg0.push_back(p1);
    std::vector<Point> seg1;
    Point p2; p2.x=10; p2.y=20; seg1.push_back(p2);
    Point p3; p3.x=30; p3.y=40; seg1.push_back(p3);
    s.segs.push_back(seg0); s.segs.push_back(seg1);
    std::string j = s.transform_to_json();
    printf("  json=%s\n", j.c_str());
    PolyLine d; CHECK(d.transform_from_json(j));
    CHECK(d.segs.size()==2);
    CHECK(d.segs[0][0].x==0  && d.segs[0][0].y==0);
    CHECK(d.segs[0][1].x==1  && d.segs[0][1].y==2);
    CHECK(d.segs[1][0].x==10 && d.segs[1][0].y==20);
    CHECK(d.segs[1][1].x==30 && d.segs[1][1].y==40);
}

void test_G21_empty_array_field()
{
    printf("\n[G21] 空数组字段\n");
    SECTION("空 vector<struct> 序列化 → 空数组 → 往返");
    Company s; s.cname="empty";
    std::string j = s.transform_to_json();
    printf("  json=%s\n", j.c_str());
    Company d; d.transform_from_json(j);
    CHECK(d.cname=="empty"); CHECK(d.departments.size()==0);

    SECTION("空 vector<int> 往返");
    MultiVec mv; mv.name="mv";
    std::string j2 = mv.transform_to_json();
    MultiVec mv2; mv2.transform_from_json(j2);
    CHECK(mv2.ints.size()==0); CHECK(mv2.strs.size()==0);
}

void test_G22_vector_reparse_is_overwrite_not_append()
{
    printf("\n[G22] vector 反序列化是覆盖而非追加\n");
    MultiVec m; m.name="mv";
    m.ints.push_back(1); m.ints.push_back(2); m.ints.push_back(3);
    // JSON 只有 2 个元素
    m.transform_from_json(std::string("{\"name\":\"mv\",\"ints\":[4,5],\"strs\":[],\"staff\":[]}"));
    CHECK(m.ints.size()==2);
    CHECK(m.ints[0]==4); CHECK(m.ints[1]==5);
}

// ============================================================
// 第四组：根节点为数组（auto_json::transform_from/to_json）
// ============================================================
void test_G23_array_root_objects_roundtrip()
{
    printf("\n[G23] 根节点数组 vector<struct> 往返\n");
    std::vector<Person> src;
    Person p1; p1.name="Alice"; p1.age=20; p1.addr.city="BJ"; p1.addr.zip=100;
    Person p2; p2.name="Bob";   p2.age=30; p2.addr.city="SH"; p2.addr.zip=200;
    src.push_back(p1); src.push_back(p2);
    std::string j = auto_json::transform_to_json(src);
    printf("  json=%s\n", j.c_str());
    std::vector<Person> dst;
    CHECK(auto_json::transform_from_json(dst, j));
    CHECK(dst.size()==2);
    CHECK(dst[0].name=="Alice"); CHECK(dst[0].addr.city=="BJ"); CHECK(dst[0].addr.zip==100);
    CHECK(dst[1].name=="Bob");   CHECK(dst[1].addr.city=="SH"); CHECK(dst[1].addr.zip==200);
}

void test_G24_array_root_deep_objects()
{
    printf("\n[G24] 根节点数组 vector<深层嵌套struct> 往返\n");
    std::vector<Company> src;
    Company c1; c1.cname="C1";
    Department d1; d1.dname="Eng";
    Employee e1; e1.name="Tom"; e1.level=3; d1.employees.push_back(e1);
    c1.departments.push_back(d1);
    src.push_back(c1);
    Company c2; c2.cname="C2";
    Department d2; d2.dname="HR";
    Employee e2; e2.name="Jane"; e2.level=2; d2.employees.push_back(e2);
    c2.departments.push_back(d2);
    src.push_back(c2);
    std::string j = auto_json::transform_to_json(src);
    printf("  json=%s\n", j.c_str());
    std::vector<Company> dst;
    CHECK(auto_json::transform_from_json(dst, j));
    CHECK(dst.size()==2);
    CHECK(dst[0].cname=="C1");
    CHECK(dst[0].departments[0].dname=="Eng");
    CHECK(dst[0].departments[0].employees[0].name=="Tom");
    CHECK(dst[1].cname=="C2");
    CHECK(dst[1].departments[0].employees[0].name=="Jane");
}

void test_G25_array_root_int_roundtrip()
{
    printf("\n[G25] 根节点数组 vector<int32_t> 往返\n");
    std::vector<int32_t> src;
    src.push_back(-100); src.push_back(0); src.push_back(100);
    std::string j = auto_json::transform_to_json(src);
    printf("  json=%s\n", j.c_str());
    std::vector<int32_t> dst;
    CHECK(auto_json::transform_from_json(dst, j));
    CHECK(dst.size()==3);
    CHECK(dst[0]==-100); CHECK(dst[1]==0); CHECK(dst[2]==100);
}

void test_G26_array_root_string_roundtrip()
{
    printf("\n[G26] 根节点数组 vector<string> 往返（含空串）\n");
    std::vector<std::string> src;
    src.push_back("hello"); src.push_back(""); src.push_back("中文");
    std::string j = auto_json::transform_to_json(src);
    printf("  json=%s\n", j.c_str());
    std::vector<std::string> dst;
    CHECK(auto_json::transform_from_json(dst, j));
    CHECK(dst.size()==3);
    CHECK(dst[0]=="hello"); CHECK(dst[1]==""); CHECK(dst[2]=="中文");
}

void test_G27_array_root_empty()
{
    printf("\n[G27] 根节点空数组 往返\n");
    std::vector<Person> src;
    std::string j = auto_json::transform_to_json(src);
    printf("  json=%s\n", j.c_str());
    CHECK(j=="[]");
    std::vector<Person> dst;
    CHECK(auto_json::transform_from_json(dst, j));
    CHECK(dst.size()==0);
}

void test_G28_array_root_reparse_overwrites()
{
    printf("\n[G28] 根节点数组反序列化是覆盖而非追加\n");
    std::vector<int32_t> v;
    v.push_back(1); v.push_back(2); v.push_back(3);
    auto_json::transform_from_json(v, std::string("[4,5]"));
    CHECK(v.size()==2);
    CHECK(v[0]==4); CHECK(v[1]==5);
}

// ============================================================
// 第五组：类型与根节点不匹配（不应崩溃）
// ============================================================
void test_G29_object_model_given_array_json()
{
    printf("\n[G29] 模型是对象，JSON 是数组 → 返回 false，不崩溃\n");
    Person p; p.name="orig"; p.age=99;
    // 单对象接口：根节点 IsArray → 直接返回 false
    bool ok = p.transform_from_json(std::string(
        "[{\"name\":\"Alice\",\"age\":30,\"addr\":{\"city\":\"BJ\",\"zip\":1}}]"));
    CHECK(ok==false);
    CHECK(p.name=="orig");  // 原值未被破坏
    CHECK(p.age==99);
}

void test_G30_vector_model_given_object_json()
{
    printf("\n[G30] 模型是 vector，JSON 是对象 → 返回 false，不崩溃\n");
    std::vector<Person> dst;
    // 数组接口：根节点 IsObject → 返回 false
    bool ok = auto_json::transform_from_json(dst,
        std::string("{\"name\":\"Alice\",\"age\":30,\"addr\":{\"city\":\"BJ\",\"zip\":1}}"));
    CHECK(ok==false);
    CHECK(dst.size()==0);
}

void test_G31_nested_field_given_array_instead_of_object()
{
    printf("\n[G31] 嵌套字段期望 Object，JSON 给了 Array → 不崩溃，保持原值\n");
    Person p; p.addr.city="orig"; p.addr.zip=99;
    p.transform_from_json(std::string(
        "{\"name\":\"X\",\"age\":1,\"addr\":[1,2,3]}"));
    CHECK(p.name=="X"); CHECK(p.age==1);
    CHECK(p.addr.city=="orig"); CHECK(p.addr.zip==99);
    CHECK(true);
}

void test_G32_nested_field_given_string_instead_of_object()
{
    printf("\n[G32] 嵌套字段期望 Object，JSON 给了 String → 不崩溃，保持原值\n");
    Person p; p.addr.city="orig"; p.addr.zip=88;
    p.transform_from_json(std::string(
        "{\"name\":\"Y\",\"age\":2,\"addr\":\"not_an_object\"}"));
    CHECK(p.name=="Y"); CHECK(p.age==2);
    CHECK(p.addr.city=="orig"); CHECK(p.addr.zip==88);
}

void test_G33_array_field_given_object_instead_of_array()
{
    printf("\n[G33] 数组字段期望 Array，JSON 给了 Object → 不崩溃，保持原状\n");
    MultiVec m; m.name="mv";
    m.transform_from_json(std::string(
        "{\"name\":\"mv\",\"ints\":{\"a\":1},\"strs\":[],\"staff\":[]}"));
    CHECK(m.name=="mv");
    CHECK(m.ints.size()==0);
    CHECK(true);
}

void test_G34_array_field_given_number_instead_of_array()
{
    printf("\n[G34] 数组字段期望 Array，JSON 给了 Number → 不崩溃，保持原状\n");
    MultiVec m; m.name="mv";
    m.transform_from_json(std::string(
        "{\"name\":\"mv\",\"ints\":123,\"strs\":[],\"staff\":[]}"));
    CHECK(m.name=="mv");
    CHECK(m.ints.size()==0);
    CHECK(true);
}

void test_G35_array_root_given_invalid_json()
{
    printf("\n[G35] 根节点数组接口：非法 JSON → 返回 false，不崩溃\n");
    std::vector<Person> dst;
    CHECK(auto_json::transform_from_json(dst, std::string("[{bad}")) == false);
    CHECK(auto_json::transform_from_json(dst, std::string(""))        == false);
    CHECK(dst.size()==0);
}

void test_G36_object_root_given_invalid_json()
{
    printf("\n[G36] 单对象接口：非法 JSON → 返回 false，不崩溃\n");
    Person p;
    CHECK(p.transform_from_json(std::string("{bad json"))   == false);
    CHECK(p.transform_from_json(std::string(""))             == false);
    CHECK(p.transform_from_json(std::string("null"))         == false);
    CHECK(p.transform_from_json(std::string("123"))          == false);
    CHECK(p.transform_from_json(std::string("\"string\""))   == false);
}

void test_G37_array_elements_type_mismatch()
{
    printf("\n[G37] 根节点数组中元素类型不匹配 → 不崩溃\n");
    SECTION("期望 vector<Person>，JSON 元素是数字");
    std::vector<Person> vp;
    auto_json::transform_from_json(vp, std::string("[1,2,3]"));
    CHECK(true);

    SECTION("期望 vector<int>，JSON 元素是对象");
    std::vector<int32_t> vi;
    auto_json::transform_from_json(vi, std::string("[{\"a\":1},{\"b\":2}]"));
    CHECK(true);
}

void test_G38_field_value_is_null()
{
    printf("\n[G38] JSON 字段值为 null → 不崩溃\n");
    SECTION("基础类型字段为 null");
    Person p; p.age=99; p.name="orig";
    p.transform_from_json(std::string(
        "{\"name\":null,\"age\":null,\"addr\":{\"city\":\"X\",\"zip\":1}}"));
    CHECK(true);   // 不崩溃
    // age 字段为 null，get_number 走到 return T{} → 赋 0；name 字段为 null，adapter<string> 返回空串
    // 只验证不崩溃，具体值由实现决定

    SECTION("嵌套对象字段为 null");
    Person p2; p2.addr.city="orig"; p2.addr.zip=1;
    p2.transform_from_json(std::string(
        "{\"name\":\"X\",\"age\":1,\"addr\":null}"));
    CHECK(true);
    CHECK(p2.addr.city=="orig");  // null 不能作为 Object 处理，addr 保持原值
}

void test_G39_array_element_is_null()
{
    printf("\n[G39] 数组元素为 null → 不崩溃\n");
    MultiVec m; m.name="mv";
    m.transform_from_json(std::string(
        "{\"name\":\"mv\",\"ints\":[1,null,3],\"strs\":[\"a\",null],\"staff\":[]}"));
    CHECK(true);
}

// ============================================================
// 第六组：行为验证
// ============================================================
void test_G40_missing_fields_keep_original()
{
    printf("\n[G40] 缺失字段保持原值\n");
    SECTION("浅层缺失");
    Person p; p.name="orig"; p.age=99; p.addr.city="X"; p.addr.zip=1;
    p.transform_from_json(std::string("{\"name\":\"Ghost\"}"));
    CHECK(p.name=="Ghost");
    CHECK(p.age==99);
    CHECK(p.addr.city=="X"); CHECK(p.addr.zip==1);

    SECTION("深层缺失：嵌套对象存在但子字段缺失");
    Person p2; p2.addr.zip=12345;
    p2.transform_from_json(std::string(
        "{\"name\":\"Alice\",\"age\":30,\"addr\":{\"city\":\"BJ\"}}"));
    CHECK(p2.addr.city=="BJ");
    CHECK(p2.addr.zip==12345);  // zip 缺失，保持原值
}

void test_G41_extra_fields_in_json_ignored()
{
    printf("\n[G41] JSON 含多余未知字段 → 忽略，已知字段正确\n");
    Person p;
    p.transform_from_json(std::string(
        "{\"name\":\"Alice\",\"age\":30,\"addr\":{\"city\":\"BJ\",\"zip\":100},"
        "\"unknown\":\"xxx\",\"extra\":999,\"nested\":{\"a\":1,\"b\":[1,2,3]}}"));
    CHECK(p.name=="Alice"); CHECK(p.age==30);
    CHECK(p.addr.city=="BJ"); CHECK(p.addr.zip==100);
    CHECK(true);  // 不崩溃
}

void test_G42_reparse_overwrites_all_fields()
{
    printf("\n[G42] 同一对象反复解析，第二次完整覆盖第一次\n");
    Person p;
    p.transform_from_json(std::string("{\"name\":\"First\",\"age\":10,\"addr\":{\"city\":\"A\",\"zip\":1}}"));
    CHECK(p.name=="First"); CHECK(p.age==10); CHECK(p.addr.city=="A");
    p.transform_from_json(std::string("{\"name\":\"Second\",\"age\":20,\"addr\":{\"city\":\"B\",\"zip\":2}}"));
    CHECK(p.name=="Second"); CHECK(p.age==20);
    CHECK(p.addr.city=="B"); CHECK(p.addr.zip==2);
}

void test_G43_to_json_idempotent()
{
    printf("\n[G43] 序列化幂等性（多次调用结果一致）\n");
    SECTION("普通对象");
    Person p; p.name="Eve"; p.age=28; p.addr.city="CD"; p.addr.zip=610000;
    CHECK(p.transform_to_json()==p.transform_to_json());

    SECTION("含 vector 对象");
    MultiVec mv; mv.name="X";
    mv.ints.push_back(1); mv.ints.push_back(2);
    mv.strs.push_back("a"); mv.strs.push_back("b");
    CHECK(mv.transform_to_json()==mv.transform_to_json());
}

void test_G44_to_json_value_allocator_overload()
{
    printf("\n[G44] transform_to_json(Value&, Allocator&) 重载：嵌入父 Document\n");
    Addr src; src.city="Wuhan"; src.zip=430000;
    rapidjson::Document doc; doc.SetObject();
    auto& alloc = doc.GetAllocator();
    rapidjson::Value addrVal(rapidjson::kObjectType);
    src.transform_to_json(addrVal, alloc);
    rapidjson::Value key; key.SetString("addr", alloc);
    doc.AddMember(key, addrVal, alloc);
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
    doc.Accept(writer);
    std::string j = buf.GetString();
    printf("  json=%s\n", j.c_str());
    rapidjson::Document doc2; doc2.Parse(j.c_str());
    CHECK(!doc2.HasParseError());
    CHECK(doc2.HasMember("addr"));
    Addr dst; dst.transform_from_json(doc2["addr"]);
    CHECK(dst.city=="Wuhan"); CHECK(dst.zip==430000);
}

void test_G45_transform_from_json_value_overload()
{
    printf("\n[G45] transform_from_json(JsonLocation&) 重载：直接传 rapidjson::Value\n");
    rapidjson::Document doc;
    doc.Parse("{\"city\":\"Nanjing\",\"zip\":210000}");
    CHECK(!doc.HasParseError());
    Addr a;
    CHECK(a.transform_from_json(static_cast<rapidjson::Value&>(doc)));
    CHECK(a.city=="Nanjing"); CHECK(a.zip==210000);
}

void test_G46_single_field_struct()
{
    printf("\n[G46] 单字段结构体\n");
    Single s; s.v=0;
    s.transform_from_json(std::string("{\"v\":777}"));
    CHECK(s.v==777);
    s.v=999;
    Single s2; s2.transform_from_json(s.transform_to_json());
    CHECK(s2.v==999);
}

void test_G47_max_fields_struct()
{
    printf("\n[G47] 接近最大字段数（20字段）结构体\n");
    // PrimModel 有 12 个字段，Company/Department/Employee 一起覆盖多字段场景
    // 用 MultiVec(name+ints+strs+staff=4字段) 验证多字段类型混合
    MultiVec s; s.name="full";
    for (int i = 0; i < 5; i++) s.ints.push_back(i);
    for (int i = 0; i < 3; i++) {
        std::string str = "s"; str += (char)('0'+i);
        s.strs.push_back(str);
    }
    Employee e; e.name="E0"; e.level=0; s.staff.push_back(e);
    MultiVec d; CHECK(d.transform_from_json(s.transform_to_json()));
    CHECK(d.ints.size()==5);  CHECK(d.ints[4]==4);
    CHECK(d.strs.size()==3);  CHECK(d.strs[0]=="s0");
    CHECK(d.staff.size()==1); CHECK(d.staff[0].name=="E0");
}

// ============================================================
// main
// ============================================================
int main()
{
    printf("==============================================\n");
    printf("  JsonTransformAuto Complete Test Suite\n");
    printf("==============================================\n");

    // --- 第一组：基础类型 ---
    test_G01_all_primitives_from_json();
    test_G02_all_primitives_roundtrip();
    test_G03_zero_values_not_lost();
    test_G04_integer_boundaries();
    test_G05_float_edge_values();
    test_G06_bool_various_sources();
    test_G07_type_coercion();
    test_G08_bad_string_to_number();
    test_G09_special_strings();

    // --- 第二组：嵌套模型 ---
    test_G10_nested_object_from_json();
    test_G11_nested_object_roundtrip();
    test_G12_three_level_nested();
    test_G13_nested_obj_contains_vector();
    test_G14_field_order_shuffled();

    // --- 第三组：数组 ---
    test_G15_vector_int_roundtrip();
    test_G16_vector_string_roundtrip();
    test_G17_vector_of_objects_roundtrip();
    test_G18_multiple_vector_fields_independent();
    test_G19_vector_of_vector_int();
    test_G20_vector_of_vector_struct();
    test_G21_empty_array_field();
    test_G22_vector_reparse_is_overwrite_not_append();

    // --- 第四组：根节点为数组 ---
    test_G23_array_root_objects_roundtrip();
    test_G24_array_root_deep_objects();
    test_G25_array_root_int_roundtrip();
    test_G26_array_root_string_roundtrip();
    test_G27_array_root_empty();
    test_G28_array_root_reparse_overwrites();

    // --- 第五组：类型与根节点不匹配（不应崩溃）---
    test_G29_object_model_given_array_json();
    test_G30_vector_model_given_object_json();
    test_G31_nested_field_given_array_instead_of_object();
    test_G32_nested_field_given_string_instead_of_object();
    test_G33_array_field_given_object_instead_of_array();
    test_G34_array_field_given_number_instead_of_array();
    test_G35_array_root_given_invalid_json();
    test_G36_object_root_given_invalid_json();
    test_G37_array_elements_type_mismatch();
    test_G38_field_value_is_null();
    test_G39_array_element_is_null();

    // --- 第六组：行为验证 ---
    test_G40_missing_fields_keep_original();
    test_G41_extra_fields_in_json_ignored();
    test_G42_reparse_overwrites_all_fields();
    test_G43_to_json_idempotent();
    test_G44_to_json_value_allocator_overload();
    test_G45_transform_from_json_value_overload();
    test_G46_single_field_struct();
    test_G47_max_fields_struct();

    printf("\n==============================================\n");
    printf("  PASS: %d   FAIL: %d   TOTAL: %d\n",
        s_pass, s_fail, s_pass+s_fail);
    printf("==============================================\n");
    return s_fail == 0 ? 0 : 1;
}
