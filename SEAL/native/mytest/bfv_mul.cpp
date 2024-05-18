// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
/*
1. 计算x次乘法，直到噪声预算降为0
2. 记录运行时间
3. 找到reliearize和mod switch后噪声消耗没有减少的原因
*/
#include "examples.h"

using namespace std;
using namespace seal;

void multiply(Ciphertext &encrypted1, Ciphertext &encrypted2, Evaluator &evaluator, Decryptor &decryptor) {
    evaluator.multiply_inplace(encrypted1, encrypted2);
    cout << "    + noise budget: " << decryptor.invariant_noise_budget(encrypted1) << " bits"
    << endl;
    
    // Perform multiplication
    // cout << "    + size of encrypted before mul: " << encrypted.size() << endl;
    // evaluator.multiply_inplace(encrypted, encrypted);
    // cout << "    + size of encrypted: " << encrypted.size() << endl;
    // cout << "    + noise budget: " << decryptor.invariant_noise_budget(encrypted) << " bits"
    // << endl;
}
void multiply_and_reliearize(Ciphertext &encrypted1, Ciphertext &encrypted2, Evaluator &evaluator, Decryptor &decryptor,RelinKeys &relin_keys) {
    evaluator.multiply_inplace(encrypted1, encrypted2);
    // Relinearize the result
    evaluator.relinearize_inplace(encrypted1, relin_keys);
    // cout << "    + size of encrypted after relinearize: " << encrypted.size() << endl;
    cout << "    + noise budget after relinearize: " << decryptor.invariant_noise_budget(encrypted1) << " bits"
    << endl;

    // // Perform multiplication
    // // cout << "    + size of encrypted before mul: " << encrypted.size() << endl;
    // evaluator.multiply_inplace(encrypted, encrypted);
    // // cout << "    + size of encrypted: " << encrypted.size() << endl;
    // // cout << "    + noise budget: " << decryptor.invariant_noise_budget(encrypted) << " bits"
    // // << endl;

    // // Relinearize the result
    // evaluator.relinearize_inplace(encrypted, relin_keys);
    // // cout << "    + size of encrypted after relinearize: " << encrypted.size() << endl;
    // cout << "    + noise budget after relinearize: " << decryptor.invariant_noise_budget(encrypted) << " bits"
    // << endl;

}
void multiply_and_reliearize_modswitch(Ciphertext &encrypted1, Ciphertext &encrypted2, Evaluator &evaluator, Decryptor &decryptor,RelinKeys &relin_keys) {
    evaluator.multiply_inplace(encrypted1, encrypted2);
    // Relinearize the result
    evaluator.relinearize_inplace(encrypted1, relin_keys);
    // cout << "    + noise budget after relinearize: " << decryptor.invariant_noise_budget(encrypted1) << " bits"
    // << endl;
    // Modulus switch to control noise growth
    evaluator.mod_switch_to_next_inplace(encrypted1);
    evaluator.mod_switch_to_next_inplace(encrypted2);
    cout << "    + noise budget after mod switch: " << decryptor.invariant_noise_budget(encrypted1) << " bits"
    << endl;


    // Perform multiplication
    // cout << "    + size of encrypted before mul: " << encrypted.size() << endl;
    // evaluator.multiply_inplace(encrypted, encrypted);
    // // cout << "    + size of encrypted: " << encrypted.size() << endl;
    // // cout << "    + noise budget: " << decryptor.invariant_noise_budget(encrypted) << " bits"
    // // << endl;

    // // Relinearize the result
    // evaluator.relinearize_inplace(encrypted, relin_keys);
    // // cout << "    + size of encrypted after relinearize: " << encrypted.size() << endl;
    // // cout << "    + noise budget after relinearize: " << decryptor.invariant_noise_budget(encrypted) << " bits"
    // // << endl;

    // // Modulus switch to control noise growth
    // evaluator.mod_switch_to_next_inplace(encrypted);
    // // cout << "    + size of encrypted after mod switch: " << encrypted.size() << endl;
    // cout << "    + noise budget after mod switch: " << decryptor.invariant_noise_budget(encrypted) << " bits"
    // << endl;
}
void bfv_mul()
{
    // 计算开始时间
    auto start_time = std::chrono::high_resolution_clock::now();
    // 创建加密参数
    EncryptionParameters parms(scheme_type::bfv);
    // size_t poly_modulus_degree = 4096;
    // size_t poly_modulus_degree = 8192;
    // size_t poly_modulus_degree = 16384;
    size_t poly_modulus_degree = 32768;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    // parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 60 }));
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 60));

    SEALContext context(parms);

    /*
    Print the parameters that we have chosen.
    */
    print_line(__LINE__);
    cout << "Set encryption parameters and print" << endl;
    print_parameters(context);

    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    /*
    Batching and slot operations are the same in BFV and BGV.
    */
//    BatchEncoder batch_encoder(context);
//    size_t slot_count = batch_encoder.slot_count();
//    size_t row_size = slot_count / 2;
//     cout << "Plaintext matrix row size: " << row_size << endl;

    uint64_t x = 103;
    Plaintext plain_x(uint64_to_hex_string(x));
    Ciphertext encrypted_x;
    encryptor.encrypt(plain_x, encrypted_x);
    Ciphertext encrypted_res;
    encryptor.encrypt(plain_x, encrypted_res);
    // Perform multiplications
    int max_multiplications = 12;
    
    for (int i = 1; i < max_multiplications; ++i) {
        cout << "i:" << i << endl;
        // multiply(encrypted_res, evaluator, decryptor);
        // multiply_and_reliearize(encrypted_res, evaluator, decryptor, relin_keys);
        multiply_and_reliearize_modswitch(encrypted_res, encrypted_x, evaluator, decryptor, relin_keys);

        // multiply_and_reliearize(encrypted_res, encrypted_x, evaluator, decryptor, relin_keys);

        // Attempt to decrypt
        int noise_budget = decryptor.invariant_noise_budget(encrypted_res);
        if(noise_budget != 0){
            Plaintext decrypted_x;
            decryptor.decrypt(encrypted_res, decrypted_x);
            cout << "0x" << decrypted_x.to_string()  << endl;

        } else {
            cout << "Decryption failed after " << i << " multiplications." << endl;
            return;
        }
        // 计算结束时间
        auto end_time = std::chrono::high_resolution_clock::now();
        // 计算时间差
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        // 打印运行时间
        std::cout << "代码运行时间: " << duration.count() << " 毫秒" << std::endl;
    }




}

