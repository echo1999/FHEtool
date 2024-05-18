#include <iostream>
#include "examples.h"

using namespace std;
using namespace seal;

std::vector<double> readGradientFromCSV2() {
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
                // std::cerr << "无法解析浮点数：" << tokens[gradientColumnIndex] << std::endl;
            }
        }
    }

    file.close();
    return gradientValues;
}

void mybfv() {
    // 创建加密参数
    EncryptionParameters parms(scheme_type::bfv);
    // size_t poly_modulus_degree = 4096;
    // size_t poly_modulus_degree = 8192;
    size_t poly_modulus_degree = 16384;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    // parms.set_plain_modulus(1024);
    /*
    为了启用批处理，我们需要将plain_modules设置为一个素数，
    它全等于1模2*poly_modulus_degree。（这里是指要求素数满足的条件）。
    Microsoft SEAL提供了一种辅助方法来查找这样的素数。
    在本例中，我们创建了一个支持批处理的20位素数。
    */
    // 为什么要选择20位素数？
    // parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 20));
    parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 60));

    // 创建一个 SEALContext 对象
    // auto context = SEALContext::Create(parms);
    SEALContext context(parms);
    print_parameters(context);

    // // 查看是否确实启用了批处理
    // auto qualifiers = context.first_context_data()->qualifiers();
    // cout << "Batching enabled: " << boolalpha << qualifiers.using_batching << endl;

    // 密钥生成
    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);


    // 加密器和解密器
    Encryptor encryptor(context, public_key);
    Decryptor decryptor(context, secret_key);

    // 运算
    Evaluator evaluator(context);

    // 实例化BatchEncoder类
    BatchEncoder batch_encoder(context);
    size_t slot_count = batch_encoder.slot_count();
    size_t row_size = slot_count / 2;
    // cout << "Plaintext matrix row size: " << row_size << endl;

    // 载入数据
    vector<double> gradientValues = readGradientFromCSV2();

    // std::vector<int> scaled_arr;
    vector<uint64_t> scaled_arr(slot_count,0ULL);
    scaled_arr[0] = 0;
    Plaintext plain_arr;
    batch_encoder.encode(scaled_arr, plain_arr);
    Ciphertext encrypted_arr;
    encryptor.encrypt(plain_arr, encrypted_arr);
    long long sum = 0;
    // 计算开始时间
    auto start_time = std::chrono::high_resolution_clock::now();
    for (double value : gradientValues) {
        long scaledValue = static_cast<long>(value * 10000000000000000);
        // cout << "scaledValue:" << scaledValue << endl;
        sum = sum + scaledValue;
        // cout << "sum:" << sum << endl;
        // cout << "typeid(sum).name()：" << typeid(sum).name() << endl;
        vector<uint64_t> scaled_temp(slot_count,0ULL);
        scaled_temp[0] = scaledValue;
        Plaintext plain_temp;
        batch_encoder.encode(scaled_temp, plain_temp);
        Ciphertext encrypted_temp;
        encryptor.encrypt(plain_temp, encrypted_temp);
        // 相加
        evaluator.add_inplace(encrypted_arr, encrypted_temp);
    }

    // // 同态乘法
    // Plaintext plain_len_re("2");
    // evaluator.multiply_plain_inplace(encrypted_arr, plain_len_re);

    // 解密
    Plaintext plain_result;
    vector<uint64_t> pod_result;
    decryptor.decrypt(encrypted_arr, plain_result);
    batch_encoder.decode(plain_result, pod_result);

    // 创建一个空的浮点数组
    double res = static_cast<double>(pod_result[0]) / 10000000000.0;
    // 计算结束时间
    auto end_time = std::chrono::high_resolution_clock::now();
    // 计算时间差
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    // 打印运行时间
    std::cout << "代码运行时间: " << duration.count() << " 毫秒" << std::endl;
    
    cout << "    + Result plaintext matrix ...... Correct." << endl;
    // cout << "sum:" << sum << endl;
    print_matrix(pod_result, row_size);
    // cout<<  "res:" << res << endl;


}