#include <iostream>
#include <random>
#include <limits> // 处理非法输入时的缓冲区错误

// =============================================
// 工具函数：掷一颗六面骰子，返回 1–6
// =============================================
int rollDice() {
    // static 保证引擎只初始化一次，下一次使用时会继承上一次的初始化结果
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<int> dist(1, 6);
    return dist(gen);
}

// =============================================
// 计算机的下注策略：骰点 1–2 下注 1，3–4 下注 2–3，5–6 下注 4–5
// =============================================
int GetComputerBet(int dice, int chips) {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<int> fluctuationDist(0, 2); // 浮动范围 [0, 2]

    int bet;
    if (dice <= 2) {
        bet = 1;
    } else if (dice <= 4) {
        bet = 2;
    } else {
        bet = 4;
    }

    bet += fluctuationDist(gen);     // 随机加 0 或 1
    bet = std::min(bet, 5);         // 不超过规则上限
    bet = std::min(bet, chips);     // 不超过自身筹码
    return bet;
}

// =============================================
// 读取玩家下注，并进行合法性校验
// =============================================
int GetPlayerBet(int chips) {
    int bet;
    int maxBet = std::min(5, chips);

    while (true) {
        std::cout << "你的剩余赌注为" << chips << "。" << std::endl;
        std::cout << "请下注（1–" << maxBet << "）：";
        std::cin >> bet;

        // 处理非数字输入
        if (std::cin.fail()) { // 遇到非数字内容（如字母、符号等）
            std::cin.clear();  // 清除 cin 的错误状态，使其恢复读取
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 将缓冲区内残留的非法内容全部丢弃，忽略尽可能多的字符，确保无论用户输入了多长的乱码都能清理干净
            std::cout << "下注须为整数，请重新下注。\n";
            continue;
        }

        if (bet >= 1 && bet <= maxBet) break;
        std::cout << "下注须在 1 到 " << maxBet << " 之间，请重新下注。\n";
    }
    return bet;
}


// =============================================
// 主游戏逻辑：一局对局
// =============================================
void playRound(int &chipsPlayer, int &chipsComputer, int round) {
    std::cout << "\n===== 第 " << round << " 回合 =====\n";
    std::cout << "当前筹码 —— 玩家A: " << chipsPlayer << "  玩家B: " << chipsComputer << "\n";

    // 双方各自掷骰子
    int dicePlayer = rollDice();
    int diceComputer = rollDice();

    // 玩家看到自己的点数，据此决定下注
    std::cout << "\n你掷出了：" << dicePlayer << "\n";
    int betPlayer = GetPlayerBet(chipsPlayer);

    // 计算机在后台决定下注
    int betComputer = GetComputerBet(diceComputer, chipsComputer);
    std::cout << "电脑已完成掷骰和下注。\n";

    // 等待玩家按下回车
    std::cout << "\n按下回车键揭晓结果……";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 先清除上一次输入下注时回车留在缓冲区的换行符
    std::cin.get(); // 等待玩家按下新的回车

    // ---- 揭晓阶段 ----
    std::cout << "\n--- 揭晓 ---\n";
    std::cout << "你的点数：" << dicePlayer   << "，下注：" << betPlayer   << "\n";
    std::cout << "电脑点数：" << diceComputer << "，下注：" << betComputer << "\n";

    // 结算：点数高者赢得对方的赌注
    if (dicePlayer > diceComputer) {
        chipsPlayer   += betComputer;
        chipsComputer -= betComputer;
        std::cout << ">> 你获胜，赢得 " << betComputer << " 点筹码！\n";
    } else if (diceComputer > dicePlayer) {
        chipsComputer += betPlayer;
        chipsPlayer   -= betPlayer;
        std::cout << ">> 电脑获胜，赢得 " << betPlayer << " 点筹码！\n";
    } else {
        std::cout << ">> 平局，筹码不变。\n";
    }
}

// =============================================
// 主函数：初始化并循环进行对局
// =============================================
int main() {
    // 双方各持 10 点初始筹码
    int chipsA = 10;
    int chipsB = 10;
    int round  = 1;

    std::cout << "=== 对抗游戏开始 ===\n";
    std::cout << "双方各持 10 点筹码，先归零者失败。\n";

    // 循环进行，直到任意一方筹码耗尽
    while (chipsA > 0 && chipsB > 0) {
        playRound(chipsA, chipsB, round);
        ++round;

        // 双方筹码都还大于零，说明游戏未结束，暂停等待
        if (chipsA > 0 && chipsB > 0) {
            std::cout << "\n按下回车键进入下一回合……";
            std::cin.get();
        }
    }

    // 输出最终结果
    std::cout << "\n===== 游戏结束 =====\n";
    std::cout << "最终筹码 —— 玩家 A: " << chipsA << "  玩家 B: " << chipsB << "\n";

    if (chipsA <= 0) {
        std::cout << "玩家 B 胜利！\n";
    } else {
        std::cout << "玩家 A 胜利！\n";
    }

    return 0;
}