#include <iostream>
#include <thread>
#include <list>
#include <string>
#include "Windows.h"
#include "easyx.h"

const bool DEBUG = false;
// ���ڴ�С
int gWidth = 680, gHeight = 500;

// �ߵĳ�ʼ����
const int snakeInitLen = 3;

// �ߵ���� 20 * 20 pixel
const int snakeStep = 20;

// �����ߵĵ�λ
struct snakeData {
    int x = 0;
    int y = 0;
};

class eatSnake {
public:
    // ��ʼ��
    eatSnake() {
        std::srand(std::time(nullptr));

        // ��ʼһ����
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

        // ��ƻ��
        drawApple();

        // ����
        drawSnake();
    }

    // ������һ��
    bool runNext(BYTE key) {
        if (key == 'A' || key == 'D' || key == 'W' || key == 'S')
            inputKey = key;
        else
            return true;

        // ����һ������ʼ��Ϸ
        if (inputKey != '\0') {
            // �ƶ���
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

            // �������� �ж��Ƿ������߽�
            RECT text{0, 0, 200, 200};
            if (nextX < 0 || nextX >= gWidth
                || nextY < 0 || nextY >= gHeight) {
                drawtext(("Game Over! your score is: " + std::to_string(snake.size())).c_str(), &text, DT_LEFT);
                return false;
            }
            drawtext(std::to_string(snake.size()).c_str(), &text, DT_LEFT);


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
            switch (getpixel(snake.front().x + 1, snake.front().y + 1)) {
                case RED:
                    drawSnake();
                    drawApple();
                    break;
                case BLACK:
                    setfillcolor(BLACK);
                    solidrectangle(snake.back().x, snake.back().y, snake.back().x + snakeStep, snake.back().y + snakeStep);
                    snake.pop_back();
                    drawSnake();
                    break;
                case WHITE:
                    drawtext(("Game Over! your score is: " + std::to_string(snake.size() - 1)).c_str(), &text, DT_LEFT);
                    return false;
                    break;
                default:
                    break;
            }

        }
        return true;
    }

private:
    std::list<snakeData> snake;
    BYTE inputKey = '\0';

    // ��������
    void drawLines() {
        setlinecolor(GREEN);
        for (int i = 0; i <= gWidth; i += snakeStep) {
            line(i, 0, i, gHeight);
        }
        for (int i = 0; i <= gHeight; i += snakeStep) {
            line(0, i, gWidth, i);
        }
    }

    // ������
    void drawSnake() {
        setfillcolor(WHITE);
        for (auto &i: snake) {
            solidrectangle(i.x, i.y, i.x + snakeStep, i.y + snakeStep);
        }
        setfillcolor(BLUE);
        solidrectangle(snake.front().x, snake.front().y, snake.front().x + snakeStep, snake.front().y + snakeStep);

        drawLines();
    }

    // ����ƻ��
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

    //��ʼ������
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
    // �ر�ͼ��ģʽ
    closegraph();
    return 0;
}
