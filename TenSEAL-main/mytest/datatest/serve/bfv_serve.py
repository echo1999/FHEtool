import csv
import tenseal as ts
import pytest
from IPython.display import HTML, display
import tabulate
import time
import random
import sys
from math import pow, fabs

def approx_equal(a, b, epsilon):
    # print("std::abs(a-b)", abs(a-b))
    # print("epsilon", epsilon)
    return abs(a - b) > epsilon

def read_arrays_and_params_from_file(filename):
    data_randomNumber = []
    mul_randomNumber = []
    real = []
    data_integer = data_decimal = mul_integer = mul_decimal = mul_times = 0

    with open(filename, 'r') as file:
        lines = file.readlines()
        n = int(lines[0].split(":")[1])

        for line in lines[1:]:
            if "data_integer:" in line:
                data_integer = int(line.split(":")[1])
            elif "data_decimal:" in line:
                data_decimal = int(line.split(":")[1])
            elif "mul_integer:" in line:
                mul_integer = int(line.split(":")[1])
            elif "mul_decimal:" in line:
                mul_decimal = int(line.split(":")[1])
            elif "mul_times:" in line:
                mul_times = int(line.split(":")[1])
            elif "data_randomNumber:" in line:
                for i in range(n):
                    data_randomNumber.append(float(lines[lines.index(line) + i + 1]))
            elif "mul_randomNumber:" in line:
                for i in range(n):
                    mul_randomNumber.append(float(lines[lines.index(line) + i + 1]))
            elif "real:" in line:
                for i in range(n):
                    real.append(float(lines[lines.index(line) + i + 1]))
    return n, data_randomNumber, mul_randomNumber, real, data_integer, data_decimal, mul_integer, mul_decimal, mul_times

def compareDoublePrecision(num1, num2, prec_max):
    # 获取两个数字的整数部分
    int_part1 = int(num1)
    int_part2 = int(num2)

    # 获取两个数字的小数部分的字符串表示
    frac_str1 = "{:.{}f}".format(num1 - int_part1, prec_max)[2:]
    frac_str2 = "{:.{}f}".format(num2 - int_part2, prec_max)[2:]

    # 比较整数部分
    if int_part1 != int_part2:
        return 0  # 整数部分不同，返回0

    # 比较小数部分
    for i in range(prec_max):
        if frac_str1[i] != frac_str2[i]:
            return i + 1  # 从第i+1位开始不同

    return prec_max  # 所有精度都相同，返回prec_max

def bfv_mul(poly_mod, modulus, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real):
    precision_sum = 0
    time_sum = 0
    real_n = n
    print("poly_modulus_degree:",poly_mod)
    print("plain_modulus:", modulus)

    for i in range(n):
        start_time1 = time.time()

        context = ts.context(
        scheme=ts.SCHEME_TYPE.BFV,
        poly_modulus_degree=poly_mod,
        plain_modulus = modulus,
        # encryption_type=enc_type,
        )

        # 扩充数值
        data_magnificant = 1
        mul_magnificant = 1
        for k in range(data_decimal+1):
            data_magnificant *= 10
        for k in range(mul_decimal+1):
            mul_magnificant *= 10
        data = int(data_randomNumber[i] * data_magnificant)
        # print("data_randomNumber after magnificant:", data_randomNumber[i])
        mul = int(mul_randomNumber[i] * mul_magnificant)
        # print("mul_randomNumber after magnificant:", mul_randomNumber[i])

        vec1 = [data]
        vec2 = [mul]

        bfv_vec1 = ts.bfv_vector(context, vec1)
        bfv_vec2 = ts.bfv_vector(context, vec2)

        # 做mul_times次乘法
        for j in range(mul_times):
            # print("j:", j)
            try:
                bfv_vec1 = bfv_vec1 * bfv_vec2
            except Exception as e:
                # print("乘法失败")
                # real_n = real_n - 1
                print("time_avg: 0ms")
                print("prec_avg: 0")
                real_n = real_n - 1
                return

        dec_vec = bfv_vec1.decrypt()
        # print("dec_vec:", dec_vec)
        res = dec_vec[0] / data_magnificant
        for j in range(mul_times):
            res = res / mul_magnificant
        # print("res:", res)
        end_time1 = time.time()
        elapsed_time = end_time1 - start_time1
        # print("time:",elapsed_time)

        # 计算精度
        prec_max = max(data_decimal, mul_decimal)
        # print("prec_max:", prec_max)
        # print("real:", real[i])
        # print("res:", res)
        prec = compareDoublePrecision(real[i], res, prec_max)
        precision_sum = precision_sum + prec
        time_sum = time_sum + elapsed_time
        # print("prec:", prec)
    precision_avg = precision_sum / real_n 
    time_avg = time_sum / real_n
    print("time_avg:", time_avg*1000 ,"ms")
    print("prec_avg:", precision_avg)
    print("----------")


# def bfv_serve(data_integer, data_decimal, mul_integer, mul_decimal, mul_times):
    # n = 10
    # # 生成满足条件的随机数
    # data_randomNumber = [0]*n
    # mul_randomNumber = [0]*n
    # real = [0]*n
    # for i in range(n):
    #     data_randomNumber[i] = generate_random_number(data_integer, data_decimal)
    #     mul_randomNumber[i] = generate_random_number(mul_integer, mul_decimal)
    #     # print("data_randomNumber:", data_randomNumber)
    #     # print("mul_randomNumber:", mul_randomNumber)
    #     real[i] = data_randomNumber[i]
    #     for _ in range(mul_times):
    #         real[i] = real[i] * mul_randomNumber[i]
    #         # print("real:", real)

    # bfv_mul(4096, 1152921504606584833, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real)
    # bfv_mul(8192, 1152921504606584833, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real)
    # bfv_mul(16384, 1152921504606584833, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real)
    # bfv_mul(32768, 1152921504606584833, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real)

if __name__ == "__main__":
    # 获取命令行参数
    # data_integer = int(sys.argv[1])
    # data_decimal = int(sys.argv[2])
    # mul_integer = int(sys.argv[3])
    # mul_decimal = int(sys.argv[4])
    # mul_times = int(sys.argv[5])

    # 调用函数获取参数和随机数
    n, data_randomNumber, mul_randomNumber, real, data_integer, data_decimal, mul_integer, mul_decimal, mul_times = read_arrays_and_params_from_file("/echo-project/ckks/data.txt")
    # 调用函数并传递参数
    # bfv_serve(data_integer, data_decimal, mul_integer, mul_decimal, mul_times)

    bfv_mul(4096, 1152921504606584833, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real)
    bfv_mul(8192, 1152921504606584833, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real)
    bfv_mul(16384, 1152921504606584833, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real)
    bfv_mul(32768, 1152921504606584833, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real)

