#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    system("chcp 65001 > nul");
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    // 1. 统一定义并初始化，消除未初始化警告
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    std::cout << "正在尝试连接 Java 服务端..." << std::endl;

    // 2. 使用 reinterpret_cast 消除 C 样式转换警告
    if (connect(sock, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "连接失败！请确认 Java 程序是否正在运行。" << std::endl;
        return 1;
    }

    std::cout << "连接成功！开始接收数据：" << std::endl;

    char buffer[1024];
    while (true) {
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::string data(buffer);
            if (data.find("END") != std::string::npos) break;
            std::cout << "收到: " << data << std::endl;
        }
    }

    closesocket(sock);
    WSACleanup();
    std::cout << "数据接收完毕，程序结束。" << std::endl;
    return 0;
}