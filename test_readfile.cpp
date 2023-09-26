#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

int main() {
    std::ifstream file("../dataset/spmv-dataset/100w/delaunay_n19/delaunay_n19.mtx"); // 打开文件

    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf(); // 将整个文件内容读入缓冲区

        // 将缓冲区内容按行分割存入向量
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(buffer, line)) {
            lines.push_back(std::move(line));
        }
        // 打印向量中的每一行内容
        file.close(); // 关闭文件
    } else {
        std::cout << "无法打开文件" << std::endl;
    }
    return 0;
}
