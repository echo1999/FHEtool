#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <bitset>
#include <cmath>
#include <limits>
#include "examples.h"
#include <chrono>


using namespace std;
using namespace seal;

std::vector<double> readGradientFromCSV() {
    // 调用函数并传入CSV文件路径
    // std::string filePath = "cnn_gradients.csv";
    // std::vector<double> gradientValues = readGradientFromCSV(filePath);
    std::vector<double> gradientValues;
    std::string filePath = "/echo-project/ckks/SEAL/native/mytest/cnn_gradients.csv";
    // 打开CSV文件
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "无法打开文件：" << filePath << std::endl;
    }

    std::string line;
    // 逐行读取CSV文件
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;

        // 使用逗号分割每行的数据
        bool insideQuotes = false;
        std::string currentToken;

        while (std::getline(iss, token, ','))
        {
            if (!insideQuotes)
            {
                if (token.empty())
                {
                    // 空字符串
                    tokens.push_back("");
                }
                else if (token.front() == '"')
                {
                    // 开始引号包围的数据
                    insideQuotes = true;
                    currentToken = token;
                }
                else
                {
                    // 普通数据，直接添加到tokens
                    tokens.push_back(token);
                }
            }
            else
            {
                // 在引号内，将内容累加到currentToken
                currentToken += "," + token;
                if (token.back() == '"')
                {
                    // 结束引号包围的数据
                    insideQuotes = false;
                    tokens.push_back(currentToken.substr(1, currentToken.length() - 2));
                    currentToken.clear();
                }
            }
        }

        // 假设Gradient列是第N列（从0开始计数）
        int gradientColumnIndex = 2; // 例如，如果是第三列，则设置为2
        if (tokens.size() > gradientColumnIndex)
        {
            try
            {
                // 将Gradient列的值转换为浮点数并存储在数组中
                double gradientValue = std::stod(tokens[gradientColumnIndex]);
                // cout << "typeid(gradientValue).name():" << typeid(gradientValue).name() << endl;
                // cout << "gradientValue:" << gradientValue << endl;
                gradientValues.push_back(gradientValue);
                // cout << "gradientsValue:" << gradientValue << endl;
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << "无法解析浮点数：" << tokens[gradientColumnIndex] << std::endl;
            }
        }
    }

    file.close();
    return gradientValues;
}

// 近似比较函数
bool approxEqual(double a, double b, double epsilon){
    // double sub = abs(a-b);
    // cout << "sub:" << sub << endl;
    return std::abs(a-b) > epsilon;
}
void testCKKSParameters(const int poly_mod, const vector<int>& coeff_mod_bit_sizes, const int precision){
    vector<double> gradientValues = readGradientFromCSV();
    // 初始化参数
    EncryptionParameters parms(scheme_type::ckks);
    size_t poly_modulus_degree = static_cast<size_t>(poly_mod);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, coeff_mod_bit_sizes));
    double scale = pow(2.0, precision);

    SEALContext context(parms);
    print_parameters(context);
    cout << endl;

    KeyGenerator keygen(context);
    auto secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    GaloisKeys gal_keys;
    keygen.create_galois_keys(gal_keys);
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);
    CKKSEncoder encoder(context);

    // 求梯度的真实平均值
    int len = gradientValues.size();
    double sum = 0;
    for (int i = 0; i < gradientValues.size(); i++)
    {
        sum = sum + gradientValues[i];
    }
    double avg = sum / len;
    cout.precision(12);
    cout << "real avg:" << avg << endl;

    Plaintext plain_zero;
    encoder.encode(0, scale, plain_zero);
    Ciphertext cipher_sum;
    encryptor.encrypt(plain_zero, cipher_sum);
    double len_rec = 1.0 / len;
    // int len_rec = 2;
    // 将梯度值加密后求和
    // 计算开始时间
    auto start_time = std::chrono::high_resolution_clock::now();
    int i = 0;
    while (i < len){
        // cout << i << ":" << endl;
        Plaintext plain;
        encoder.encode(gradientValues[i], scale, plain);
        Ciphertext cipher;
        encryptor.encrypt(plain, cipher);
        evaluator.add(cipher, cipher_sum, cipher_sum);
        i = i + 1;
    }
    // // 梯度值之和解密
    // Plaintext plain_sum;
    // decryptor.decrypt(cipher_sum, plain_sum);
    // vector<double> sum_res;
    // encoder.decode(plain_sum, sum_res);  
 
    // // 计算结束时间
    // auto end_time = std::chrono::high_resolution_clock::now();
    // // 计算时间差
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    // // 打印运行时间
    // std::cout << "求和运行时间: " << duration.count() << " 毫秒" << std::endl;
    // cout << "sum_res:" << sum_res[0] << endl; 

    // 梯度值之和求平均
    Plaintext plain_len_rec;
    encoder.encode(len_rec, scale, plain_len_rec);
    Ciphertext cipher_avg;
    evaluator.multiply_plain(cipher_sum, plain_len_rec, cipher_avg);
    // 解码解密
    Plaintext plain_res;
    decryptor.decrypt(cipher_avg, plain_res);
    vector<double> res;
    encoder.decode(plain_res, res);
    // 计算结束时间
    auto end_time = std::chrono::high_resolution_clock::now();
    // 计算时间差
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    // 打印运行时间
    std::cout << "代码运行时间: " << duration.count() << " 毫秒" << std::endl;

    // cout << "res.size():" << res.size() << endl;
    cout.precision(12);
    cout << "res:" << res[0] << endl;
    
    double epsilon = 1.0;
    for (int dec_prec = 0; dec_prec < scale; ++dec_prec){
        epsilon *= 0.1;
        if(approxEqual(res[0], avg, epsilon)){
            cout << "精度为：" << dec_prec << endl;
            break;
        }
    }

    // cout << "Precision = " << prec << endl;
    return;

}
void myckks(){
    for (const auto& parameters : {
        // std::make_tuple(8192, std::vector<int>{60, 40, 40, 60}, 40),
        std::make_tuple(8192, std::vector<int>{60, 21, 21, 60}, 40),
        // std::make_tuple(8192, std::vector<int>{40, 21, 21, 40}, 21),
        // std::make_tuple(8192, std::vector<int>{40, 21, 21, 40}, 40),
        // std::make_tuple(8192, std::vector<int>{30, 20, 30}, 40),
        // std::make_tuple(8192, std::vector<int>{20, 20, 20}, 38),
        // std::make_tuple(4096, std::vector<int>{40, 20, 40}, 40),
        // std::make_tuple(4096, std::vector<int>{30, 20, 30}, 40),
        // std::make_tuple(4096, std::vector<int>{20, 20, 20}, 38),
        // std::make_tuple(4096, std::vector<int>{19, 19, 19}, 35),
        // std::make_tuple(4096, std::vector<int>{18, 18, 18}, 33),
        // std::make_tuple(4096, std::vector<int>{30, 30, 30}, 25),
        // std::make_tuple(4096, std::vector<int>{25, 25, 25}, 20),
        // std::make_tuple(4096, std::vector<int>{18, 18, 18}, 16),
        // std::make_tuple(2048, std::vector<int>{18, 18, 18}, 16)
        // 添加更多参数组合...
    }) {
        int poly_mod = std::get<0>(parameters);
        vector<int> coeff_mod_bit_sizes = std::get<1>(parameters);
        int scale = std::get<2>(parameters);

        // cout << "Testing parameters: Poly Modulus = " << poly_mod << ", Coeff Mod Bit Sizes = ";
        // for (const auto& size : coeff_mod_bit_sizes) {
        //     cout << size << " ";
        // }
        // cout << ", Scale = " << scale << endl;

        testCKKSParameters(poly_mod, coeff_mod_bit_sizes, scale);
        // cout << ", Precision = " << prec << endl;
    }

    return;

}
