from flask import Flask, request, jsonify
import os
import random
import math
import subprocess

# from flask_compress import Compress
app = Flask(__name__)

# Compress(app)

def generate_random_number(integer_digits, decimal_digits):
    # Generate integer part
    integer_part = 0
    for i in range(integer_digits):
        integer_part = integer_part * 10 + random.randint(0, 9)

    # Generate decimal part
    decimal_part = sum(random.randint(0, 9) / math.pow(10, i + 1) for i in range(decimal_digits))

    # Combine integer and decimal parts
    result = integer_part + decimal_part

    return abs(result)  # Ensure non-negative value

@app.route('/process_data', methods=['POST'])
def process_data():
    data = request.get_json()
    library = data.get('library')
    method = data.get('method')
    data_integer = data.get('data_integer')
    data_decimal = data.get('data_decimal')
    mul_integer = data.get('mul_integer')
    mul_decimal = data.get('mul_decimal')
    mul_times = data.get('mul_times')

    n = 10
    data_random_number = [generate_random_number(data_integer, data_decimal) for _ in range(n)]
    mul_random_number = [generate_random_number(mul_integer, mul_decimal) for _ in range(n)]
    real = [data_random_number[i] * mul_random_number[i] ** mul_times for i in range(n)]

    with open("data.txt", "w") as out_file:
        out_file.write(f"random_length: {n}\n")
        out_file.write(f"data_integer: {data_integer}\n")
        out_file.write(f"data_decimal: {data_decimal}\n")
        out_file.write(f"mul_integer: {mul_integer}\n")
        out_file.write(f"mul_decimal: {mul_decimal}\n")
        out_file.write(f"mul_times: {mul_times}\n")

        out_file.write("data_randomNumber:\n")
        for num in data_random_number:
            out_file.write(f"{num:.{data_decimal+1}f}\n")

        out_file.write("mul_randomNumber:\n")
        for num in mul_random_number:
            out_file.write(f"{num:.{mul_decimal+1}f}\n")

        out_file.write("real:\n")
        for num in real:
            out_file.write(f"{num:.{data_decimal+mul_decimal+1}f}\n")

    os.system("chmod +rw data.txt")
    print("Random numbers and parameters have been written to data.txt")

    # Call the appropriate function based on the method
    if library == 1:
        result = run_seal(method)
    elif library == 2:
        result = run_tenseal(method)
    else :
        result = run_openfhe(method)
    # print("result:", result)
    return jsonify({"result": result})

def run_seal(method):
    # Create script file
    with open("script_seal.sh", "w") as script_file:
        script_file.write("#!/bin/bash\n")
        script_file.write("cd SEAL/native/mytest\n")
        script_file.write(f"cmake -DMETHOD={method} -S . -B build\n")
        script_file.write("cmake --build build\n")
        script_file.write("cd build/bin\n")
        script_file.write("./sealexamples\n")

    print("script has been written to script_seal.sh")
    # Execute script file
    subprocess.run(["chmod", "+x", "script_seal.sh"])
    # Execute script file and redirect output to a file
    subprocess.run(["./script_seal.sh"], stdout=open("result_seal.txt", "w"))
    print("Result has been written to result_seal.txt")
    
    # Read script execution result
    with open("result_seal.txt", "r") as result_file:
        result = result_file.read()
        

    return result

def run_tenseal(method):
    # Create script file
    with open("script_tenseal.sh", "w") as script_file:
        script_file.write("#!/bin/bash\n")
        script_file.write("cd TenSEAL-main/mytest/datatest/serve\n")
        if method == 2:
            script_file.write("python3 bfv_serve.py\n")
        elif method == 3:
            script_file.write("python3 ckks_serve.py\n")
        else:
            print("This algorithm is not included")

    # Execute script file
    subprocess.run(["chmod", "+x", "script_tenseal.sh"])
    # Execute script file and redirect output to a file
    subprocess.run(["./script_tenseal.sh"], stdout=open("result_tenseal.txt", "w"))

    # Read script execution result
    with open("result_tenseal.txt", "r") as result_file:
        result = result_file.read()

    return result

def run_openfhe(method):
    # Create script file
    with open("script_openfhe.sh", "w") as script_file:
        script_file.write("#!/bin/bash\n")
        script_file.write("cd openfhe-development/build\n")
        script_file.write("make\n")
        if method == 1:
            script_file.write("bin/examples/pke/simple-integers-bgvrns\n")
        elif method == 2:
            script_file.write("bin/examples/pke/simple-integers\n")
        elif method == 3:
            script_file.write("bin/examples/pke/simple-real-numbers\n")
        else:
            print("This algorithm is not included")

    # Execute script file
    subprocess.run(["chmod", "+x", "script_openfhe.sh"])
    # Execute script file and redirect output to a file
    subprocess.run(["./script_openfhe.sh"], stdout=open("result_openfhe.txt", "w"))

    # Read script execution result
    with open("result_openfhe.txt", "r") as result_file:
        result = result_file.read()

    return result

if __name__ == "__main__":
    # app.run(host='0.0.0.0',port=5051)
    app.run(debug=True)
