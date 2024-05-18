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
    // cout << "    + noise budget after mod switch: " << noise_budget << " bits" << endl;
}

uint64_t bgv_hexStringToUint64(const std::string &hexString)
{
    std::stringstream ss;
    ss << std::hex << hexString;

    uint64_t result;
    ss >> result;

    return result;
}
int compare_floats(float a, float b) {
    int precision = 1;
    float diff = fabs(a - b);
    while (diff < 1) {
        precision *= 10;
        diff *= 10;
    }
    int result = 1;
    while (diff >= 1) {
        if ((int)diff % 10 != 0) {
            return result;
        }
        diff /= 10;
        precision /= 10;
        result++;
    }
    return result;
}
void bgv_serve(int data_integer, int data_decimal, int mul_integer, int mul_decimal, int mul_times){
    // 生成n个满足条件的随机数
    int n = 10;
    int time_sum = 0;
    int prec_sum = 0;
    for (int i = 0; i < n; ++i) {
        // 生成满足条件的随机数
        double data_randomNumber = generateRandomNumber_bgv(data_integer, data_decimal);
        // cout << std::fixed << std::setprecision(data_decimal+1) << "randomNumber:" << data_randomNumber << endl;
        double mul_randomNumber = generateRandomNumber_bgv(mul_integer, mul_decimal);
        // cout << std::fixed << std::setprecision(mul_decimal+1) << "randomNumber:" << mul_randomNumber << endl;

        // 求真实值
        double real = data_randomNumber;
        for(int z = 0; z < mul_times; ++z){
            real = real * mul_randomNumber;
        }
        // cout << "real:" << real << endl;


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
        // print_line(__LINE__);
        // cout << "Set encryption parameters and print" << endl;
        // print_parameters(context);

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

        // 扩充数值
        int data_magnificant = 0;
        int mul_magnificant = 0;
        for(int k = 0; k < data_magnificant; ++k){
            data_magnificant = data_magnificant * 10;
        }
        for(int k = 0; k < mul_magnificant; ++k){
            mul_magnificant = mul_magnificant * 10;
        }
        uint64_t data = data_randomNumber * data_magnificant;
        uint64_t mul = mul_randomNumber * mul_magnificant;
        Plaintext data_plain(uint64_to_hex_string(data));
        Ciphertext data_encrypted;
        encryptor.encrypt(data_plain, data_encrypted);
        Plaintext mul_plain(uint64_to_hex_string(mul));
        Ciphertext mul_encrypted;
        encryptor.encrypt(mul_plain, mul_encrypted);

        //  做mul_times次乘法
        for(int j = 0; j < mul_times; ++j){
            bgv_multiply_and_reliearize_modswitch_business(data_encrypted, mul_encrypted, evaluator, decryptor, relin_keys);
        }
       
        // 解密
        Plaintext decrypted;
        decryptor.decrypt(data_encrypted, decrypted);
        uint64_t decrypted_unit64 = bgv_hexStringToUint64(decrypted.to_string());
        // cout << "decrypted_unit64:" <<decrypted_unit64  << endl;
        double decrypted_double = decrypted_unit64/data_magnificant;
        decrypted_double = decrypted_double/mul_magnificant;

        // 计算结束时间
        auto end_time = std::chrono::high_resolution_clock::now();
        // 计算时间差
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        // 打印运行时间
        // std::cout << "代码运行时间: " << duration.count() << " 毫秒" << std::endl;
        time_sum = time_sum + duration.count();

        // 计算精度
        int prec = compare_floats(real, decrypted_double);
        cout << "decrypted_double:" << decrypted_double  << endl;
        cout << "real:" << real  << endl;
        prec_sum = prec_sum + prec;
    }
    double time_avg = time_sum / n;
    cout << "time_avg:" << time_avg << endl;
    double prec_avg = prec_sum / n;
    cout << "prec_avg:" << prec_avg << endl;
    return;

}
