#include <iostream>
#include <random>
#include <limits>
#include <string>

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
        HumanPlayer(const std::string& name, int chips)
            : Player(name, chips) {}

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

// 清除整个屏幕，并将光标移回左上角，从而实现「刷新」的效果
void clearScreen() {
    std::cout << "\033[2J\033[H";
}

void showRules() {
    std::cout << "\n=== 游戏规则 ===\n";
    std::cout << "每回合双方各掷一次骰子，根据点数决定下注（1–5 点）。\n";
    std::cout << "点数高者赢得对方的赌注；平局时双方筹码均不改变。\n";
    std::cout << "筹码归零者失败。\n";
    std::cout << "\n按下回车键回到主菜单……";
    std::cin.get();
}

void showMenu() {
    clearScreen();
    std::cout << "\n=== 对抗游戏 ===\n";
    std::cout << "1. 规则介绍\n";
    std::cout << "2. 人机对抗\n";
    std::cout << "3. 真人对抗\n";
    std::cout << "0. 退出\n";
    std::cout << "请选择：";
}

int main() {
    int choice;

    while (true) {
        showMenu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "无效选项，请重新输入。\n";
            continue;
        }

        // 无论选了哪个选项，都先清掉换行符，避免影响后续输入昵称等操作
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                clearScreen();
                showRules();
                break;
            case 2: {
                clearScreen();
                std::string NickName;
                std::cout << "请输入你的昵称：";
                std::getline(std::cin, NickName); // std::getline 会自动消费掉行末的换行符，因此无需用 cin.ignore 清除缓冲区
                clearScreen();
                HumanPlayer human(NickName, 10);
                ComputerPlayer computer(10);
                Game game(&human, &computer);
                game.run();
                break;
            }
            case 3:
                // 真人对抗暂未实现，留作日后扩展
                std::cout << "该模式尚未开放，敬请期待。\n";
                break;
            case 0:
                return 0;
            default:
                std::cout << "无效选项，请重新输入。\n";
        }
    }
}