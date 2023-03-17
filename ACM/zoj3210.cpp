#include <iostream>
#include <iomanip>

int main() {

    int caseNum;
    std::cin >> caseNum;
    for (int i = 0; i < caseNum; ++i) {
        int len;
        std::cin >> len;
        int arr1[100] = {0};
        int arr2[100] = {0};
        int cnt = 0;
        for (int j = 0; j < len; ++j)
            std::cin >> arr1[j];
        for (int j = 0; j < len; ++j)
            std::cin >> arr2[j];

        auto isQueue = [&]{
            for (int j = 0; j < len; ++j) {
                if (arr1[j] != arr2[j]) {
                    return false;
                }
            }
            return true;
        };
        auto isStack = [&] {
            for (int j = 0; j < len; ++j) {
                if (arr1[j] != arr2[len - j - 1]) {
                    return false;
                }
            }
            return true;
        };
        if (isStack()) {
            cnt = 1;
        }
        if (isQueue()) {
            if (cnt == 1) cnt = 3;
            else cnt = 2;
        }
        std::string out[] = {"neither", "stack", "queue", "both"};
        std::cout << out[cnt] << '\n';
    }
}
