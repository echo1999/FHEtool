import csv
import tenseal as ts
import pytest
from IPython.display import HTML, display
import tabulate
import time

def approx_equal(a, b, epsilon):
    # print("std::abs(a-b)", abs(a-b))
    # print("epsilon", epsilon)
    return abs(a - b) > epsilon

enc_type = ts.ENCRYPTION_TYPE.ASYMMETRIC
# prec = 40
# ct_size_benchmarks = [["Value range", "Polynomial modulus", "Coefficient modulus sizes", "Precision", "Operation", "Status"]]
# ct_size_benchmarks = [["Polynomial modulus", "Coefficient modulus sizes", "Scale", "Operation", "Status", "Time"]]
start_time1 = time.time()
for (poly_mod, coeff_mod_bit_sizes, prec) in [
    (32768, [60, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 60], 40),
]:
    # print(start_time)
    context = ts.context(
    scheme=ts.SCHEME_TYPE.CKKS,
    poly_modulus_degree=poly_mod,
    coeff_mod_bit_sizes=coeff_mod_bit_sizes,
    encryption_type=enc_type,
    )
    scale = 2 ** prec
#     # 使用data数组创建ckks_vec数组
#     ckks_vec = []
#     div = [1/data_len]
    
    max_multiplications = 8
    # maxtimes = 256
    # # ckks_num = ts.ckks_vector(context, num, scale)
    vec = [0]
    j = 3
    # for j in range(2, maxtimes + 1):
    #     print("x:",j)
    #     flag = 0
    vec[0] = j
    real = j
    ckks_vec = ts.ckks_vector(context, vec, scale)
    ckks_vec2 = ts.ckks_vector(context, vec, scale)
    for i in range(1, max_multiplications):
        print("i:",i)
        ckks_vec = ckks_vec * ckks_vec2
        dec_vec = ckks_vec.decrypt()
        print("dec_vec:", dec_vec[0])
        end_time1 = time.time()
        elapsed_time = end_time1 - start_time1
        print("time:",elapsed_time)

        # real = real * j
        # print("real:", real)
        # epsilon = 1
        # for dec_prec in range(scale):
        #     epsilon *= 0.1
        #     if approx_equal(dec_vec[0], real, epsilon):
        #         if dec_prec == 0:
        #             print(f"Decryption failed after {i} multiplications.")
        #             flag = 1
        #             break
        #         break

        # if flag == 1:
        #     break
#     ckks_div = ts.ckks_vector(context, div, scale)
#     i = 0
#     start_time1 = time.time()
#     try:
#         while i < data_len:
#             ckks_vec.append(ts.ckks_vector(context, data[i], scale))
#             ckks_sum = ckks_sum + ckks_vec[i]
#             i = i + 1
#     except:
#             ct_size_benchmarks.append([poly_mod, coeff_mod_bit_sizes, "2**{}".format(prec), "sum", "failed", "/"])
#             continue
#     dec_sum = ckks_sum.decrypt()
#     end_time1 = time.time()
#     for dec_prec in reversed(range(prec)):
#         if pytest.approx(dec_sum, abs=2 ** -dec_prec) == [re_sum]:
#             ct_size_benchmarks.append([poly_mod, coeff_mod_bit_sizes, "2**{}".format(prec), "sum",  "decryption prec 2 ** {}".format(-dec_prec), format(end_time1 - start_time1, '.3f') + "s"])
#             break
#     # 求平均
#     start_time2 = time.time()
#     try:
#          ckks_avg = ckks_sum * ckks_div
#     except:
#             ct_size_benchmarks.append([poly_mod, coeff_mod_bit_sizes, "2**{}".format(prec), "mul", "failed", "/"])
#             continue
#     dec_avg = ckks_avg.decrypt()
#     end_time2 = time.time()
#     for dec_prec in reversed(range(prec)):
#         if pytest.approx(dec_avg, abs=2 ** -dec_prec) == [re_sum/data_len]:
#             ct_size_benchmarks.append([poly_mod, coeff_mod_bit_sizes, "2**{}".format(prec), "mul",  "decryption prec 2 ** {}".format(-dec_prec), format(end_time2 - start_time2, '.3f') + "s"])
#             break

# print(tabulate.tabulate(ct_size_benchmarks))

