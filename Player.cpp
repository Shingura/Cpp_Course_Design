#pragma once
#include <iostream>
#include <string>
#include <random>
#include <limits>
#include <algorithm>

// =============================================
// 玩家基类：可能为真人玩家或电脑
// =============================================
class Player {
    protected:
        // static 保证所有 Player 及其子类共享同一个引擎
        static std::mt19937 gen;

    public:
        std::string name;
        int chips;  // 剩余赌注
        int dice;   // 本轮骰点
        int bet;    // 本轮下注

        Player(std::string  name, int chips)
            : name(std::move(name)), chips(chips), dice(0), bet(0) {}

        virtual ~Player() = default; // 默认析构函数

        // 掷骰子，结果存入 dice
        void rollDice() {
            // static 保证引擎只初始化一次，下一次使用时会继承上一次的初始化结果
            static std::uniform_int_distribution<int> dist(1, 6);
            dice = dist(gen);
        }

        // 判定玩家是否获胜
        bool isAlive() const {
            return chips > 0;
        }

        // 纯虚函数：子类（真人和计算机）各自决定如何下注
        virtual void decideBet() = 0;
};

// =============================================
// 真人玩家子类：输出掷出点数并向玩家请求输入下注
// =============================================
class HumanPlayer : public Player {
    public:
        HumanPlayer(std::string name, int chips)
            : Player(std::move(name), chips) {}

        void decideBet() override {
            std::cout << name << "，你掷出了：" << dice <<"\n";

            int maxBet = std::min(5, chips);
            while (true) {
                std::cout << "你的剩余赌注为 " << chips << "。" << std::endl;
                std::cout << "请下注（1–" << maxBet << "）：";
                std::cin >> bet;

                if (std::cin.fail()) { // 遇到非数字内容（如字母、符号等）
                    std::cin.clear();  // 清除 cin 的错误状态，使其恢复读取
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 将缓冲区内残留的非法内容全部丢弃，忽略尽可能多的字符，确保无论用户输入了多长的乱码都能清理干净
                    std::cout << "下注须为整数，请重新下注。\n";
                    continue;
                }
                if (bet >= 1 && bet <= maxBet) break;
                std::cout << "下注须在 1 到 " << maxBet << " 之间，请重新输入。\n";
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
