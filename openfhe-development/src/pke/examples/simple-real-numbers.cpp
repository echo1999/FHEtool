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
//   Simple examples for CKKS
//  */

// #define PROFILE

// #include "openfhe.h"

// using namespace lbcrypto;

// int main() {
//     // Step 1: Setup CryptoContext

//     // A. Specify main parameters
//     /* A1) Multiplicative depth:
//    * The CKKS scheme we setup here will work for any computation
//    * that has a multiplicative depth equal to 'multDepth'.
//    * This is the maximum possible depth of a given multiplication,
//    * but not the total number of multiplications supported by the
//    * scheme.
//    *
//    * For example, computation f(x, y) = x^2 + x*y + y^2 + x + y has
//    * a multiplicative depth of 1, but requires a total of 3 multiplications.
//    * On the other hand, computation g(x_i) = x1*x2*x3*x4 can be implemented
//    * either as a computation of multiplicative depth 3 as
//    * g(x_i) = ((x1*x2)*x3)*x4, or as a computation of multiplicative depth 2
//    * as g(x_i) = (x1*x2)*(x3*x4).
//    *
//    * For performance reasons, it's generally preferable to perform operations
//    * in the shorted multiplicative depth possible.
//    */
//     uint32_t multDepth = 1;

//     /* A2) Bit-length of scaling factor.
//    * CKKS works for real numbers, but these numbers are encoded as integers.
//    * For instance, real number m=0.01 is encoded as m'=round(m*D), where D is
//    * a scheme parameter called scaling factor. Suppose D=1000, then m' is 10 (an
//    * integer). Say the result of a computation based on m' is 130, then at
//    * decryption, the scaling factor is removed so the user is presented with
//    * the real number result of 0.13.
//    *
//    * Parameter 'scaleModSize' determines the bit-length of the scaling
//    * factor D, but not the scaling factor itself. The latter is implementation
//    * specific, and it may also vary between ciphertexts in certain versions of
//    * CKKS (e.g., in FLEXIBLEAUTO).
//    *
//    * Choosing 'scaleModSize' depends on the desired accuracy of the
//    * computation, as well as the remaining parameters like multDepth or security
//    * standard. This is because the remaining parameters determine how much noise
//    * will be incurred during the computation (remember CKKS is an approximate
//    * scheme that incurs small amounts of noise with every operation). The
//    * scaling factor should be large enough to both accommodate this noise and
//    * support results that match the desired accuracy.
//    */
//     // scaleModSize决定缩放因子D的比特长度
//     uint32_t scaleModSize = 50;

//     /* A3) Number of plaintext slots used in the ciphertext.
//    * CKKS packs multiple plaintext values in each ciphertext.
//    * The maximum number of slots depends on a security parameter called ring
//    * dimension. In this instance, we don't specify the ring dimension directly,
//    * but let the library choose it for us, based on the security level we
//    * choose, the multiplicative depth we want to support, and the scaling factor
//    * size.在本例中，我们不直接指定环尺寸，而是让库根据我们选择的安全级别、
//    * 我们希望支持的乘法深度以及缩放因子大小来为我们选择环尺寸。
//    *
//    * Please use method GetRingDimension() to find out the exact ring dimension
//    * being used for these parameters. Give ring dimension N, the maximum batch
//    * size is N/2, because of the way CKKS works.
//    * 请使用 GetRingDimension() 方法找出这些参数所使用的确切环尺寸。
//    * 由于 CKKS 的工作方式，在环尺寸为 N 的情况下，最大批量大小为 N/2。
//    */
//     // 密文中使用的明文槽数，CKKS 在每个密码文本中包含多个明文值。明文槽的最大数量取决于一个名为 "环维度 "（ring dimension）的安全参数。
//     uint32_t batchSize = 8;

//     /* A4) Desired security level based on FHE standards.
//    * This parameter can take four values. Three of the possible values
//    * correspond to 128-bit, 192-bit, and 256-bit security, and the fourth value
//    * corresponds to "NotSet", which means that the user is responsible for
//    * choosing security parameters. Naturally, "NotSet" should be used only in
//    * non-production environments, or by experts who understand the security
//    * implications of their choices.
//    *
//    * If a given security level is selected, the library will consult the current
//    * security parameter tables defined by the FHE standards consortium
//    * (https://homomorphicencryption.org/introduction/) to automatically
//    * select the security parameters. Please see "TABLES of RECOMMENDED
//    * PARAMETERS" in  the following reference for more details:
//    * http://homomorphicencryption.org/wp-content/uploads/2018/11/HomomorphicEncryptionStandardv1.1.pdf
//    */
//     CCParams<CryptoContextCKKSRNS> parameters;
//     parameters.SetMultiplicativeDepth(multDepth);
//     parameters.SetScalingModSize(scaleModSize);
//     parameters.SetBatchSize(batchSize);

//     CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);

//     // Enable the features that you wish to use
//     cc->Enable(PKE);
//     cc->Enable(KEYSWITCH);
//     cc->Enable(LEVELEDSHE);
//     std::cout << "CKKS scheme is using ring dimension " << cc->GetRingDimension() << std::endl << std::endl;
//     std::cout << "test!!!"<< std::endl;
//     // B. Step 2: Key Generation
//     /* B1) Generate encryption keys.
//    * These are used for encryption/decryption, as well as in generating
//    * different kinds of keys.
//    */
//     auto keys = cc->KeyGen();

//     /* B2) Generate the digit size
//    * In CKKS, whenever someone multiplies two ciphertexts encrypted with key s,
//    * we get a result with some components that are valid under key s, and
//    * with an additional component that's valid under key s^2.
//    *
//    * In most cases, we want to perform relinearization of the multiplicaiton
//    * result, i.e., we want to transform the s^2 component of the ciphertext so
//    * it becomes valid under original key s. To do so, we need to create what we
//    * call a relinearization key with the following line.
//    */
//     // 重线性化密钥
//     cc->EvalMultKeyGen(keys.secretKey);

//     /* B3) Generate the rotation keys
//    * CKKS supports rotating the contents of a packed ciphertext, but to do so,
//    * we need to create what we call a rotation key. This is done with the
//    * following call, which takes as input a vector with indices that correspond
//    * to the rotation offset we want to support. Negative indices correspond to
//    * right shift and positive to left shift. Look at the output of this demo for
//    * an illustration of this.
//    *
//    * Keep in mind that rotations work over the batch size or entire ring dimension (if the batch size is not specified).
//    * This means that, if ring dimension is 8 and batch
//    * size is not specified, then an input (1,2,3,4,0,0,0,0) rotated by 2 will become
//    * (3,4,0,0,0,0,1,2) and not (3,4,1,2,0,0,0,0).
//    * If ring dimension is 8 and batch
//    * size is set to 4, then the rotation of (1,2,3,4) by 2 will become (3,4,1,2).
//    * Also, as someone can observe
//    * in the output of this demo, since CKKS is approximate, zeros are not exact
//    * - they're just very small numbers.
//    */
//   /* B3) 生成旋转密钥
//    * CKKS 支持旋转打包密文的内容，但要做到这一点、
//    * 我们需要创建所谓的旋转密钥。下面的调用
//    * 下面的调用将输入一个矢量，该矢量的索引对应于
//    * 与我们希望支持的旋转偏移量相对应。负指数对应
//    * 右移，正则左移。请看本演示的输出
//    * 以说明这一点。
//    *
//    * 请记住，旋转是在批量大小或整个环尺寸（如果未指定批量大小）范围内进行的。
//    * 这意味着，如果环尺寸为 8，而批次
//    * 大小未指定，则输入值 (1,2,3,4,0,0,0,0) 旋转 2 将变为
//    * (3,4,0,0,0,0,1,2) 而不是 (3,4,1,2,0,0,0,0)。
//    * 如果环的尺寸是 8，而批次
//    * 大小设置为 4，那么 (1,2,3,4) 旋转 2 将变为 (3,4,1,2)。
//    * 另外，有人可以观察到
//    * 在这个演示的输出中，由于 CKKS 是近似值，所以零并不精确
//    * 它们只是很小的数字。
//    */
//     cc->EvalRotateKeyGen(keys.secretKey, {1, -2});

//     // Step 3: Encoding and encryption of inputs

//     // Inputs
//     std::vector<double> x1 = {0.25, 0.5, 0.75, 1.0, 2.0, 3.0, 4.0, 5.0};
//     std::vector<double> x2 = {5.0, 4.0, 3.0, 2.0, 1.0, 0.75, 0.5, 0.25};

//     // Encoding as plaintexts
//     Plaintext ptxt1 = cc->MakeCKKSPackedPlaintext(x1);
//     Plaintext ptxt2 = cc->MakeCKKSPackedPlaintext(x2);

//     std::cout << "Input x1: " << ptxt1 << std::endl;
//     std::cout << "Input x2: " << ptxt2 << std::endl;

//     // Encrypt the encoded vectors
//     auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
//     auto c2 = cc->Encrypt(keys.publicKey, ptxt2);

//     // Step 4: Evaluation

//     // Homomorphic addition
//     auto cAdd = cc->EvalAdd(c1, c2);

//     // Homomorphic subtraction
//     auto cSub = cc->EvalSub(c1, c2);

//     // Homomorphic scalar multiplication
//     auto cScalar = cc->EvalMult(c1, 4.0);

//     // Homomorphic multiplication
//     auto cMul = cc->EvalMult(c1, c2);

//     // Homomorphic rotations
//     auto cRot1 = cc->EvalRotate(c1, 1);
//     auto cRot2 = cc->EvalRotate(c1, -2);

//     // Step 5: Decryption and output
//     Plaintext result;
//     // We set the cout precision to 8 decimal digits for a nicer output.
//     // If you want to see the error/noise introduced by CKKS, bump it up
//     // to 15 and it should become visible.
//     std::cout.precision(8);

//     std::cout << std::endl << "Results of homomorphic computations: " << std::endl;

//     cc->Decrypt(keys.secretKey, c1, &result);
//     result->SetLength(batchSize);
//     std::cout << "x1 = " << result;
//     std::cout << "Estimated precision in bits: " << result->GetLogPrecision() << std::endl;

//     // Decrypt the result of addition
//     cc->Decrypt(keys.secretKey, cAdd, &result);
//     result->SetLength(batchSize);
//     std::cout << "x1 + x2 = " << result;
//     std::cout << "Estimated precision in bits: " << result->GetLogPrecision() << std::endl;

//     // Decrypt the result of subtraction
//     cc->Decrypt(keys.secretKey, cSub, &result);
//     result->SetLength(batchSize);
//     std::cout << "x1 - x2 = " << result << std::endl;

//     // Decrypt the result of scalar multiplication
//     cc->Decrypt(keys.secretKey, cScalar, &result);
//     result->SetLength(batchSize);
//     std::cout << "4 * x1 = " << result << std::endl;

//     // Decrypt the result of multiplication
//     cc->Decrypt(keys.secretKey, cMul, &result);
//     result->SetLength(batchSize);
//     std::cout << "x1 * x2 = " << result << std::endl;

//     // Decrypt the result of rotations

//     cc->Decrypt(keys.secretKey, cRot1, &result);
//     result->SetLength(batchSize);
//     std::cout << std::endl << "In rotations, very small outputs (~10^-10 here) correspond to 0's:" << std::endl;
//     std::cout << "x1 rotate by 1 = " << result << std::endl;

//     cc->Decrypt(keys.secretKey, cRot2, &result);
//     result->SetLength(batchSize);
//     std::cout << "x1 rotate by -2 = " << result << std::endl;

//     return 0;
// }

// #include "openfhe.h"

// using namespace lbcrypto;

// int main() {
//       // 计算开始时间
//     auto start_time = std::chrono::high_resolution_clock::now();

//     CCParams<CryptoContextCKKSRNS> parameters;
//     // 指定主要参数
//     // A1) 密匙分布
//     // CKKS 的秘钥分布应该是 SPARSE_TERNARY 或 UNIFORM_TERNARY，此处指定UNIFORM_TERNARY分布
//     SecretKeyDist secretKeyDist = UNIFORM_TERNARY;
//     parameters.SetSecretKeyDist(secretKeyDist);

//     // A2) 基于 FHE 标准的预期安全级别。
//     // HEStd_128_classic：128位安全

//     parameters.SetSecurityLevel(HEStd_128_classic);

//     // A3) 缩放参数。
//     /*
//     默认情况下，我们将模量大小和重缩放技术设置为以下值，
//     以获得良好的精度和性能权衡。我们建议您保留以下参数，除非您是 FHE 专家。
//     */
//     // #if NATIVEINT == 128 && !defined(__EMSCRIPTEN__)
//     //   ScalingTechnique rescaleTech = FIXEDAUTO;
//     //   usint dcrtBits               = 78;
//     //   usint firstMod               = 89;
//     // #else
//     //     ScalingTechnique rescaleTech = FLEXIBLEAUTO;
//     //     usint dcrtBits               = 59;
//     //     usint firstMod               = 60;
//     // #endif
//     usint dcrtBits = 50;
//     usint firstMod = 60;
//     ScalingTechnique rescaleTech = FIXEDAUTO;

//     parameters.SetScalingModSize(dcrtBits);
//     parameters.SetScalingTechnique(rescaleTech);
//     parameters.SetFirstModSize(firstMod);

//     // A4) 乘法深度
//     /*
//     引导的目的是增加可用层的数量，换句话说，就是动态增加乘法深度。
//     然而，引导过程本身需要消耗一些层级才能运行。
//     我们使用 GetBootstrapDepth 计算所需的引导级数，
//     并将其添加到 levelsAvailableAfterBootstrap 中以设置初始乘法深度。
//     我们建议使用下面的输入参数开始。
//     */
//     // std::vector<uint32_t> levelBudget = {4, 4};
//     std::vector<uint32_t> levelBudget = {3, 3};

//     /*
//     我们为用户提供了在引导过程中配置优化算法值的选项。
//     在这里，我们分别为编码和解码的线性变换中的 "小步-大步 "算法指定 "大步"。
//     可以选择 2 的幂或槽数的精确除数。将其设置为默认值 {0, 0} 可以让 OpenFHE 自动选择值。
//     */
//     std::vector<uint32_t> bsgsDim = {0, 0};

//     // uint32_t levelsAvailableAfterBootstrap = 10;
//     uint32_t levelsAvailableAfterBootstrap = 7;
//     usint depth = levelsAvailableAfterBootstrap + FHECKKSRNS::GetBootstrapDepth(levelBudget, secretKeyDist);
//     parameters.SetMultiplicativeDepth(depth);

//     CryptoContext<DCRTPoly> cryptoContext = GenCryptoContext(parameters);

//     cryptoContext->Enable(PKE);
//     cryptoContext->Enable(KEYSWITCH);
//     cryptoContext->Enable(LEVELEDSHE);
//     cryptoContext->Enable(ADVANCEDSHE);
//     cryptoContext->Enable(FHE);

//     usint ringDim = cryptoContext->GetRingDimension();
//     // 这是可用于完全打包的最大插槽数量。
//     // usint numSlots = ringDim / 2;
//     usint numSlots = 4;
//     std::cout << "CKKS scheme is using ring dimension " << ringDim << std::endl << std::endl;

//     // 步骤 2：自举的预计算
//     // cryptoContext->EvalBootstrapSetup(levelBudget);    
//     cryptoContext->EvalBootstrapSetup(levelBudget, bsgsDim, numSlots);

//     auto keyPair = cryptoContext->KeyGen();
//     cryptoContext->EvalMultKeyGen(keyPair.secretKey);
//     cryptoContext->EvalBootstrapKeyGen(keyPair.secretKey, numSlots);

//     std::vector<double> x = {3.0};
//     size_t encodedLength  = x.size();

//     // 我们从一个已耗尽所有等级的密码文本开始。
//     Plaintext ptxt = cryptoContext->MakeCKKSPackedPlaintext(x, 1, depth - 1);

//     ptxt->SetLength(encodedLength);
//     std::cout << "Input: " << ptxt << std::endl;

//     Ciphertext<DCRTPoly> ciph = cryptoContext->Encrypt(keyPair.publicKey, ptxt);

//     std::cout << "Initial number of levels remaining: " << depth - ciph->GetLevel() << std::endl;

//     // 执行Bootstrapping操作
//     auto ciphertextAfter = cryptoContext->EvalBootstrap(ciph);

//     std::cout << "Number of levels remaining after bootstrapping: " << depth - ciphertextAfter->GetLevel() << std::endl
//           << std::endl;

//     Plaintext result;
//     cryptoContext->Decrypt(keyPair.secretKey, ciphertextAfter, &result);
//     result->SetLength(encodedLength);
//     std::cout << "Output after bootstrapping \n\t" << result << std::endl;


//     // 计算结束时间
//     auto end_time = std::chrono::high_resolution_clock::now();
//     // 计算时间差
//     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
//     // 打印运行时间
//     std::cout << "代码运行时间: " << duration.count() << " 毫秒" << std::endl;
    
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

void ckks_mul(int scale_num, ScalingTechnique scalTech, int n, int data_decimal, int mul_decimal, int mul_times, double data_randomNumber[], double mul_randomNumber[], double real[]){
    // 输出参数
      cout << "ScalingModSize:" << scale_num << endl;
      cout << "MultiplicativeDepth:" << mul_times << endl;
    //   cout << "ScalingTechnique:" << scalTech << endl;

      int time_sum = 0;
      int prec_sum = 0;
      for (int i = 0; i < n; ++i) {
        // 计算开始时间
        auto start_time = std::chrono::high_resolution_clock::now();

        uint32_t batchSize = 1;
        CCParams<CryptoContextCKKSRNS> parameters;
        parameters.SetMultiplicativeDepth(mul_times);
        parameters.SetScalingModSize(scale_num);
        parameters.SetScalingTechnique(scalTech);
        parameters.SetBatchSize(batchSize);
        parameters.SetSecurityLevel(HEStd_128_classic);

        CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);

        cc->Enable(PKE);
        cc->Enable(KEYSWITCH);
        cc->Enable(LEVELEDSHE);

        auto keys = cc->KeyGen();
        cc->EvalMultKeyGen(keys.secretKey);


        std::vector<double> data = {data_randomNumber[i]};
        std::vector<double> mul = {mul_randomNumber[i]};

        Plaintext plaintext_data = cc->MakeCKKSPackedPlaintext(data);
        Plaintext plaintext_mul = cc->MakeCKKSPackedPlaintext(mul);

        auto cipertext_res = cc->Encrypt(plaintext_data, keys.publicKey);
        auto cipertext_mul = cc->Encrypt(plaintext_mul, keys.publicKey);

        //  做mul_times次乘法
        for(int j = 0; j < mul_times; ++j){
            // cout << "j:" << j << endl;
            try{
                cipertext_res = cc->EvalMult(cipertext_res, cipertext_mul);
            }catch(const std::exception& e){
                // std::cout << "乘法失败" << endl;
                std::cout << "time_avg:0ms"<< endl;
                std::cout << "prec_avg:0"<< endl;
                std::cout << "----------"<< endl;
                return;
            }
        }

        Plaintext plaintext_res;
        std::cout.precision(8);

        cc->Decrypt(cipertext_res, keys.secretKey, &plaintext_res);
        plaintext_res->SetLength(batchSize);

        std::vector<complex<double>> finaleResult = plaintext_res->GetCKKSPackedValue();
        // std::cout << "乘法结果: " << finaleResult[0].real() << std::endl;

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
        // cout << "result:" << finaleResult[0].real() << endl;
        int prec = compareDoublePrecision(real[i], finaleResult[0].real(), prec_max);
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
    // cout<< "n:" << n <<endl;
    // ckks_mul(15, FLEXIBLEAUTO, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    ckks_mul(20, FLEXIBLEAUTO, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    ckks_mul(30, FLEXIBLEAUTO, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    ckks_mul(40, FLEXIBLEAUTO, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    ckks_mul(50, FLEXIBLEAUTO, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    // ckks_mul(60, FLEXIBLEAUTO, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real);
    return 0;
    
}