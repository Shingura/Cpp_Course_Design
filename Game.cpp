#include "Player.cpp"
#include <iostream>
#include <limits>

// =============================================
// Game 类持有双方玩家的指针，无需了解玩家具体类型，方便兼容 PvE 和 PvP
// =============================================
class Game {
    Player* a;
    Player* b;
    int round; // 记录游戏当前轮数

public:
    Game(Player* a, Player* b) : a(a), b(b), round(1) {}

    void playRound() {
        std::cout << "\n======== 第 " << round << " 回合 ========\n";
        std::cout << "—— 当前筹码 ——" << std::endl;
        std::cout << a->name << " 拥有 " << a->chips << " 点筹码" << std::endl;
        std::cout << b->name << " 拥有 " << b->chips << " 点筹码" << std::endl;
        std::cout << std::endl;
        // 双方掷骰并决定下注
        a->rollDice(); a->decideBet();
        b->rollDice(); b->decideBet();

        // 揭晓
        std::cout << "\n按下回车键揭晓结果……";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();

        std::cout << "\n--- 揭晓 ---\n";
        std::cout << a->name << " 的点数：" << a->dice << "，下注：" << a->bet << "\n";
        std::cout << b->name << " 的点数：" << b->dice << "，下注：" << b->bet << "\n";

        // 结算
        if (a->dice > b->dice) {
            a->chips += b->bet;
            b->chips -= b->bet;
            std::cout << ">> " << a->name << " 获胜，赢得 " << b->bet << " 点筹码！\n";
        } else if (b->dice > a->dice) {
            b->chips += a->bet;
            a->chips -= a->bet;
            std::cout << ">> " << b->name << " 获胜，赢得 " << a->bet << " 点筹码！\n";
        } else {
            std::cout << ">> 平局，双方筹码不变。";
        }

        ++round;
    }

    void run() {
        std::cout << "=== 对抗游戏开始 ===\n";
        std::cout << "双方各持 " << a->chips << " 点筹码，先归零者失败。\n";

        while (a->isAlive() && b->isAlive()) {
            playRound();

            if (a->isAlive() && b->isAlive()) {
                std::cout << "\n按下回车键进入下一回合……";
                std::cin.get();
            }
        }

        std::cout << "\n===== 游戏结束 =====\n";
        std::cout << a->name << " 拥有 " << a->chips << " 点筹码" << std::endl;
        std::cout << b->name << " 拥有 " << b->chips << " 点筹码" << std::endl;

        if (!a->isAlive()) {
            std::cout << b->name << " 获胜！\n";
        } else {
            std::cout << a->name << " 获胜！\n";
        }
    }
};

std::mt19937 Player::gen(std::random_device{}());
