import tenseal.sealapi as sealapi
import csv
import time
import pytest
import tabulate

def approx_equal(a, b, epsilon):
    return abs(a - b) > epsilon

def helper_params_ckks(poly_modulus_degree, coeff_mod_bit_sizes):
    parms = sealapi.EncryptionParameters(sealapi.SCHEME_TYPE.CKKS)
    parms.set_poly_modulus_degree(poly_modulus_degree)
    coeff = sealapi.CoeffModulus.Create(poly_modulus_degree, coeff_mod_bit_sizes)
    parms.set_coeff_modulus(coeff)

    return parms


# def helper_context_ckks(poly_modulus_degree):
#     return sealapi.SEALContext(
#         helper_params_ckks(poly_modulus_degree), True, sealapi.SEC_LEVEL_TYPE.TC128
#     )

# def is_close_enough(out, expected):
#     for idx in range(len(expected)):
#         assert abs(expected[idx] - out[idx]) < 0.1

# 从csv中读取数据
data_len = 20
with open('/echo-project/ckks/TenSEAL-main/mytest/datatest/cnn_gradients.csv') as csvfile:
    reader = csv.reader(csvfile)
    header = next(reader) #跳过第一行
    count = 0
    data = []
    for row in reader:
        if count < data_len :
            temp = row[2]
            # temp = row[1].replace('[','')
            # temp = temp.replace(']','')
            # print("count:", count)
            data.append(float(temp))
            count = count + 1
# print(data)
# 将数据转换为list型，以符合ts.context对入参类型的要求；计算原数据之和以及平均值
i = 0
re_sum = 0
while i < data_len:
    re_sum = re_sum + data[i]
    data[i] = [data[i]]
    i = i + 1
re_avg = re_sum / data_len
# print("re_sum:", re_sum)
# print("re_avg:", re_avg)

ct_size_benchmarks = [["Polynomial modulus", "Coefficient modulus sizes", "Scale", "Operation", "Status", "Time"]]

for (poly_mod, coeff_mod_bit_sizes, prec) in [
    (8192, [60, 40, 40, 60], 40),
    (8192, [40, 21, 21, 40], 40),
    (8192, [40, 21, 21, 40], 21),
    (8192, [40, 21, 21, 40], 40),
    (8192, [30, 20, 30], 40),
    (8192, [20, 20, 20], 38),
    (4096, [40, 20, 40], 40),
    (4096, [30, 20, 30], 40),
    (4096, [20, 20, 20], 38),
    (4096, [19, 19, 19], 35),
    (4096, [18, 18, 18], 33),
    (4096, [30, 30, 30], 25),
    (4096, [25, 25, 25], 20),
    (4096, [18, 18, 18], 16),
    (2048, [18, 18, 18], 16),
]:
    # 初始化
    # ctx = helper_context_ckks()
    # start_time1 = time.time()
    ctx = sealapi.SEALContext(
        helper_params_ckks(poly_mod, coeff_mod_bit_sizes), True, sealapi.SEC_LEVEL_TYPE.TC128
    )
    # print("poly_mod:", poly_mod)
    # print("coeff_mod_bit_sizes:", coeff_mod_bit_sizes)
    # print("prec:", prec)

    encoder = sealapi.CKKSEncoder(ctx)
    keygen = sealapi.KeyGenerator(ctx)
    pk = sealapi.PublicKey()
    public_key = keygen.create_public_key(pk)
    secret_key = keygen.secret_key()

    decryptor = sealapi.Decryptor(ctx, secret_key)
    encryptor = sealapi.Encryptor(ctx, pk, secret_key)
    evaluator = sealapi.Evaluator(ctx)

    # plaintext = sealapi.Plaintext()
    # ciphertext = sealapi.Ciphertext(ctx)
    scale = 2 ** prec
    ckks_vec = []
    zero = [0]
    div = [1/data_len]

    plain_sum = sealapi.Plaintext()
    cipher_sum = sealapi.Ciphertext(ctx)
    encoder.encode(zero, scale, plain_sum)
    encryptor.encrypt(plain_sum, cipher_sum)

    i = 0
    start_time1 = time.time()

    try:
        while i < data_len:
            plain = sealapi.Plaintext()
            cipher = sealapi.Ciphertext(ctx)
            encoder.encode(data[i], scale, plain)
            # print("data[i]:", data[i])
            encryptor.encrypt(plain, cipher)
            ckks_vec.append(cipher)
            # cipher_sum = cipher_sum + ckks_vec[i]
            evaluator.add_inplace(cipher_sum, cipher)
            i = i + 1
    except:
            ct_size_benchmarks.append([poly_mod, coeff_mod_bit_sizes, "2**{}".format(prec), "sum", "failed", "/"])
            continue    

    # 求和
    plaintext_sum = sealapi.Plaintext()
    decryptor.decrypt(cipher_sum, plaintext_sum)
    decrypted_sum = encoder.decode_double(plaintext_sum)
    end_time1 = time.time()
    # print("decrypted_sum:", decrypted_sum[0])

    epsilon = 1.0
    for dec_prec in range(prec):
        epsilon *= 0.1  # 根据精度迭代计算误差容限
        if approx_equal(float(decrypted_sum[0]), re_sum, epsilon):
            # print(f"精度测试通过，精度为 10^-{dec_prec}")
            ct_size_benchmarks.append([poly_mod, coeff_mod_bit_sizes, "2**{}".format(prec), "sum",  "decryption prec 10 ** {}".format(-dec_prec), format(end_time1 - start_time1, '.3f') + "s"])
            break

    # // 梯度值之和求平均
    start_time2 = time.time()
    plain_div = sealapi.Plaintext()
    cipher_div = sealapi.Ciphertext(ctx)
    encoder.encode(div, scale, plain_div)
    encryptor.encrypt(plain_div, cipher_div)
    try:
        plain_len_rec = sealapi.Plaintext()
        cipher_avg = sealapi.Ciphertext()
        evaluator.multiply(cipher_sum, cipher_div, cipher_avg)
    except:
        ct_size_benchmarks.append([poly_mod, coeff_mod_bit_sizes, "2**{}".format(prec), "mul", "failed", "/"])
        continue

    plaintext_out = sealapi.Plaintext()
    plaintext_div = sealapi.Plaintext()

    decryptor.decrypt(cipher_div, plaintext_div)
    decrypted_div = encoder.decode_double(plaintext_div)
    
    decryptor.decrypt(cipher_avg, plaintext_out)
    decrypted = encoder.decode_double(plaintext_out)

    end_time2 = time.time()

    epsilon = 1.0
    for dec_prec in range(prec):
        epsilon *= 0.1  # 根据精度迭代计算误差容限
        if approx_equal(float(decrypted[0]), re_avg, epsilon):
            # print(f"精度测试通过，精度为 10^-{dec_prec}")
            ct_size_benchmarks.append([poly_mod, coeff_mod_bit_sizes, "2**{}".format(prec), "mul",  "decryption prec 10 ** {}".format(-dec_prec), format(end_time2 - start_time2, '.3f') + "s"])
            break
print(tabulate.tabulate(ct_size_benchmarks))