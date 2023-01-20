#include <iostream>
#include <thread>
#include <list>
#include <string>
#include "Windows.h"
#include "easyx.h"

const bool DEBUG = false;
// 窗口大小
int gWidth = 680, gHeight = 500;

// 蛇的初始长度
const int snakeInitLen = 3;

// 蛇的面积 20 * 20 pixel
const int snakeStep = 20;

// 代表蛇的点位
struct snakeData {
    int x = 0;
    int y = 0;
};

class eatSnake {
public:
    // 初始化
    eatSnake() {
        std::srand(std::time(nullptr));

        // 初始一条蛇
        int x = (snakeStep * std::rand()) % gWidth;
        while (x - snakeInitLen * snakeStep < 0 || x + snakeInitLen * snakeStep > gWidth) {
            x = (snakeStep * std::rand()) % gWidth;
        }
        int y = (snakeStep * std::rand()) % gHeight;
        while (y - snakeInitLen * snakeStep < 0 || y + snakeInitLen * snakeStep > gHeight) {
            y = (snakeStep * std::rand()) % gHeight;
        }
        for (int i = 0; i < snakeInitLen; ++i) {
            snake.push_back({x + i * snakeStep, y});
        }

        // 画苹果
        drawApple();

        // 画蛇
        drawSnake();
    }

    // 进行下一步
    bool runNext(BYTE key) {
        if (key == 'A' || key == 'D' || key == 'W' || key == 'S')
            inputKey = key;
        else
            return true;

        // 输入一个方向开始游戏
        if (inputKey != '\0') {
            // 画出分数 判断是否碰到边界
            RECT text{0, 0, 200, 200};
            if (snake.front().x - snakeStep < 0 || snake.front().x + snakeStep >= gWidth
                || snake.front().y - snakeStep < 0 || snake.front().y + snakeStep >= gHeight) {
                drawtext(("Game Over! your score is: " + std::to_string(snake.size())).c_str(), &text, DT_LEFT);
                return false;
            }
            drawtext(std::to_string(snake.size()).c_str(), &text, DT_LEFT);

            // 移动蛇
            int nextX = snake.front().x;
            int nextY = snake.front().y;
            switch (key) {
                case 'A':
                    nextX -= snakeStep;
                    break;
                case 'D':
                    nextX += snakeStep;
                    break;
                case 'W':
                    nextY -= snakeStep;
                    break;
                case 'S':
                    nextY += snakeStep;
                    break;
                default:
                    break;
            }
            snake.push_front({nextX, nextY});
            if (DEBUG) {
                std::cout << "key: " << inputKey << '\n';

                switch (getpixel(snake.front().x + 1, snake.front().y + 1)) {
                    case RED:
                        std::cout << "next head color is: red" << '\n';
                        break;
                    case BLUE:
                        std::cout << "next head color is: blue" << '\n';
                        break;
                    case BLACK:
                        std::cout << "next head color is: black" << '\n';
                        break;
                    default:
                        break;
                }
                std::cout << "next head x: " << snake.front().x << " y : " << snake.front().y << '\n';
            }
            if (getpixel(snake.front().x + 1, snake.front().y + 1) == RED) {
                drawSnake();
                drawApple();
            } else {
                setfillcolor(BLACK);
                solidrectangle(snake.back().x, snake.back().y, snake.back().x + snakeStep, snake.back().y + snakeStep);
                snake.pop_back();
                drawSnake();
            }


        }
        return true;
    }

private:
    std::list<snakeData> snake;
    BYTE inputKey = '\0';

    // 画出线条
    void drawLines() {
        setlinecolor(GREEN);
        for (int i = 0; i <= gWidth; i += snakeStep) {
            line(i, 0, i, gHeight);
        }
        for (int i = 0; i <= gHeight; i += snakeStep) {
            line(0, i, gWidth, i);
        }
    }

    // 画出蛇
    void drawSnake() {
        setfillcolor(WHITE);
        for (auto &i: snake) {
            solidrectangle(i.x, i.y, i.x + snakeStep, i.y + snakeStep);
        }
        setfillcolor(BLUE);
        solidrectangle(snake.front().x, snake.front().y, snake.front().x + snakeStep, snake.front().y + snakeStep);

        drawLines();
    }

    // 画出苹果
    void drawApple() {
        int x = (snakeStep * std::rand()) % gWidth;
        int y = (snakeStep * std::rand()) % gHeight;
        while (x - snakeStep < 0 || x + snakeStep > gWidth ||
               y - snakeStep < 0 || y + snakeStep > gHeight ||
               [&x, &y, this] {
                   for (auto &one: snake) {
                       if (x == one.x && y == one.y)
                           return true;
                   }
                   return false;
               }()) {
            x = (snakeStep * std::rand()) % gWidth;
            y = (snakeStep * std::rand()) % gHeight;
        }


        setfillcolor(RED);
        solidrectangle(x, y, x + snakeStep, y + snakeStep);

        if (DEBUG) {
            std::cout << "make a apple at: x: " << x << " y: " << y << '\n';
        }
    }
};

int main() {
    setbuf(stdout, nullptr);

    //初始化窗口
    initgraph(gWidth + 1, gHeight + 1, 0);
    eatSnake game;
    ExMessage msg;

    while (true) {
        peekmessage(&msg, EX_KEY, true);
        if (!game.runNext(msg.vkcode))
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(24h);
    // 关闭图形模式
    closegraph();
    return 0;
}
