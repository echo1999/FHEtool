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

def generate_random_number(integer_digits, decimal_digits):
    # 生成整数部分
    integer_part = 0
    for i in range(integer_digits):
        integer_part = integer_part * 10 + random.randint(0, 9)

    # 生成小数部分
    decimal_part = 0.0
    for i in range(decimal_digits):
        decimal_part += float(random.randint(0, 9)) / pow(10, i + 1)

    # 组合整数和小数部分
    result = integer_part + decimal_part

    return fabs(result)  # 取绝对值确保非负数


n = 10 
for _ in range(10):
    random_number1 = generate_random_number(9, 2)
    random_number2 = generate_random_number(1, 0)
    print("random_number1:", random_number1)
    print("random_number2:", random_number2)
    real = random_number1 * random_number2 * random_number2
    print("real:", real)
    
    start_time1 = time.time()
    # enc_type = ts.ENCRYPTION_TYPE.ASYMMETRIC
    poly_mod = 16384
    # poly_mod = 65536
    modulus = 1152921504606584833

    # print(start_time)
    context = ts.context(
    scheme=ts.SCHEME_TYPE.BFV,
    poly_modulus_degree=poly_mod,
    plain_modulus = modulus,
    # encryption_type=enc_type,
    )

    random_number1 = random_number1 * 100
    random_number2 = random_number2 * 100
    vec1 = [random_number1]
    vec2 = [random_number2]

    bfv_vec1 = ts.bfv_vector(context, vec1)
    bfv_vec2 = ts.bfv_vector(context, vec2)

    bfv_vec1 = bfv_vec1 * bfv_vec2
    bfv_vec = bfv_vec1 * bfv_vec2

    dec_vec = bfv_vec.decrypt()
    res = dec_vec[0]/1000000
    print("res:", res)
    end_time1 = time.time()
    elapsed_time = end_time1 - start_time1
    print("time:",elapsed_time)


