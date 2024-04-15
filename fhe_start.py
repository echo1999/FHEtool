import tkinter as tk
from tkinter import ttk
from functools import partial
import requests
import re
import json

def run_seal(method, data_integer, data_decimal, mul_integer, mul_decimal, mul_times):
    # 发送请求
    # print("data_integer:", data_integer.get())
    data = {
        'library' : 1,
        'method': method,
        'data_integer': int(data_integer.get()),
        'data_decimal': int(data_decimal.get()),
        'mul_integer': int(mul_integer.get()),
        'mul_decimal': int(mul_decimal.get()),
        'mul_times': int(mul_times.get())
    }

    # 发送请求并获取响应
    response = requests.post('http://120.48.153.43:5000/process_data', json=data, timeout=1000)

    if response.status_code == 200 and method != 3:  
        result = response.json()
        result = result['result']
        # print("result:", result)

        # 定义正则表达式模式
        pattern = r"poly_modulus_degree: (\d+)\n.*?coeff_modulus size: (\d+).*?plain_modulus: (\d+)\n.*?time_avg:([\d.]+)ms.*?prec_avg:(\d+)"
        # 使用正则表达式进行匹配
        matches = re.findall(pattern, result, re.DOTALL)
        # print("matches:", matches)

        # 新建五个数组
        poly_modulus_degree_list = []
        coeff_modulus_size_list = []
        plain_modulus_list = []
        time_avg_list = []
        prec_avg_list = []

        # 提取的结果存入数组
        for match in matches:
            poly_modulus_degree, coeff_modulus_size, plain_modulus, time_avg, prec_avg = match
            poly_modulus_degree_list.append(int(poly_modulus_degree))
            coeff_modulus_size_list.append(int(coeff_modulus_size))
            plain_modulus_list.append(int(plain_modulus))
            time_avg_list.append(float(time_avg))
            prec_avg_list.append(float(prec_avg))
        
        # 打印结果
        if method == 1:
            result_text.insert(tk.END, "运行 SEAL BGV...\n")
        else:
            result_text.insert(tk.END, "运行 SEAL BFV...\n")
        result_text.update_idletasks()
        
        length = len(poly_modulus_degree_list)
        for k in range(length):
            result_text.insert(tk.END, "第" + str(k+1) )
            result_text.insert(tk.END, "组：\n")
            result_text.insert(tk.END, "poly_modulus_degree_list:" + str(poly_modulus_degree_list[k]) + "\n")
            result_text.insert(tk.END, "coeff_modulus_size_list:" + str(coeff_modulus_size_list[k]) + "\n")
            result_text.insert(tk.END, "plain_modulus_list:" + str(plain_modulus_list[k]) + "\n")
            result_text.insert(tk.END, "time_avg_list:" + str(time_avg_list[k]) + "\n")
            result_text.insert(tk.END, "prec_avg_list:" + str(prec_avg_list[k]) + "\n")
            result_text.update_idletasks()
        result_text.insert(tk.END, "----------\n")
        result_text.update_idletasks()

    # 检查响应状态码
    if response.status_code == 200 and method == 3:
        result = response.json()
        result = result['result']
        # print("result:", result)
        # 正则表达式模式
        pattern = r"poly_modulus_degree: (\d+)\n.*?coeff_modulus size: (\d+).*?scale:2\^(\d+).*?time_avg:([\d.]+)ms.*?prec_avg:(\d+)"
        matches = re.findall(pattern, result, re.DOTALL)

        # 新建五个数组
        poly_modulus_degree_list = []
        coeff_modulus_size_list = []
        scale_exp_list = []
        time_avg_list = []
        prec_avg_list = []

        # 提取的结果存入数组
        for match in matches:
            poly_modulus_degree, coeff_modulus_size, scale_exp, time_avg, prec_avg = match
            poly_modulus_degree_list.append(int(poly_modulus_degree))
            coeff_modulus_size_list.append(int(coeff_modulus_size))
            scale_exp_list.append(int(scale_exp))
            time_avg_list.append(float(time_avg))
            prec_avg_list.append(float(prec_avg))

        result_text.insert(tk.END, "运行 SEAL CKKS...\n")
        result_text.update_idletasks()

        length = len(poly_modulus_degree_list)
        for k in range(length):
            result_text.insert(tk.END, "第" + str(k+1) )
            result_text.insert(tk.END, "组：\n")
            result_text.insert(tk.END, "poly_modulus_degree:" + str(poly_modulus_degree_list[k]) + "\n")
            result_text.insert(tk.END, "coeff_modulus_size:" + str(coeff_modulus_size_list[k]) + "\n")
            result_text.insert(tk.END, "scale_exp:" + str(scale_exp_list[k]) + "\n")
            result_text.insert(tk.END, "time_avg:" + str(time_avg_list[k]) + "\n")
            result_text.insert(tk.END, "prec_avg:" + str(prec_avg_list[k]) + "\n")
            result_text.update_idletasks()
        result_text.insert(tk.END, "----------\n")
        result_text.update_idletasks()

def run_tenseal(method, data_integer, data_decimal, mul_integer, mul_decimal, mul_times):
    # 发送请求
    # print("method:", method)
    data = {
        'library' : 2,
        'method': method,
        'data_integer': int(data_integer.get()),
        'data_decimal': int(data_decimal.get()),
        'mul_integer': int(mul_integer.get()),
        'mul_decimal': int(mul_decimal.get()),
        'mul_times': int(mul_times.get())
    }

    # 发送请求并获取响应
    response = requests.post('http://120.48.153.43:5000/process_data', json=data, timeout=1000)

    if response.status_code == 200 and method == 2:  
        result = response.json()
        result = result['result']
        # print("result:", result)

        # 使用正则表达式匹配数据并存入数组
        poly_modulus_degree_list = re.findall(r'poly_modulus_degree: (\d+)', result)
        plain_modulus_list = re.findall(r'plain_modulus: (\d+)', result)
        time_avg_list = re.findall(r'time_avg: ([\d.]+)', result)
        prec_avg_list = re.findall(r'prec_avg: ([\d.]+)', result)

        result_text.insert(tk.END, "运行 TenSEAL BFV...\n")
        result_text.update_idletasks()

        length = len(poly_modulus_degree_list)
        for k in range(length):
            result_text.insert(tk.END, "第" + str(k+1) )
            result_text.insert(tk.END, "组：\n")
            result_text.insert(tk.END, "poly_modulus_degree:" + str(poly_modulus_degree_list[k]) + "\n")
            result_text.insert(tk.END, "plain_modulus:" + str(plain_modulus_list[k]) + "\n")
            result_text.insert(tk.END, "time_avg:" + str(time_avg_list[k]) + "\n")
            result_text.insert(tk.END, "prec_avg:" + str(prec_avg_list[k]) + "\n")
            result_text.update_idletasks()
        result_text.insert(tk.END, "----------\n")
        result_text.update_idletasks()

    if response.status_code == 200 and method == 3:  
        result = response.json()
        result = result['result']
        # print("result:", result)

        # 使用正则表达式匹配数据并存入数组
        poly_modulus_degree_list = re.findall(r'poly_modulus_degree: (\d+)', result)
        coeff_mod_bit_sizes_list = re.findall(r'coeff_mod_bit_sizes: \[([\d, ]+)\]', result)
        scale_exp_list = re.findall(r'scale:2\^ (\d+)', result)
        time_avg_list = re.findall(r'time_avg: ([\d.]+)', result)
        prec_avg_list = re.findall(r'prec_avg: ([\d.]+)', result)

        # 将coeff_mod_bit_sizes_list中的字符串转换为整数列表
        coeff_mod_bit_sizes_list = [[int(bit) for bit in bits.split(', ')] for bits in coeff_mod_bit_sizes_list]

        result_text.insert(tk.END, "运行 TenSEAL CKKS...\n")
        result_text.update_idletasks()

        length = len(poly_modulus_degree_list)
        for k in range(length):
            result_text.insert(tk.END, "第" + str(k+1) )
            result_text.insert(tk.END, "组：\n")
            result_text.insert(tk.END, "poly_modulus_degree:" + str(poly_modulus_degree_list[k]) + "\n")
            result_text.insert(tk.END, "coeff_modulus_size:" + str(coeff_mod_bit_sizes_list[k]) + "\n")
            result_text.insert(tk.END, "scale_exp:" + str(scale_exp_list[k]) + "\n")
            result_text.insert(tk.END, "time_avg:" + str(time_avg_list[k]) + "\n")
            result_text.insert(tk.END, "prec_avg:" + str(prec_avg_list[k]) + "\n")
            result_text.update_idletasks()
        result_text.insert(tk.END, "----------\n")
        result_text.update_idletasks()

        # 打印结果
        # print("poly_modulus_degree_list:", poly_modulus_degree_list)
        # print("coeff_mod_bit_sizes_list:", coeff_mod_bit_sizes_list)
        # print("scale_exp_list:", scale_exp_list)
        # print("time_avg_list:", time_avg_list)
        # print("prec_avg_list:", prec_avg_list)

def run_openfhe(method, data_integer, data_decimal, mul_integer, mul_decimal, mul_times):
    # 发送请求
    # print("method:", method)
    data = {
        'library' : 3,
        'method': method,
        'data_integer': int(data_integer.get()),
        'data_decimal': int(data_decimal.get()),
        'mul_integer': int(mul_integer.get()),
        'mul_decimal': int(mul_decimal.get()),
        'mul_times': int(mul_times.get())
    }

    # 发送请求并获取响应
    response = requests.post('http://120.48.153.43:5000/process_data', json=data, timeout=10000)

    if response.status_code == 200 and method != 3:  
        result = response.json()
        result = result['result']
        # print("result:", result)

        # 使用正则表达式匹配数据并存入数组
        PlaintextModulus_list = re.findall(r'PlaintextModulus:(\d+)', result)
        MultiplicativeDepth_list = re.findall(r'MultiplicativeDepth:(\d+)', result)
        time_avg_list = re.findall(r'time_avg:([\d.]+)ms', result)
        prec_avg_list = re.findall(r'prec_avg:([\d.]+)', result)

        # 打印结果
        if method == 1:
            result_text.insert(tk.END, "运行 OpenFHE BGV...\n")
        else:
            result_text.insert(tk.END, "运行 OpenFHE BFV...\n")
        result_text.update_idletasks()

        length = len(PlaintextModulus_list)
        for k in range(length):
            result_text.insert(tk.END, "第" + str(k+1) )
            result_text.insert(tk.END, "组：\n")
            result_text.insert(tk.END, "PlaintextModulus:" + str(PlaintextModulus_list[k]) + "\n")
            result_text.insert(tk.END, "MultiplicativeDepth:" + str(MultiplicativeDepth_list[k]) + "\n")
            result_text.insert(tk.END, "time_avg:" + str(time_avg_list[k]) + "\n")
            result_text.insert(tk.END, "prec_avg:" + str(prec_avg_list[k]) + "\n")
            result_text.update_idletasks()
        result_text.insert(tk.END, "----------\n")
        result_text.update_idletasks()

        # 打印结果
        # print("PlaintextModulus_list:", PlaintextModulus_list)
        # print("MultiplicativeDepth_list:", MultiplicativeDepth_list)
        # print("time_avg_list:", time_avg_list)
        # print("prec_avg_list:", prec_avg_list)

    if response.status_code == 200 and method == 3:  
        result = response.json()
        result = result['result']
        # print("result:", result)

        # 使用正则表达式匹配数据并存入数组
        ScalingModSize_list = re.findall(r'ScalingModSize:(\d+)', result)
        MultiplicativeDepth_list = re.findall(r'MultiplicativeDepth:(\d+)', result)
        time_avg_list = re.findall(r'time_avg:([\d.]+)ms', result)
        prec_avg_list = re.findall(r'prec_avg:([\d.]+)', result)

        result_text.insert(tk.END, "运行 OpenFHE CKKS...\n")
        result_text.update_idletasks()
        
        length = len(ScalingModSize_list)
        for k in range(length):
            result_text.insert(tk.END, "第" + str(k+1) )
            result_text.insert(tk.END, "组：\n")
            result_text.insert(tk.END, "ScalingModSize:" + str(ScalingModSize_list[k]) + "\n")
            result_text.insert(tk.END, "MultiplicativeDepth:" + str(MultiplicativeDepth_list[k]) + "\n")
            result_text.insert(tk.END, "time_avg:" + str(time_avg_list[k]) + "\n")
            result_text.insert(tk.END, "prec_avg:" + str(prec_avg_list[k]) + "\n")
            result_text.update_idletasks()
        result_text.insert(tk.END, "----------\n")
        result_text.update_idletasks()

def run_selected_algorithms(method_vars,data_integer,data_decimal,mul_integer,mul_decimal,mul_times):
    selected_methods = []
    if method_vars[0].get() == 1:
        selected_methods.append("SEAL")
    if method_vars[1].get() == 2:
        selected_methods.append("TenSEAL")
    if method_vars[2].get() == 3:
        selected_methods.append("OpenFHE")
    
    result_text.delete(1.0, tk.END)
    for method in selected_methods:
        if method == "SEAL":
            result_text.insert(tk.END, "运行 SEAL...\n")
            result_text.update_idletasks()
            run_seal(1, data_integer,data_decimal,mul_integer,mul_decimal,mul_times)
            run_seal(2, data_integer,data_decimal,mul_integer,mul_decimal,mul_times)
            run_seal(3, data_integer,data_decimal,mul_integer,mul_decimal,mul_times)
            
        elif method == "TenSEAL":
            result_text.insert(tk.END, "运行 TenSEAL...\n")
            run_tenseal(2, data_integer,data_decimal,mul_integer,mul_decimal,mul_times)
            run_tenseal(3, data_integer,data_decimal,mul_integer,mul_decimal,mul_times)

        elif method == "OpenFHE":
            result_text.insert(tk.END, "运行 OpenFHE...\n")
            run_openfhe(1, data_integer,data_decimal,mul_integer,mul_decimal,mul_times)
            run_openfhe(2, data_integer,data_decimal,mul_integer,mul_decimal,mul_times)
            run_openfhe(3, data_integer,data_decimal,mul_integer,mul_decimal,mul_times)

    # result_text.insert(tk.END, "所有算法均执行完毕\n")


def create_gui():
    root = tk.Tk()
    root.title("全同态加密工具")

    method_label = ttk.Label(root, text="算法库:")
    method_label.grid(row=0, column=0, sticky="w")

    method_vars = []
    methods = [("SEAL", 1), ("TenSEAL", 2), ("OpenFHE", 3)]
    for idx, (text, value) in enumerate(methods):
        var = tk.IntVar()
        cb = ttk.Checkbutton(root, text=text, variable=var, onvalue=value, offvalue=0)
        cb.grid(row=0, column=idx+1)
        method_vars.append(var)
    

    input_width = 25
    
    data_integer_label = ttk.Label(root, text="数据整数位数:")
    data_integer_label.grid(row=1, column=0, sticky="w")
    data_integer = ttk.Entry(root, width=input_width)
    data_integer.grid(row=1, column=1, columnspan=4)

    data_decimal_label = ttk.Label(root, text="数据小数位数:")
    data_decimal_label.grid(row=2, column=0, sticky="w")
    data_decimal = ttk.Entry(root, width=input_width)
    data_decimal.grid(row=2, column=1, columnspan=4)

    mul_integer_label = ttk.Label(root, text="乘数整数位数:")
    mul_integer_label.grid(row=3, column=0, sticky="w")
    mul_integer = ttk.Entry(root, width=input_width)
    mul_integer.grid(row=3, column=1, columnspan=4)

    mul_decimal_label = ttk.Label(root, text="乘数小数位数:")
    mul_decimal_label.grid(row=4, column=0, sticky="w")
    mul_decimal = ttk.Entry(root, width=input_width)
    mul_decimal.grid(row=4, column=1, columnspan=4)

    mul_times_label = ttk.Label(root, text="乘法次数:")
    mul_times_label.grid(row=5, column=0, sticky="w")
    mul_times = ttk.Entry(root, width=input_width)
    mul_times.grid(row=5, column=1, columnspan=4)

    run_button = tk.Button(root, text="运行", command=lambda: run_selected_algorithms(method_vars,data_integer,data_decimal,mul_integer,mul_decimal,mul_times))
    run_button.grid(row=7, column=0, columnspan=4, padx=5, pady=5)

    # Result Text
    global result_text
    result_text = tk.Text(root, height=10, width=50)
    result_text.grid(row=8, column=0, columnspan=4, padx=5, pady=5)

    root.mainloop()

if __name__ == "__main__":
    create_gui()