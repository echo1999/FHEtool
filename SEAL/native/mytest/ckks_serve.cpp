/*
输入参数：数据整数位数data_integer、数据小数位数data_decimal、乘数整数位数mul_integer、数据小数位数mul_decimal、乘法次数mul_times
输出：使用100组符合要求的随机数据计算的平均精度、运算时间
*/
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

// double ckks_serve_generateRandomNumber(int integerDigits, int decimalDigits) {
//     // 生成整数部分
//     int integerPart = 0;
//     for (int i = 0; i < integerDigits; ++i) {
//         integerPart = integerPart * 10 + rand() % 10;
//     }

//     // 生成小数部分
//     double decimalPart = 0.0;
//     for (int i = 0; i < decimalDigits; ++i) {
//         decimalPart += static_cast<double>(rand() % 10) / std::pow(10, i + 1);
//     }

//     // 组合整数和小数部分
//     double result = integerPart + decimalPart;

//     return std::abs(result); // 取绝对值确保非负数
// }

// 读取数组和参数
int readArraysAndParamsFromFile_ckks(const std::string& filename,
                                double*& data_randomNumber,
                                double*& mul_randomNumber,
                                double*& real,
                                int& data_integer,
                                int& data_decimal,
                                int& mul_integer,
                                int& mul_decimal,
                                int& mul_times) {
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        std::string line;
        int n = 0; // 数组长度

        // 读取数组长度
        if (std::getline(inFile, line)) {
            if (line.find("random_length:") != std::string::npos) {
                n = std::stoi(line.substr(line.find(":") + 1));
            }else{
                cout << "没有找到random_length参数" << endl;
            }
        } else {
            std::cerr << "Error: Unable to read array length from file " << filename << std::endl;
            return -1;
        }

        // 创建相应大小的数组
        data_randomNumber = new double[n];
        mul_randomNumber = new double[n];
        real = new double[n];

        // 读取参数
        while (std::getline(inFile, line)) {
            if (line.find("data_integer:") != std::string::npos) {
                data_integer = std::stoi(line.substr(line.find(":") + 1));
            } else if (line.find("data_decimal:") != std::string::npos) {
                data_decimal = std::stoi(line.substr(line.find(":") + 1));
            } else if (line.find("mul_integer:") != std::string::npos) {
                mul_integer = std::stoi(line.substr(line.find(":") + 1));
            } else if (line.find("mul_decimal:") != std::string::npos) {
                mul_decimal = std::stoi(line.substr(line.find(":") + 1));
            } else if (line.find("mul_times:") != std::string::npos) {
                mul_times = std::stoi(line.substr(line.find(":") + 1));
            } else if (line.find("data_randomNumber:") != std::string::npos) {
                // 读取data_randomNumber数组元素
                for (int i = 0; i < n; ++i) {
                    std::getline(inFile, line);
                    data_randomNumber[i] = std::stod(line);
                }
            } else if (line.find("mul_randomNumber:") != std::string::npos) {
                // 读取mul_randomNumber数组元素
                for (int i = 0; i < n; ++i) {
                    std::getline(inFile, line);
                    mul_randomNumber[i] = std::stod(line);
                }
            } else if (line.find("real:") != std::string::npos) {
                // 读取real数组元素
                for (int i = 0; i < n; ++i) {
                    std::getline(inFile, line);
                    real[i] = std::stod(line);
                }
            }
        }

        inFile.close();
        std::cout << "Arrays and parameters have been read from " << filename << std::endl;

        // 返回数组长度
        return n;

    } else {
        std::cerr << "Unable to open file " << filename << " for reading!" << std::endl;
        return -1;
    }
}

void ckks_serve_multiply_and_reliearize(Ciphertext &encrypted1, Ciphertext &encrypted2, Evaluator &evaluator, RelinKeys &relin_keys) {
    evaluator.multiply_inplace(encrypted1, encrypted2);
    // Relinearize the result
    evaluator.relinearize_inplace(encrypted1, relin_keys);
}

uint64_t ckks_serve_hexStringToUint64(const std::string &hexString)
{
    std::stringstream ss;
    ss << std::hex << hexString;

    uint64_t result;
    ss >> result;

    return result;
}
const double epsilon = 1e-9; // 设置一个很小的阈值
int ckks_compareDoublePrecision(double num1, double num2, int prec_max) {
    // 获取两个数字的整数部分
    long long int_part1 = static_cast<long long>(num1);
    long long int_part2 = static_cast<long long>(num2);

    // 获取两个数字的小数部分的字符串表示
    std::ostringstream oss1, oss2;
    oss1 << std::fixed << std::setprecision(prec_max) << (num1 - static_cast<double>(int_part1));
    oss2 << std::fixed << std::setprecision(prec_max) << (num2 - static_cast<double>(int_part2));

    std::string frac_str1 = oss1.str().substr(2); // 去掉小数点前的 "0."
    std::string frac_str2 = oss2.str().substr(2);

    // 比较整数部分
    if (int_part1 != int_part2) {
        return 0; // 整数部分不同，返回0
    }

    // 比较小数部分
    for (int i = 0; i < prec_max; ++i) {
        if (frac_str1[i] != frac_str2[i]) {
            return i + 1; // 从第i+1位开始不同
        }
    }

    return prec_max; // 所有精度都相同，返回prec_max
}

void ckks_mul(size_t poly_modulus_degree, int scale_num, int n, int data_decimal, int mul_decimal, int mul_times, double data_randomNumber[], double mul_randomNumber[], double real[]){
    // 输出加密参数
    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    double scale = pow(2.0, scale_num);
    SEALContext context(parms);
    print_parameters(context);
    cout << "scale:" << "2^" << scale_num  << endl;

    int time_sum = 0;
    int prec_sum = 0;
    for(int i = 0; i < n; ++i){
        // cout << std::fixed << std::setprecision(data_decimal+1) << "data_randomNumber:" << data_randomNumber[i] << endl;
        // cout << std::fixed << std::setprecision(mul_decimal+1) << "mul_randomNumber:" << mul_randomNumber[i] << endl;
        // 计算开始时间
        auto start_time = std::chrono::high_resolution_clock::now();
        
        EncryptionParameters parms(scheme_type::ckks);
        parms.set_poly_modulus_degree(poly_modulus_degree);
        parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
        double scale = pow(2.0, 40);
        SEALContext context(parms);

        KeyGenerator keygen(context);
        auto secret_key = keygen.secret_key();
        PublicKey public_key;
        keygen.create_public_key(public_key);
        RelinKeys relin_keys;
        keygen.create_relin_keys(relin_keys);
        Encryptor encryptor(context, public_key);
        Evaluator evaluator(context);
        Decryptor decryptor(context, secret_key);
        CKKSEncoder encoder(context);
        
        Plaintext data_plain;
        Ciphertext data_encrypted;
        Plaintext mul_plain;
        Ciphertext mul_encrypted;

        try {
        encoder.encode(data_randomNumber[i], scale, data_plain);
        encoder.encode(mul_randomNumber[i], scale, mul_plain);
        } catch(const std::exception& e){
            // cout << "error" << endl;
            cout << "time_avg:" << 0 << "ms" << endl;
            cout << "prec_avg:" << 0 << endl;
            return;
        }

        encryptor.encrypt(data_plain, data_encrypted);
        encryptor.encrypt(mul_plain, mul_encrypted);

        //  做mul_times次乘法
        for(int j = 0; j < mul_times; ++j){
            // cout << "j:" << j << endl;
            try{
            ckks_serve_multiply_and_reliearize(data_encrypted, mul_encrypted, evaluator, relin_keys);
            }catch(const std::exception& e){
                // cout << "error" << endl;
                cout << "time_avg:" << 0 << "ms" << endl;
                cout << "prec_avg:" << 0 << endl;
                return;
            }
        }

        // 解密
        Plaintext decrypted;
        decryptor.decrypt(data_encrypted, decrypted);
        vector<double> res;
        encoder.decode(decrypted, res);

        // 计算结束时间
        auto end_time = std::chrono::high_resolution_clock::now();
        // 计算时间差
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        // 打印运行时间
        // std::cout << "代码运行时间: " << duration.count() << " 毫秒" << std::endl;
        time_sum = time_sum + duration.count();

        // 计算精度
        int prec_max = 0;
        if(data_decimal > mul_decimal){
            prec_max = data_decimal;
        }else{
            prec_max = mul_decimal;
        }
        // cout << "prec_max:" << prec_max <<endl;
        int prec = ckks_compareDoublePrecision(real[i], res[0], prec_max);
        // cout << "prec:" << prec << endl;
        // cout << std::fixed << std::setprecision(data_decimal+1)<< "res[0]:" << res[0]  << endl;
        // cout << std::fixed << std::setprecision(data_decimal+1)<< "real:" << real[i]  << endl;
        prec_sum = prec_sum + prec;
    }
    double time_avg = time_sum / double(n);
    cout << "time_avg:" << time_avg << "ms" << endl;
    double prec_avg = prec_sum / double(n);
    cout << "prec_avg:" << prec_avg << endl;
}

// void ckks_serve(int data_integer, int data_decimal, int mul_integer, int mul_decimal, int mul_times){
void ckks_serve(){
    // // 生成n个满足条件的随机数
    // int n = 10;
    // double data_randomNumber[n] = {0};
    // double mul_randomNumber[n] = {0};
    // double real[n] = {0};
    // for (int i = 0; i < n; ++i) {
    //     // 生成满足条件的随机数
    //     data_randomNumber[i] = ckks_serve_generateRandomNumber(data_integer, data_decimal);
    //     // cout << std::fixed << std::setprecision(data_decimal+1) << "randomNumber:" << data_randomNumber << endl;
    //     mul_randomNumber[i] = ckks_serve_generateRandomNumber(mul_integer, mul_decimal);
    //     // cout << std::fixed << std::setprecision(mul_decimal+1) << "randomNumber:" << mul_randomNumber << endl;

    //     // 求真实值
    //     real[i] = data_randomNumber[i];
    //     for(int z = 0; z < mul_times; ++z){
    //         real[i] = real[i] * mul_randomNumber[i];
    //     }
    //     // cout << "real:" << real << endl;
    // }
    double* data_randomNumber;
    double* mul_randomNumber;
    double* real;
    int data_integer, data_decimal, mul_integer, mul_decimal, mul_times;

    int n = readArraysAndParamsFromFile_ckks("/echo-project/ckks/data.txt", data_randomNumber, mul_randomNumber, real, data_integer, data_decimal, mul_integer, mul_decimal, mul_times);

    ckks_mul(4096, 20, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    ckks_mul(8192, 40, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    ckks_mul(16384, 40, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    ckks_mul(16384, 60, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    ckks_mul(32768, 40, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    ckks_mul(32768, 60, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    return;
}
