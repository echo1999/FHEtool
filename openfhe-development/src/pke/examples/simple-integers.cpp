// //==================================================================================
// // BSD 2-Clause License
// //
// // Copyright (c) 2014-2022, NJIT, Duality Technologies Inc. and other contributors
// //
// // All rights reserved.
// //
// // Author TPOC: contact@openfhe.org
// //
// // Redistribution and use in source and binary forms, with or without
// // modification, are permitted provided that the following conditions are met:
// //
// // 1. Redistributions of source code must retain the above copyright notice, this
// //    list of conditions and the following disclaimer.
// //
// // 2. Redistributions in binary form must reproduce the above copyright notice,
// //    this list of conditions and the following disclaimer in the documentation
// //    and/or other materials provided with the distribution.
// //
// // THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// // AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// // IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// // DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// // FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// // DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// // SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// // CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// // OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// // OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// //==================================================================================

// /*
//   Simple example for BFVrns (integer arithmetic)
//  */

// #include "openfhe.h"

// using namespace lbcrypto;

// int main() {
//     // Sample Program: Step 1: Set CryptoContext
//     CCParams<CryptoContextBFVRNS> parameters;
//     parameters.SetPlaintextModulus(65537);
//     parameters.SetMultiplicativeDepth(2);

//     CryptoContext<DCRTPoly> cryptoContext = GenCryptoContext(parameters);
//     // Enable features that you wish to use
//     cryptoContext->Enable(PKE);
//     cryptoContext->Enable(KEYSWITCH);
//     cryptoContext->Enable(LEVELEDSHE);

//     // Sample Program: Step 2: Key Generation

//     // Initialize Public Key Containers
//     KeyPair<DCRTPoly> keyPair;

//     // Generate a public/private key pair
//     keyPair = cryptoContext->KeyGen();

//     // Generate the relinearization key
//     cryptoContext->EvalMultKeyGen(keyPair.secretKey);

//     // Generate the rotation evaluation keys
//     cryptoContext->EvalRotateKeyGen(keyPair.secretKey, {1, 2, -1, -2});

//     // Sample Program: Step 3: Encryption

//     // First plaintext vector is encoded
//     std::vector<int64_t> vectorOfInts1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
//     Plaintext plaintext1               = cryptoContext->MakePackedPlaintext(vectorOfInts1);
//     // Second plaintext vector is encoded
//     std::vector<int64_t> vectorOfInts2 = {3, 2, 1, 4, 5, 6, 7, 8, 9, 10, 11, 12};
//     Plaintext plaintext2               = cryptoContext->MakePackedPlaintext(vectorOfInts2);
//     // Third plaintext vector is encoded
//     std::vector<int64_t> vectorOfInts3 = {1, 2, 5, 2, 5, 6, 7, 8, 9, 10, 11, 12};
//     Plaintext plaintext3               = cryptoContext->MakePackedPlaintext(vectorOfInts3);

//     // The encoded vectors are encrypted
//     auto ciphertext1 = cryptoContext->Encrypt(keyPair.publicKey, plaintext1);
//     auto ciphertext2 = cryptoContext->Encrypt(keyPair.publicKey, plaintext2);
//     auto ciphertext3 = cryptoContext->Encrypt(keyPair.publicKey, plaintext3);

//     // Sample Program: Step 4: Evaluation

//     // Homomorphic additions
//     auto ciphertextAdd12     = cryptoContext->EvalAdd(ciphertext1, ciphertext2);
//     auto ciphertextAddResult = cryptoContext->EvalAdd(ciphertextAdd12, ciphertext3);

//     // Homomorphic multiplications
//     auto ciphertextMul12      = cryptoContext->EvalMult(ciphertext1, ciphertext2);
//     auto ciphertextMultResult = cryptoContext->EvalMult(ciphertextMul12, ciphertext3);

//     // Homomorphic rotations
//     auto ciphertextRot1 = cryptoContext->EvalRotate(ciphertext1, 1);
//     auto ciphertextRot2 = cryptoContext->EvalRotate(ciphertext1, 2);
//     auto ciphertextRot3 = cryptoContext->EvalRotate(ciphertext1, -1);
//     auto ciphertextRot4 = cryptoContext->EvalRotate(ciphertext1, -2);

//     // Sample Program: Step 5: Decryption

//     // Decrypt the result of additions
//     Plaintext plaintextAddResult;
//     cryptoContext->Decrypt(keyPair.secretKey, ciphertextAddResult, &plaintextAddResult);

//     // Decrypt the result of multiplications
//     Plaintext plaintextMultResult;
//     cryptoContext->Decrypt(keyPair.secretKey, ciphertextMultResult, &plaintextMultResult);

//     // Decrypt the result of rotations
//     Plaintext plaintextRot1;
//     cryptoContext->Decrypt(keyPair.secretKey, ciphertextRot1, &plaintextRot1);
//     Plaintext plaintextRot2;
//     cryptoContext->Decrypt(keyPair.secretKey, ciphertextRot2, &plaintextRot2);
//     Plaintext plaintextRot3;
//     cryptoContext->Decrypt(keyPair.secretKey, ciphertextRot3, &plaintextRot3);
//     Plaintext plaintextRot4;
//     cryptoContext->Decrypt(keyPair.secretKey, ciphertextRot4, &plaintextRot4);

//     plaintextRot1->SetLength(vectorOfInts1.size());
//     plaintextRot2->SetLength(vectorOfInts1.size());
//     plaintextRot3->SetLength(vectorOfInts1.size());
//     plaintextRot4->SetLength(vectorOfInts1.size());

//     std::cout << "Plaintext #1: " << plaintext1 << std::endl;
//     std::cout << "Plaintext #2: " << plaintext2 << std::endl;
//     std::cout << "Plaintext #3: " << plaintext3 << std::endl;

//     // Output results
//     std::cout << "\nResults of homomorphic computations" << std::endl;
//     std::cout << "#1 + #2 + #3: " << plaintextAddResult << std::endl;
//     std::cout << "#1 * #2 * #3: " << plaintextMultResult << std::endl;
//     std::cout << "Left rotation of #1 by 1: " << plaintextRot1 << std::endl;
//     std::cout << "Left rotation of #1 by 2: " << plaintextRot2 << std::endl;
//     std::cout << "Right rotation of #1 by 1: " << plaintextRot3 << std::endl;
//     std::cout << "Right rotation of #1 by 2: " << plaintextRot4 << std::endl;

//     return 0;
// }
#include "openfhe.h"

using namespace std;
using namespace lbcrypto;

// 读取数组和参数
int readArraysAndParamsFromFile(const std::string& filename,
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

int compareDoublePrecision(double num1, double num2, int prec_max) {
    // 获取两个数字的整数部分
    long long int_part1 = static_cast<long long>(num1);
    // cout << "int_part1:" << int_part1 << endl;
    long long int_part2 = static_cast<long long>(num2);
    // cout << "int_part2:" << int_part2 << endl;

    // 获取两个数字的小数部分的字符串表示
    std::ostringstream oss1, oss2;
    oss1 << std::fixed << std::setprecision(prec_max) << (num1 - static_cast<double>(int_part1));
    oss2 << std::fixed << std::setprecision(prec_max) << (num2 - static_cast<double>(int_part2));

    std::string frac_str1 = oss1.str().substr(2); // 去掉小数点前的 "0."
    std::string frac_str2 = oss2.str().substr(2);

    // 比较整数部分
    if (int_part1 != int_part2) {
        // cout << "整数部分不同" << endl;
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

void bfv_mul(int64_t plaintext_modulus, int n, int data_decimal, int mul_decimal, int mul_times, double data_randomNumber[], double mul_randomNumber[], double real[]){
    // 输出参数
    cout << "PlaintextModulus:" << plaintext_modulus << endl;
    cout << "MultiplicativeDepth:" << mul_times << endl;

    int time_sum = 0;
    int prec_sum = 0;
    for (int i = 0; i < n; ++i) {
        // 计算开始时间
        auto start_time = std::chrono::high_resolution_clock::now();

        // Sample Program: Step 1 - Set CryptoContext
        // CCParams<CryptoContextBGVRNS> parameters;
        CCParams<CryptoContextBFVRNS> parameters;
        parameters.SetMultiplicativeDepth(mul_times);
        // parameters.SetPlaintextModulus(100000595969);
        parameters.SetPlaintextModulus(plaintext_modulus);
        // parameters.SetPlaintextModulus(1152921504606584833);

        CryptoContext<DCRTPoly> cryptoContext = GenCryptoContext(parameters);
        // Enable features that you wish to use
        cryptoContext->Enable(PKE);
        cryptoContext->Enable(KEYSWITCH);
        cryptoContext->Enable(LEVELEDSHE);

        // Sample Program: Step 2 - Key Generation

        // Initialize Public Key Containers
        KeyPair<DCRTPoly> keyPair;

        // Generate a public/private key pair
        keyPair = cryptoContext->KeyGen();

        // Generate the relinearization key
        cryptoContext->EvalMultKeyGen(keyPair.secretKey);

        // 扩充数值
        int data_magnificant = 1;
        int mul_magnificant = 1;
        for(int k = 0; k < data_decimal; ++k){
            data_magnificant = data_magnificant * 10;
        }
        for(int k = 0; k < mul_decimal; ++k){
            mul_magnificant = mul_magnificant * 10;
        }

        double data_temp = data_randomNumber[i] * data_magnificant;
        // cout << "data_temp:" << data_temp << endl;
        int64_t data = static_cast<uint64_t>(std::llround(data_temp));
        // cout << "data:" << data << endl;
        int64_t mul = mul_randomNumber[i] * double(mul_magnificant);
        // cout << "mul:" << mul << endl;

        if (plaintext_modulus < data || plaintext_modulus < mul){
            // cout << "乘法失败" << endl;
            std::cout << "time_avg:0ms"<< endl;
            std::cout << "prec_avg:0"<< endl;
            std::cout << "----------"<< endl;
            return;
        }
        std::vector<int64_t> vectorOfIntsdata = {data};

        Plaintext plaintext_data               = cryptoContext->MakePackedPlaintext(vectorOfIntsdata);
        // Second plaintext vector is encoded
        std::vector<int64_t> vectorOfIntsmul = {mul};
        Plaintext plaintext_mul               = cryptoContext->MakePackedPlaintext(vectorOfIntsmul);

        // The encoded vectors are encrypted
        auto ciphertext_res = cryptoContext->Encrypt(keyPair.publicKey, plaintext_data);
        auto ciphertext_mul = cryptoContext->Encrypt(keyPair.publicKey, plaintext_mul);
        // auto ciphertext3 = cryptoContext->Encrypt(keyPair.publicKey, plaintext3);

        //  做mul_times次乘法
        for(int j = 0; j < mul_times; ++j){
            // cout << "j:" << j << endl;
            try{
                ciphertext_res = cryptoContext->EvalMultAndRelinearize(ciphertext_res, ciphertext_mul);
                cryptoContext->ModReduceInPlace(ciphertext_res);
            }catch(const std::exception& e){
                std::cout << "time_avg:0ms" << endl;
                std::cout << "prec_avg:0" << endl;
                std::cout << "----------"<< endl;
                return;
            }
        }

        // Attempt to decrypt
        Plaintext plaintext_res;
        cryptoContext->Decrypt(keyPair.secretKey, ciphertext_res, &plaintext_res);

        std::vector<int64_t> finalResult = plaintext_res->GetPackedValue();
        
        double decrypted_double = finalResult[0]/double(data_magnificant);
        for (int j = 0; j < mul_times; ++j){
            decrypted_double = decrypted_double/double(mul_magnificant);
        }
        // std::cout << "乘法结果: " << decrypted_double << std::endl;

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
        int prec = compareDoublePrecision(real[i], decrypted_double, prec_max);
        // cout << "real:" << real[i] << endl;
        // cout << "prec:" << prec <<endl;

        prec_sum = prec_sum + prec;
    }
    double time_avg = time_sum / double(n);
    std::cout << "time_avg:" << time_avg << "ms" << endl;
    double prec_avg = prec_sum / double(n);
    std::cout << "prec_avg:" << prec_avg << endl;
    std::cout << "----------"<< endl;
}
int main() {
    double* data_randomNumber;
    double* mul_randomNumber;
    double* real;
    int data_integer, data_decimal, mul_integer, mul_decimal, mul_times;

    int n = readArraysAndParamsFromFile("/echo-project/ckks/data.txt", data_randomNumber, mul_randomNumber, real, data_integer, data_decimal, mul_integer, mul_decimal, mul_times);

    // std::cout << "data_randomNumber[0]:" << data_randomNumber[0] << endl;
    // std::cout << "real[0]:" << real[0] << endl;
    // std::cout << "data_decimal:" << data_decimal << endl;
    bfv_mul(65537, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    bfv_mul(786433, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    bfv_mul(536903681, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    bfv_mul(100000595969, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    // bfv_mul(2066315351707, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);

    return 0;
}