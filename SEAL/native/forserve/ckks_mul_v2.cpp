/*
1. 跑通流程
2. relinearize rescale
3. 把精度改为二进制
4. 参数设置为与BFV一致，记录运行时间、精度
*/

#include <bitset>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include "examples.h"

using namespace std;
using namespace seal;

void ckks_v2_multiply_and_reliearize(
    Ciphertext &encrypted1, Ciphertext &encrypted2, Evaluator &evaluator, RelinKeys &relin_keys)
{
    evaluator.multiply_inplace(encrypted1, encrypted2);
    // Relinearize the result
    evaluator.relinearize_inplace(encrypted1, relin_keys);
}

// void ckks_multiply_and_reliearize_rescale(Ciphertext &encrypted1, Ciphertext &encrypted2, Evaluator &evaluator,
// RelinKeys &relin_keys) {
//     evaluator.multiply_inplace(encrypted1, encrypted1);
//     // Relinearize the result
//     evaluator.relinearize_inplace(encrypted1, relin_keys);
//     cout << "    + Scale of encrypted1 before rescale: " << log2(encrypted1.scale()) << " bits" << endl;
//     cout << "    + Scale of encrypted2 before rescale: " << log2(encrypted2.scale()) << " bits" << endl;
//     evaluator.rescale_to_next_inplace(encrypted1);
//     // cout << "    + Scale of encrypted1 after rescale: " << log2(encrypted1.scale()) << " bits" << endl;
//     // evaluator.rescale_to_next_inplace(encrypted2);
// }

// 近似比较函数
bool approxEqual2_v2(double a, double b, double epsilon)
{
    // cout << "std::abs(a-b)" << std::abs(a-b) << endl;
    // cout << "epsilon" << epsilon << endl;
    return std::abs(a - b) > epsilon;
}

void ckks_mul_v2()
{
    // // 计算开始时间
    // auto start_time = std::chrono::high_resolution_clock::now();

    EncryptionParameters parms(scheme_type::ckks);

    // size_t poly_modulus_degree = 8192;
    size_t poly_modulus_degree = 32768;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(
        poly_modulus_degree, { 60, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 60 }));

    double scale = pow(2.0, 40);
    SEALContext context(parms);

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

    int max_multiplications = 19;
    int maxtimes = 100;
    double temp = 0.01;

    for (int j = 2; j <= maxtimes; j++)
    {
        int flag = 0;
        // cout << "x:" << j << endl;
        double num = j * temp;
        double real = num;
        cout << "x:" << num << endl;

        Plaintext plain;
        encoder.encode(num, scale, plain);
        Ciphertext encrypted_x;
        encryptor.encrypt(plain, encrypted_x);
        Ciphertext encrypted_res;
        encryptor.encrypt(plain, encrypted_res);

        for (int i = 1; i < max_multiplications; ++i)
        {
            ckks_v2_multiply_and_reliearize(encrypted_res, encrypted_x, evaluator, relin_keys);

            Plaintext decrypted_x;
            decryptor.decrypt(encrypted_res, decrypted_x);
            vector<double> res;
            encoder.decode(decrypted_x, res);
            cout.precision(12);


            real = real * num;
            // cout << "real:" << real << endl;
            double epsilon = 0.1;
            for (int dec_prec = 0; dec_prec < scale; ++dec_prec)
            {
                epsilon *= 0.1;
                if (approxEqual2_v2(res[0], real, epsilon)){
                    // cout << "精度为：" << dec_prec << endl;
                    if(dec_prec == 0){
                        cout << "Decryption failed after " << i << " multiplications." << endl;
                        flag = 1;
                        break;
                    }
                    break;
                }
            }
            if (flag == 1){
                break;
            }

            // // 计算结束时间
            // auto end_time = std::chrono::high_resolution_clock::now();
            // // 计算时间差
            // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            // // 打印运行时间
            // std::cout << "代码运行时间: " << duration.count() << " 毫秒" << std::endl;
        }
    }
    // cout << "Precision = " << prec << endl;
    return;
}
