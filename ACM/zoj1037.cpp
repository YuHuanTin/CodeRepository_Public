#include <iostream>
#include <iomanip>
#include <vector>

template<typename T>
void print(std::vector<T> &V) {
    for (auto &one: V) { std::cout << one << ' '; }
    std::cout << '\n';
}

template<typename T>
void printXY(std::vector<T> &V, int X, int Y) {
    for (int i = 0; i < Y; ++i) {
        for (int j = 0; j < X; ++j) {
            std::cout << std::setw(5) << V[i * X + j];
        }
        std::cout << '\n';
    }
}

struct Data {
    int x;
    int y;
    int val;
    bool used;
};

enum Dir {
    NONE, UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT
};

int findPos(int X, int Y, std::vector<Data> &V) {
    for (int i = 0; i < V.size(); ++i) {
        if (V[i].x == X && V[i].y == Y) {
            return i;
        }
    }
    return -1;
}

std::vector<Dir> checkAvailableDir(int X, int Y, int sizeX, int sizeY, std::vector<Data> &V) {
    std::vector<Dir> result;
    if (X - 1 >= 0 && !V[findPos(X - 1, Y, V)].used) {
        result.push_back(LEFT);
    }
    if (X + 1 < sizeX && !V[findPos(X + 1, Y, V)].used) {
        result.push_back(RIGHT);
    }
    if (Y - 1 >= 0 && !V[findPos(X, Y - 1, V)].used) {
        result.push_back(UP);
    }
    if (Y + 1 < sizeY && !V[findPos(X, Y + 1, V)].used) {
        result.push_back(DOWN);
    }
    if (Y - 1 >= 0 && X - 1 >= 0 && !V[findPos(X - 1, Y - 1, V)].used) {
        result.push_back(UP_LEFT);
    }
    if (Y - 1 >= 0 && X + 1 < sizeX && !V[findPos(X + 1, Y - 1, V)].used) {
        result.push_back(UP_RIGHT);
    }
    if (Y + 1 < sizeY && X - 1 >= 0 && !V[findPos(X - 1, Y + 1, V)].used) {
        result.push_back(DOWN_LEFT);
    }
    if (Y + 1 < sizeY && X + 1 < sizeX && !V[findPos(X + 1, Y + 1, V)].used) {
        result.push_back(DOWN_RIGHT);
    }
    return result;
}

double minLen = 0;

void dfs(int Deep, int X, int Y, int sizeX, int sizeY, std::vector<Data> &V, double StepLen) {
    if (minLen != 0 && minLen < StepLen) {
        return;
    }
    if (Deep == sizeY * sizeX - 1) {
        if (X == 1 && Y == 0 || X == 0 && Y == 1) {
            StepLen += 1;
            if (minLen == 0 || StepLen < minLen) minLen = StepLen;
        } else if (X == 1 && Y == 1) {
            StepLen += 1.414213562;
            if (minLen == 0 || StepLen < minLen) minLen = StepLen;
        }
    }

    V[findPos(X, Y, V)].used = true;

    std::vector<Dir> availableDir = checkAvailableDir(X, Y, sizeX, sizeY, V);
    for (auto &oneDir: availableDir) {
        if (oneDir == RIGHT) {
            dfs(Deep + 1, X + 1, Y, sizeX, sizeY, V, StepLen + 1);
        } else if (oneDir == LEFT) {
            dfs(Deep + 1, X - 1, Y, sizeX, sizeY, V, StepLen + 1);
        } else if (oneDir == UP) {
            dfs(Deep + 1, X, Y - 1, sizeX, sizeY, V, StepLen + 1);
        } else if (oneDir == DOWN) {
            dfs(Deep + 1, X, Y + 1, sizeX, sizeY, V, StepLen + 1);
        } else if (oneDir == UP_LEFT) {
            dfs(Deep + 1, X - 1, Y - 1, sizeX, sizeY, V, StepLen + 1.414213562);
        } else if (oneDir == UP_RIGHT) {
            dfs(Deep + 1, X + 1, Y - 1, sizeX, sizeY, V, StepLen + 1.414213562);
        } else if (oneDir == DOWN_LEFT) {
            dfs(Deep + 1, X - 1, Y + 1, sizeX, sizeY, V, StepLen + 1.414213562);
        } else if (oneDir == DOWN_RIGHT) {
            dfs(Deep + 1, X + 1, Y + 1, sizeX, sizeY, V, StepLen + 1.414213562);
        }
    }
    V[findPos(X, Y, V)].used = false;
}


int main() {
    int caseNum = 0;
    std::cin >> caseNum;

    
    // 测试规律写的 dfs 算法
//    int x, y;
//    for (int i = 0; i < caseNum; ++i) {
//        minLen = 0;
//        std::cin >> x >> y;
//
//        std::vector<int> grid(x * y);
//        for (int j = 0; j < x * y; ++j) grid[j] = j;
//
//        std::vector<Data> data;
//        for (int xF = 0; xF < x; ++xF) {
//            for (int yF = 0; yF < y; ++yF) {
//                data.push_back({xF, yF, grid[yF * x + xF], false});
//            }
//        }
//
//        dfs(0, 0, 0, x, y, data, 0);
//
//        std::cout << "Scenario #" << i + 1 << ":\n" << std::fixed << std::setprecision(2) << minLen << '\n';
//    }

    // 实际提交的代码
    int x, y;
    for (int i = 0; i < caseNum; ++i) {
        std::cin >> x >> y;

        double res = x * y;
        if (x % 2 == 1 && y % 2 == 1) {
            res += 0.41421356237309504880;
        }

        std::cout << "Scenario #" << i + 1 << ":\n" << std::fixed << std::setprecision(2) << res << '\n';
    }
}


