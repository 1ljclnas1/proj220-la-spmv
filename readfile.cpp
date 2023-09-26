#include <iostream>
#include <fstream>
#include <string>
#include <vector>
int main() {
    std::ifstream file("../dataset/spmv-dataset/100w/delaunay_n19/delaunay_n19.mtx"); // 打开文件

    if (file.is_open()) {
        std::string line;
        std::vector<std::string> lines;
        while (std::getline(file, line)) { // 逐行读取文件内容
            lines.push_back(std::move(line));
        }

        file.close(); // 关闭文件
    } else {
        std::cout << "无法打开文件" << std::endl;
    }

    return 0;
}
