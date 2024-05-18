/*
业务要求：13位数，其中11位为整数，2位为小数，精度要求小数后5位；2次乘法；
1. 生成满足要求的随机数10个（11位整数+5位小数）
2. 做两次乘法，记录时间&精度
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

double generateRandomNumber(int integerDigits, int decimalDigits) {
    // 生成整数部分
    int integerPart = 0;
    for (int i = 0; i < integerDigits; ++i) {
        integerPart = integerPart * 10 + rand() % 10;
    }

    // 生成小数部分
    double decimalPart = 0.0;
    for (int i = 0; i < decimalDigits; ++i) {
        decimalPart += static_cast<double>(rand() % 10) / std::pow(10, i + 1);
    }

    // 组合整数和小数部分
    double result = integerPart + decimalPart;

    return std::abs(result); // 取绝对值确保非负数
}

void ckks_multiply_and_reliearize_business(Ciphertext &encrypted1, Ciphertext &encrypted2, Evaluator &evaluator, RelinKeys &relin_keys) {
    evaluator.multiply_inplace(encrypted1, encrypted2);
    // Relinearize the result
    evaluator.relinearize_inplace(encrypted1, relin_keys);
}

// 近似比较函数
bool approxEqual_business(double a, double b, double epsilon){
    // cout << "std::abs(a-b)" << std::abs(a-b) << endl;
    // cout << "epsilon" << epsilon << endl;
    return std::abs(a-b) > epsilon;
}

void ckks_business(){
    // 生成n个满足条件的随机数并输出
    int n = 10;
    for (int i = 0; i < n; ++i) {
        // 生成满足条件的随机数
        double randomNumber1 = generateRandomNumber(7, 6);
        cout << std::fixed << std::setprecision(6) << "randomNumber:" << randomNumber1 << endl;
        double randomNumber2 = generateRandomNumber(1, 2);
        cout << std::fixed << std::setprecision(6) << "randomNumber:" << randomNumber2 << endl;

        // 求真实值
        double real = randomNumber1 * randomNumber2   ;
        // double real = randomNumber1 ;
        cout << "real:" << real << endl;

        // 计算开始时间
        auto start_time = std::chrono::high_resolution_clock::now();

        // print_example_banner("Example: CKKS Basics");
        EncryptionParameters parms(scheme_type::ckks);

        // size_t poly_modulus_degree = 8192;
        size_t poly_modulus_degree = 32768;
        parms.set_poly_modulus_degree(poly_modulus_degree);
        // parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 60 }));
        parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 60 }));
        double scale = pow(2.0, 40);
        SEALContext context(parms);
        // print_parameters(context);
        // cout << endl;

        KeyGenerator keygen(context);
        auto secret_key = keygen.secret_key();
        PublicKey public_key;
        keygen.create_public_key(public_key);
        RelinKeys relin_keys;
        keygen.create_relin_keys(relin_keys);
        // GaloisKeys gal_keys;
        // keygen.create_galois_keys(gal_keys);
        Encryptor encryptor(context, public_key);
        Evaluator evaluator(context);
        Decryptor decryptor(context, secret_key);
        CKKSEncoder encoder(context);

        Plaintext plain1;
        encoder.encode(randomNumber1, scale, plain1);
        Ciphertext encrypted1;
        encryptor.encrypt(plain1, encrypted1);
        Plaintext plain2;
        encoder.encode(randomNumber2, scale, plain2);
        Ciphertext encrypted2;
        encryptor.encrypt(plain2, encrypted2);

        //  做两次乘法
        ckks_multiply_and_reliearize_business(encrypted1, encrypted2, evaluator, relin_keys);
        // ckks_multiply_and_reliearize_business(encrypted1, encrypted2, evaluator, relin_keys);
       
        // 解密
        Plaintext decrypted;
        decryptor.decrypt(encrypted1, decrypted);
        vector<double> res;
        encoder.decode(decrypted, res);

        // 计算结束时间
        auto end_time = std::chrono::high_resolution_clock::now();
        // 计算时间差
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        // 打印运行时间
        std::cout << "代码运行时间: " << duration.count() << " 毫秒" << std::endl;

        cout << std::fixed << std::setprecision(6) << "decrypted:" << res[0] << endl;


        double epsilon = 1.0;
        int flag = 0;
        for (int dec_prec = 0; dec_prec < 6; ++dec_prec){
            epsilon *= 0.1;
            if(approxEqual_business(res[0], real, epsilon)){
                cout << "精度为：" << dec_prec << endl;
                flag = 1;
                break;
            }
        }
        // cout << "flag:" << flag << endl;
        if(flag == 0){
            cout << "精度无损失" << endl;
        }

    }

    return;

}
