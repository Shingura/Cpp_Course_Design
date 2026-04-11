#pragma once
#include <iostream>
#include <string>
#include <random>
#include <limits>
#include <algorithm>
#include <utility>

// =============================================
// 玩家基类：可能为真人玩家或电脑
// =============================================

class Player {
    protected: // 使用 protected 使子类（HumanPlayer 和 ComputerPlayer）可访问随机数引擎，但外部代码无法直接访问
        static std::mt19937 gen;
        // static: 1. 保证所有 Player 及其子类共享同一个引擎，避免每次重新初始化，提高运行效率
        //         2. 静态成员变量的声明与初始化分离，确保全局唯一，定义位于 line:45
        // 生成随机数还可以使用 C 标准库的 rand() 函数，代码简单，但不支持分布控制，生成随机整数时需要通过取余（%）手动计算，例如：rand() % 6 + 1。

    public:
        std::string name;
        int chips;  // 剩余赌注
        int dice;   // 本轮骰点
        int bet;    // 本轮下注

        Player(std::string name, int chips)
            : name(std::move(name)), chips(chips), dice(0), bet(0) {} // 使用 std::move 避免不必要的拷贝，提高性能

        virtual ~Player() = default; // 默认析构函数

        // 掷骰子，结果存入 dice
        void rollDice() {
            static std::uniform_int_distribution<int> dist(1, 6); // 生成均匀分布的整数随机数，同 line:14，static 保证分布只初始化一次
            dice = dist(gen);
        }

        // 判定玩家是否获胜
        bool isAlive() const {
            return chips > 0;
        }

        // 纯虚函数：子类（真人和计算机）各自决定如何下注，方便 Game.cpp 中统一接口
        virtual void decideBet() = 0; // = 0 声明纯虚函数，表示该函数是一个接口，必须由子类实现，同时使 Player 成为抽象类，不能直接实例化。。
};

std::mt19937 Player::gen(std::random_device{}()); // 使用硬件随机数生成器为随机数引擎提供高质量种子值

// =============================================
// 真人玩家子类：输出掷出点数并向玩家请求输入下注
// =============================================
class HumanPlayer : public Player {
    public:
        HumanPlayer(std::string name, int chips)
            : Player(std::move(name), chips) {}

        void decideBet() override {
            std::cout << name << "，你掷出了：" << dice <<"\n"; // 这里的 dice 由 rollDice() 生成，详见 Game.cpp line:23, 24

            int maxBet = std::min(5, chips); // 不能超过自身持有赌注
            while (true) {
                std::cout << "你的剩余赌注为 " << chips << "。" << std::endl;
                std::cout << "请下注（1–" << maxBet << "）：";
                std::cin >> bet;

                if (std::cin.fail()) { // 遇到非数字内容（如字母、符号等）
                    std::cin.clear();  // 清除 cin 的错误状态，使其恢复读取
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 将缓冲区内残留的非法内容全部丢弃，忽略尽可能多的字符（直到换行符为止），确保无论用户输入了多长的乱码都能清理干净
                    std::cout << "下注须为整数，请重新下注。\n";
                    continue;
                }
                if (bet >= 1 && bet <= maxBet) break; // 完成下注，写入变量 bet
                std::cout << "下注须在 1 到 " << maxBet << " 之间，请重新输入。\n"; // 若不符合 bet >= 1 && bet <= maxBet，则要求重新下注
            }
        }
};

// =============================================
// 电脑子类：不向外输出掷出点数，根据固定规则下注
// =============================================
class ComputerPlayer : public Player {
    public:
        explicit ComputerPlayer(int chips) : Player("电脑", chips) {}

        void decideBet() override {
            static std::uniform_int_distribution<int> fluctuationDist(0, 1);

            int base;
            if (dice <= 2) {
                base = 1;
            } else if (dice <= 4) {
                base = 2;
            } else {
                base = 4;
            }

            bet = base + fluctuationDist(gen); // 随机加 0 或 1，使行为更加难以预测
            bet = std::min(bet, 5);           // 不超过规则上限
            bet = std::min(bet, chips);       // 不超过自身筹码

            std::cout << "电脑已完成掷骰和下注。\n";
        }
};
