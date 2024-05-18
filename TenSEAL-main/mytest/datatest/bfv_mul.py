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

start_time1 = time.time()
# enc_type = ts.ENCRYPTION_TYPE.ASYMMETRIC
poly_mod = 32768
# poly_mod = 65536
modulus = 1152921504606584833

# print(start_time)
context = ts.context(
scheme=ts.SCHEME_TYPE.BFV,
poly_modulus_degree=poly_mod,
plain_modulus = modulus,
# encryption_type=enc_type,
)


max_multiplications = 12
maxtimes = 10
# bfv_num = ts.bfv_vector(context, num, scale)
vec = [0]
j = 103
# for j in range(2, maxtimes + 1):
#     print("x:",j)
    # flag = 0
vec[0] = j
real = j
bfv_vec = ts.bfv_vector(context, vec)
bfv_vec2 = ts.bfv_vector(context, vec)
for i in range(1, max_multiplications):
    bfv_vec = bfv_vec * bfv_vec2
    dec_vec = bfv_vec.decrypt()
    print("dec_vec:", dec_vec[0])

    end_time1 = time.time()
    elapsed_time = end_time1 - start_time1
    print("time:",elapsed_time)

    # real = real * j
    # print("real:", real)
    # epsilon = 0.1
    # for dec_prec in range(40):
    #     epsilon *= 0.1
    #     if approx_equal(dec_vec[0], real, epsilon):
    #         if dec_prec == 0:
    #             print(f"Decryption failed after {i} multiplications.")
    #             flag = 1
    #             break
    #         break

        # if flag == 1:
        #     break

