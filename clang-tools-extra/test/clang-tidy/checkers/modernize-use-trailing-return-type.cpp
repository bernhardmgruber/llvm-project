// RUN: %check_clang_tidy -std=c++14-or-later %s modernize-use-trailing-return-type %t -- -- -fdeclspec -fexceptions -DCOMMAND_LINE_INT=int

namespace std {
    template <typename T>
    class vector;

    template <typename T, int N>
    class array;

    class string;

    template <typename T>
    class basic_ostream;

    using ostream = basic_ostream<char>;

    template <typename T>
    auto declval() -> T;

    template <typename... Ts>
    class tuple;
}

//
// Name collisions
//
struct Object { long long value; };

Object j1(unsigned Object) { return {Object * 2}; }
// CHECK-MESSAGES: :[[@LINE-1]]:8: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}Object j1(unsigned Object) { return {Object * 2}; }{{$}}
::Object j1(unsigned Object);
// CHECK-MESSAGES: :[[@LINE-1]]:10: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}auto j1(unsigned Object) -> ::Object;{{$}}
const Object& j2(unsigned a, int b, char Object, long l);
// CHECK-MESSAGES: :[[@LINE-1]]:15: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}const Object& j2(unsigned a, int b, char Object, long l);{{$}}
const struct Object& j2(unsigned a, int b, char Object, long l);
// CHECK-MESSAGES: :[[@LINE-1]]:22: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}auto j2(unsigned a, int b, char Object, long l) -> const struct Object&;{{$}}
std::vector<Object> j3(unsigned Object);
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}std::vector<Object> j3(unsigned Object);{{$}}
std::vector<const Object> j7(unsigned Object);
// CHECK-MESSAGES: :[[@LINE-1]]:27: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}std::vector<const Object> j7(unsigned Object);{{$}}
std::vector<Object> j4(unsigned vector);
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}auto j4(unsigned vector) -> std::vector<Object>;{{$}}
std::vector<::Object> j4(unsigned vector);
// CHECK-MESSAGES: :[[@LINE-1]]:23: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}auto j4(unsigned vector) -> std::vector<::Object>;{{$}}
std::vector<struct Object> j4(unsigned vector);
// CHECK-MESSAGES: :[[@LINE-1]]:28: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}auto j4(unsigned vector) -> std::vector<struct Object>;{{$}}
std::vector<Object> j4(unsigned Vector);
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}auto j4(unsigned Vector) -> std::vector<Object>;{{$}}
using std::vector;
vector<Object> j5(unsigned vector);
// CHECK-MESSAGES: :[[@LINE-1]]:16: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}vector<Object> j5(unsigned vector);{{$}}
constexpr auto Size = 5;
std::array<int, Size> j6(unsigned Size);
// CHECK-MESSAGES: :[[@LINE-1]]:23: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}std::array<int, Size> j6(unsigned Size);{{$}}
std::array<decltype(Size), (Size * 2) + 1> j8(unsigned Size);
// CHECK-MESSAGES: :[[@LINE-1]]:44: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}std::array<decltype(Size), (Size * 2) + 1> j8(unsigned Size);{{$}}
using std::ostream;
std::tuple<int, std::string, ostream>& operator<<(ostream& ostream, float i);
// CHECK-MESSAGES: :[[@LINE-1]]:40: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}std::tuple<int, std::string, ostream>& operator<<(ostream& ostream, float i);{{$}}

class CC {
    int Object;
    struct Object m();
// CHECK-MESSAGES: :[[@LINE-1]]:19: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}    auto m() -> struct Object;{{$}}
};
Object CC::m() { return {0}; }
// CHECK-MESSAGES: :[[@LINE-1]]:12: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}auto CC::m() -> Object { return {0}; }{{$}}
class DD : public CC {
    ::Object g();
// CHECK-MESSAGES: :[[@LINE-1]]:14: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}    auto g() -> ::Object;{{$}}
};
Object DD::g() {
// CHECK-MESSAGES: :[[@LINE-1]]:12: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}auto DD::g() -> Object {{{$}}
    return {0};
}

//
// bug 44206, no rewrite should happen due to collision with parameter name
//

ostream& operator<<(ostream& ostream, int i);
// CHECK-MESSAGES: :[[@LINE-1]]:10: warning: use a trailing return type for this function [modernize-use-trailing-return-type]
// CHECK-FIXES: {{^}}ostream& operator<<(ostream& ostream, int i);{{$}}

//
// Samples which do not trigger the check
//

auto f() -> int;
auto f(int) -> int;
auto f(int arg) -> int;
auto f(int arg1, int arg2, int arg3) -> int;
auto f(int arg1, int arg2, int arg3, ...) -> int;
template <typename T> auto f(T t) -> int;

auto ff();

void c();
void c(int arg);
void c(int arg) { return; }

struct D2 : B {
    D2();
    virtual ~D2();
    
    virtual auto f1(int, bool b) -> double final;
    virtual auto base2(int, bool b) -> double override;
    virtual auto base3() const -> float override final { }

    operator double();
};

auto l1 = [](int arg) {};
auto l2 = [](int arg) -> double {};
