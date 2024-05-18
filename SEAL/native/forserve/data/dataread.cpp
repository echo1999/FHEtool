#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

std::vector<double> readGradientFromCSV(const std::string& filePath) {
    std::vector<double> gradientValues;

    // 打开CSV文件
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "无法打开文件：" << filePath << std::endl;
        return gradientValues;
    }

    std::string line;
    // 逐行读取CSV文件
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;

        // 使用逗号分割每行的数据
        bool insideQuotes = false;
        std::string currentToken;

        while (std::getline(iss, token, ',')) {
            if (!insideQuotes) {
                if (token.empty()) {
                    // 空字符串
                    tokens.push_back("");
                } else if (token.front() == '"') {
                    // 开始引号包围的数据
                    insideQuotes = true;
                    currentToken = token;
                } else {
                    // 普通数据，直接添加到tokens
                    tokens.push_back(token);
                }
            } else {
                // 在引号内，将内容累加到currentToken
                currentToken += "," + token;
                if (token.back() == '"') {
                    // 结束引号包围的数据
                    insideQuotes = false;
                    tokens.push_back(currentToken.substr(1, currentToken.length() - 2));
                    currentToken.clear();
                }
            }
        }

        // 假设Gradient列是第N列（从0开始计数）
        int gradientColumnIndex = 2; // 例如，如果是第三列，则设置为2
        if (tokens.size() > gradientColumnIndex) {
            try {
                // 将Gradient列的值转换为浮点数并存储在数组中
                double gradientValue = std::stod(tokens[gradientColumnIndex]);
                gradientValues.push_back(gradientValue);
            } catch (const std::invalid_argument& e) {
                std::cerr << "无法解析浮点数：" << tokens[gradientColumnIndex] << std::endl;
            }
        }
    }

    file.close();

    return gradientValues;
}

int main() {
    // 调用函数并传入CSV文件路径
    std::string filePath = "cnn_gradients.csv";
    std::vector<double> gradientValues = readGradientFromCSV(filePath);

    // 打印提取的Gradient值
    for (double gradient : gradientValues) {
        std::cout << gradient << " ";
    }
    std::cout << std::endl;

    return 0;
}


