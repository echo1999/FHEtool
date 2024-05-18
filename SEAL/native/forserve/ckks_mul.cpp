/*
1. 跑通流程
2. relinearize rescale
3. 把精度改为二进制
4. 参数设置为与BFV一致，记录运行时间、精度
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

void ckks_multiply(Ciphertext &encrypted1, Ciphertext &encrypted2, Evaluator &evaluator) {
    evaluator.multiply_inplace(encrypted1, encrypted2);
}

void ckks_multiply_and_reliearize(Ciphertext &encrypted1, Ciphertext &encrypted2, Evaluator &evaluator, RelinKeys &relin_keys) {
    evaluator.multiply_inplace(encrypted1, encrypted2);
    // Relinearize the result
    evaluator.relinearize_inplace(encrypted1, relin_keys);
}

// 近似比较函数
bool approxEqual2(double a, double b, double epsilon){
    // cout << "std::abs(a-b)" << std::abs(a-b) << endl;
    // cout << "epsilon" << epsilon << endl;
    return std::abs(a-b) > epsilon;
}

void ckks_mul(){
    // 计算开始时间
    auto start_time = std::chrono::high_resolution_clock::now();

    print_example_banner("Example: CKKS Basics");
    EncryptionParameters parms(scheme_type::ckks);

    // size_t poly_modulus_degree = 8192;
    size_t poly_modulus_degree = 32768;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 60 }));
    
    double scale = pow(2.0, 40);
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

    int max_multiplications = 20;


    Plaintext plain;
    encoder.encode(103, scale, plain);
    Ciphertext encrypted_x;
    encryptor.encrypt(plain, encrypted_x);
    Ciphertext encrypted_res;
    encryptor.encrypt(plain, encrypted_res);

    double num = 103.0;
    double real = num;
    for (int i = 1; i < max_multiplications; ++i) {
        cout << "i:" << i << endl;
        // evaluator.multiply_inplace(encrypted_res, encrypted_x);
        // ckks_multiply(encrypted_res, encrypted_x, evaluator);
        ckks_multiply_and_reliearize(encrypted_res, encrypted_x, evaluator, relin_keys);
        // ckks_multiply_and_reliearize_rescale(encrypted_res, encrypted_x, evaluator, relin_keys);

        Plaintext decrypted_x;
        decryptor.decrypt(encrypted_res, decrypted_x);
        vector<double> res;
        encoder.decode(decrypted_x, res);
        cout.precision(12);
        cout << "res:" << res[0] << endl;

        //  求真实值
        // real = real * 2.0;
        
        real = real * num;
        cout << "real:" << real << endl;
        double epsilon = 1.0;
        for (int dec_prec = 0; dec_prec < scale; ++dec_prec){
            epsilon *= 0.1;
            if(approxEqual2(res[0], real, epsilon)){
                cout << "精度为：" << dec_prec << endl;
                break;
            }
        }

        // 计算结束时间
        auto end_time = std::chrono::high_resolution_clock::now();
        // 计算时间差
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        // 打印运行时间
        std::cout << "代码运行时间: " << duration.count() << " 毫秒" << std::endl;
    }


    // cout << "Precision = " << prec << endl;
    return;

}
