#pragma once
#include "Game.cpp"
#include "httplib.h"
#include <iostream>
#include <fstream>
#include <mutex>
#include <string>

// 用结构体来存储游戏状态
struct GameState {
    std::string nameP1;
    std::string nameP2;
    bool p1Ready = false;
    bool p2Ready = false;
    bool bothReady = false;
};

GameState gameState;
std::mutex stateMutex; // 通过进程锁防止两名玩家同时写入状态产生冲突

// 从根目录下读取 index.html 文件

std::string getIndexHtml() {
    std::ifstream file("index.html");
    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

void startServer() {
    httplib::Server server;

    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(getIndexHtml(), "text/html; charset=utf-8");
    });

    server.Post("/join", [](const httplib::Request& req, httplib::Response& res) {
        auto body = req.body;
        int slot = (body.find("\"slot\":1") != std::string::npos) ? 1 : 2;
        auto nameStart = body.find("\"name\":\"") + 8;
        auto nameEnd = body.find("\"", nameStart);
        std::string name = body.substr(nameStart, nameEnd - nameStart);

        std::lock_guard<std::mutex> lock(stateMutex);

        std::string message;
        if (slot == 1) {
            if (gameState.p1Ready) {
                message = "1P 已被占用！";
            } else {
                gameState.nameP1 = name;
                gameState.p1Ready = true;
                message = "你已加入 1P，等待 2P 就绪……";
            }
        } else {
            if (gameState.p2Ready) {
                message = "2P 已被占用！";
            } else {
                gameState.nameP2 = name;
                gameState.p2Ready = true;
                message = "你已加入 2P，等待 1P 就绪……";
            }
        }

        res.set_content("{\"message\":\"" + message + "\"}", "application/json");
    });

    std::cout << "服务器已启动，请在浏览器中打开 http://localhost:8080\n";
    server.listen("0.0.0.0", 8080);
}

