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

double generateRandomNumber_bgv(int integerDigits, int decimalDigits) {
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

void bgv_multiply_and_reliearize_modswitch_business(Ciphertext &encrypted1, Ciphertext &encrypted2, Evaluator &evaluator, Decryptor &decryptor,RelinKeys &relin_keys) {
    evaluator.multiply_inplace(encrypted1, encrypted2);
    evaluator.relinearize_inplace(encrypted1, relin_keys);
    evaluator.mod_switch_to_next_inplace(encrypted1);
    evaluator.mod_switch_to_next_inplace(encrypted2);
    int noise_budget = decryptor.invariant_noise_budget(encrypted1);
    cout << "    + noise budget after mod switch: " << noise_budget << " bits" << endl;
}

uint64_t bgv_hexStringToUint64(const std::string &hexString)
{
    std::stringstream ss;
    ss << std::hex << hexString;

    uint64_t result;
    ss >> result;

    return result;
}

void bgv_business(){
    // 生成n个满足条件的随机数并输出
    int n = 10;
    for (int i = 0; i < n; ++i) {
        // 生成满足条件的随机数
        double randomNumber1 = generateRandomNumber_bgv(9, 2);
        cout << std::fixed << std::setprecision(3) << "randomNumber:" << randomNumber1 << endl;
        double randomNumber2 = generateRandomNumber_bgv(1, 0);
        cout << std::fixed << std::setprecision(3) << "randomNumber:" << randomNumber2 << endl;

        // 求真实值
        double real = randomNumber1 * randomNumber2 * randomNumber2   ;
        // double real = randomNumber1 ;
        cout << "real:" << real << endl;

        // 计算开始时间
        auto start_time = std::chrono::high_resolution_clock::now();

        EncryptionParameters parms(scheme_type::bfv);
        
        // size_t poly_modulus_degree = 8196;
        size_t poly_modulus_degree = 16384;
        
        parms.set_poly_modulus_degree(poly_modulus_degree);
        parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
        // parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 40, 60 }));
        parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 60));

        SEALContext context(parms);
        print_line(__LINE__);
        cout << "Set encryption parameters and print" << endl;
        print_parameters(context);

        KeyGenerator keygen(context);
        auto secret_key = keygen.secret_key();
        PublicKey public_key;
        keygen.create_public_key(public_key);
        RelinKeys relin_keys;
        keygen.create_relin_keys(relin_keys);
        Encryptor encryptor(context, public_key);
        Evaluator evaluator(context);
        Decryptor decryptor(context, secret_key);
        // CKKSEncoder encoder(context);

        uint64_t x = randomNumber1 * 100;
        uint64_t y = randomNumber2 * 100;
        Plaintext plain1(uint64_to_hex_string(x));
        Ciphertext encrypted1;
        encryptor.encrypt(plain1, encrypted1);
        Plaintext plain2(uint64_to_hex_string(y));
        Ciphertext encrypted2;
        encryptor.encrypt(plain2, encrypted2);

        //  做两次乘法
        bgv_multiply_and_reliearize_modswitch_business(encrypted1, encrypted2, evaluator, decryptor, relin_keys);
        bgv_multiply_and_reliearize_modswitch_business(encrypted1, encrypted2, evaluator, decryptor, relin_keys);
       
        // 解密
        Plaintext decrypted;
        decryptor.decrypt(encrypted1, decrypted);
        uint64_t decrypted_unit64 = bgv_hexStringToUint64(decrypted.to_string());
        cout << "decrypted_unit64:" <<decrypted_unit64  << endl;
        double decrypted_double = decrypted_unit64/1000000.0;
        cout << "decrypted_double:" <<std::setprecision(3) << decrypted_double  << endl;

        // 计算结束时间
        auto end_time = std::chrono::high_resolution_clock::now();
        // 计算时间差
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        // 打印运行时间
        std::cout << "代码运行时间: " << duration.count() << " 毫秒" << std::endl;

    }

    return;

}
