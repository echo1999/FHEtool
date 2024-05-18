import csv
import tenseal as ts
import pytest
from IPython.display import HTML, display
import tabulate
import time

# 从csv中读取数据
data_len = 20
with open('/echo-project/ckks/TenSEAL-main/mytest/gradients.csv') as csvfile:
    reader = csv.reader(csvfile)
    header = next(reader) #跳过第一行
    count = 0
    data = []
    for row in reader:
        if count < data_len :
            # 去除数据中的符号
            temp = row[1].replace('[','')
            temp = temp.replace(']','')
            # print("count:", count)
            data.append(float(temp))
            count = count + 1
print(data)
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
# 一、实现使用一组参数加密求和 -> 二、研究如何除以data.len -> 三、使用一组参数加密求和研究如何控制变量设置参数 -> 四、记录运行时间、精度 -> 五、选择最佳结果
# 一、实现使用一组参数加密
# 实例化context -> 使用data数组创建ckks_vec数组 -> 对ckks_vec数组求和ckks_sum-> 解密sum，计算data_sum，并对比ckks_sum与data_sum的差距
# 二、研究如何除以data.len
# 查看测试函数、exapmle，确认是否需要加密data.len，计算sum/data.len

# 实例化context
# poly_mod = 8192
# coeff_mod_bit_sizes = [60, 40, 40, 60]
enc_type = ts.ENCRYPTION_TYPE.ASYMMETRIC
# prec = 40
# ct_size_benchmarks = [["Value range", "Polynomial modulus", "Coefficient modulus sizes", "Precision", "Operation", "Status"]]
ct_size_benchmarks = [["Polynomial modulus", "Coefficient modulus sizes", "Scale", "Operation", "Status", "Time"]]

for (poly_mod, coeff_mod_bit_sizes, prec) in [
    (8192, [60, 40, 40, 60], 40),
    (8192, [40, 21, 21, 40], 40),
    (8192, [40, 21, 21, 40], 21),
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
    # print(start_time)
    context = ts.context(
    scheme=ts.SCHEME_TYPE.CKKS,
    poly_modulus_degree=poly_mod,
    coeff_mod_bit_sizes=coeff_mod_bit_sizes,
    encryption_type=enc_type,
    )
    scale = 2 ** prec
    # 使用data数组创建ckks_vec数组
    ckks_vec = []
    zero = [0]
    div = [1/data_len]
    ckks_sum = ts.ckks_vector(context, zero, scale)
    ckks_div = ts.ckks_vector(context, div, scale)
    i = 0
    start_time1 = time.time()
    try:
        while i < data_len:
            ckks_vec.append(ts.ckks_vector(context, data[i], scale))
            ckks_sum = ckks_sum + ckks_vec[i]
            i = i + 1
    except:
            ct_size_benchmarks.append([poly_mod, coeff_mod_bit_sizes, "2**{}".format(prec), "sum", "failed", "/"])
            continue
    dec_sum = ckks_sum.decrypt()
    end_time1 = time.time()
    for dec_prec in reversed(range(prec)):
        if pytest.approx(dec_sum, abs=2 ** -dec_prec) == [re_sum]:
            ct_size_benchmarks.append([poly_mod, coeff_mod_bit_sizes, "2**{}".format(prec), "sum",  "decryption prec 2 ** {}".format(-dec_prec), format(end_time1 - start_time1, '.3f') + "s"])
            break
    # 求平均
    start_time2 = time.time()
    try:
         ckks_avg = ckks_sum * ckks_div
    except:
            ct_size_benchmarks.append([poly_mod, coeff_mod_bit_sizes, "2**{}".format(prec), "mul", "failed", "/"])
            continue
    dec_avg = ckks_avg.decrypt()
    end_time2 = time.time()
    for dec_prec in reversed(range(prec)):
        if pytest.approx(dec_avg, abs=2 ** -dec_prec) == [re_sum/data_len]:
            ct_size_benchmarks.append([poly_mod, coeff_mod_bit_sizes, "2**{}".format(prec), "mul",  "decryption prec 2 ** {}".format(-dec_prec), format(end_time2 - start_time2, '.3f') + "s"])
            break

print(tabulate.tabulate(ct_size_benchmarks))

