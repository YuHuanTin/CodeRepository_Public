#include <algorithm>
#include <cstdio>
#include <iostream>
#include <unordered_map>
#include <vector>

using std::unordered_map;
using std::vector;

template<typename T>
void print(std::vector<T> V) {
    for (auto &one: V) { std::cout << one << ' '; }
    std::cout << '\n';
}

void printLen(const std::string &Str, bool AddPrefixNewLen = false) {
    size_t len = 32;
    size_t strLen = Str.length();
    if (strLen > len) {
        throw std::runtime_error("strLen over len");
    }
    std::string s((len - strLen +1)/ 2, '-');

    if (AddPrefixNewLen) std::cout << '\n';
    if (strLen % 2 == 0) std::cout << '-';
    std::cout << s << Str << s << '\n';
}

template<typename T>
struct is_vector : std::false_type {};

template<typename T>
struct is_vector<vector<T>> : std::true_type {};

template<typename TClass, typename Tfn, typename... Targs>
void doTest(TClass &Class, Tfn Function, Targs &...Args) {
    printLen("input");
    (print(Args), ...);
    printLen("run");
    auto fn = [&]() {
        return (Class.*Function)(Args...);
    };
    auto output = [&](auto ret) {
        printLen("return val", true);
        if constexpr (std::is_same_v<decltype(ret), bool>) {
            std::cout << std::boolalpha << ret;
        } else if constexpr (is_vector<decltype(ret)>::value) {
            print(ret);
        } else {
            std::cout << ret;
        }
    };
    if constexpr (std::is_same_v<decltype((Class.*Function)(Args...)), void>) {
        fn();
    } else if constexpr (std::is_same_v<decltype((Class.*Function)(Args...)), bool>) {
        auto ret = fn();
        output(ret);
    } else if constexpr (is_vector<decltype((Class.*Function)(Args...))>::value) {
        auto ret = fn();
        output(ret);
    }
    printLen("output");
    (print(Args), ...);
}

class Solution {
public:
    vector<int> intersect(vector<int> &nums1, vector<int> &nums2) { return {1, 2, 3, 4}; }

    void foo() {

    }
};

int main() {
    std::cout << std::unitbuf;


    Solution s;
    vector<int> p {1, 1, 2};
    vector<int> p2 {0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
    vector<int> p3 {1};
    vector<int> p4 {1, 0};
    doTest(s, &Solution::intersect, p, p2);
    doTest(s, &Solution::foo);

    return 0;
}
