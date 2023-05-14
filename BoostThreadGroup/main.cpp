// request c++ 23 standard & boost::thread

#include <vector>
#include <chrono>
#include <boost/thread.hpp>
#include "boost/thread/detail/thread.hpp"
#include "boost/thread/detail/thread_group.hpp"

using namespace std;

const string strTable = R"( !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~)";

string getRandomStr(size_t StrLen) {
    string randomStr;
    for (int i = 0; i < StrLen; ++i) {
        randomStr.append(1, strTable[std::rand() % strTable.length()]);
    }
    return randomStr;
}

// 取出条件
bool selectCondition(const string &Str) {
    if (Str.contains("aac")) return true;
    return false;
}
int main() {
    cout << unitbuf;
    boost::thread_group threadGroup;
    // 加入随机数据
    auto begin = chrono::system_clock::now();
    const size_t containSize = 5000000;
    const size_t threadNum = 20;
    const size_t splitSize = containSize / threadNum;
    vector<string> randomStrs(containSize);
    for (int i = 0; i < threadNum; ++i) {
        threadGroup.create_thread([&, i]() {
            for (int j = i * splitSize; j < (i + 1) * splitSize; ++j) {
                randomStrs[j] = getRandomStr(std::rand() % 96 + 4);
            }
        });
    }
    threadGroup.join_all();
    cout << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - begin).count() << '\n';
    begin = chrono::system_clock::now();
    // 模拟 select
    vector<string_view> selectStr;
    for (int i = 0; i < threadNum; ++i) {
        threadGroup.create_thread([&,i]() {
            auto Begin = &randomStrs.at(0) + i * splitSize;
            auto End = &randomStrs.at(0) + (i + 1) * splitSize;
            while (Begin != End) {
                if (selectCondition(*Begin)) selectStr.emplace_back(Begin->c_str(), Begin->length());
                ++Begin;
            }
        });
    }
    threadGroup.join_all();
    cout << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - begin).count() << '\n';
    // 输出结果
    for (auto i : selectStr) {
        cout << i << '\n';
    }

    return 0;
}
