import csv
import tenseal as ts
import pytest
from IPython.display import HTML, display
import tabulate
import time
import random
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
            return i   # 从第i+1位开始不同，精度为i

    return prec_max  # 所有精度都相同，返回prec_max

def ckks_mul(poly_mod, coeff_mod_bit_sizes, prec, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real):
    precision_sum = 0
    time_sum = 0
    real_n = n
    print("poly_modulus_degree:",poly_mod)
    print("coeff_mod_bit_sizes:", coeff_mod_bit_sizes)
    print("scale:2^", prec)

    for i in range(n):
        start_time1 = time.time()

        context = ts.context(
        scheme=ts.SCHEME_TYPE.CKKS,
        poly_modulus_degree=poly_mod,
        coeff_mod_bit_sizes=coeff_mod_bit_sizes,
        encryption_type=ts.ENCRYPTION_TYPE.ASYMMETRIC,
        )
        scale = 2 ** prec

        secret_key = context.secret_key()

        data = data_randomNumber[i]
        mul = mul_randomNumber[i]
        vec1 = [float(data)]
        print("vec1:", vec1)
        vec2 = [float(mul)]
        print("vec2:", vec2)

        ckks_vec1 = ts.ckks_vector(context, vec1, scale)
        ckks_vec2 = ts.ckks_vector(context, vec2, scale)

        # 做mul_times次乘法
        for j in range(mul_times):
            # print("j:", j)
            try:
                # ckks_vec[i] = ckks_vec[i] * ckks_vec2[i]
                ckks_vec1 = ckks_vec1 * ckks_vec2
            except Exception as e:
                # print("乘法失败")
                print("time_avg: 0ms")
                print("prec_avg: 0")
                real_n = real_n - 1
                return

        # dec_vec = ckks_vec[i].decrypt()
        dec_vec = ckks_vec1.decrypt(secret_key)
        # dec_vec = decrypt(ckks_mul)
        # print("dec_vec:", dec_vec)
        res = dec_vec[0]
        end_time1 = time.time()
        elapsed_time = end_time1 - start_time1
        # print("time:",elapsed_time)

        # 计算精度
        prec_max = max(data_decimal, mul_decimal)
        print("real:", real[i])
        print("res:", res)
        prec = compareDoublePrecision(real[i], res, prec_max)
        # print("prec:", prec)

        precision_sum = precision_sum + prec
        time_sum = time_sum + elapsed_time
     
    time_avg = time_sum / real_n
    precision_avg = precision_sum / real_n

    print("time_avg:", time_avg*1000 ,"ms")
    print("prec_avg:", precision_avg)
    print("----------")

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

    ckks_mul(4096, [24, 20, 20, 20, 24], 20, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real)
    ckks_mul(8192, [60, 40, 40, 60], 40, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real)
    ckks_mul(16384, [60, 50, 50, 50, 50, 50, 50, 60], 50, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real)
    ckks_mul(16384, [60, 40, 40, 40, 40, 40, 40, 40, 60], 40, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real)
    ckks_mul(32768, [60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60], 60, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real)
    ckks_mul(32768, [60, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 60], 40, n, data_decimal, mul_decimal, mul_times, data_randomNumber, mul_randomNumber, real)



