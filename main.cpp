#pragma once
#include "Server.cpp"

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
                startServer();
                break;
            case 0:
                return 0;
            default:
                std::cout << "无效选项，请重新输入。\n";
        }
    }
}