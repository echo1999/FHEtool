from flask import Flask, request, jsonify
import tenseal as ts
from flask_compress import Compress
app = Flask(__name__)

Compress(app)

POLY_MODULUS_DEGREE = 8192
COEFF_MOD_BIT_SIZES = [60, 40, 40, 60]
SCALE = pow(2, 40)

@app.route('/init', methods=['GET','POST'])
def init( 
        # poly_modulus_degree = None, 
        # coeff_mod_bit_sizes = None
        ):
    data_init = request.json
    if data_init is not None:
        poly_modulus_degree = data_init.get('poly_modulus_degree')
        coeff_mod_bit_sizes = data_init.get('coeff_mod_bit_sizes')
        if poly_modulus_degree is None:
            poly_modulus_degree = POLY_MODULUS_DEGREE
        if coeff_mod_bit_sizes is None:
            coeff_mod_bit_sizes = COEFF_MOD_BIT_SIZES
    else:
        print("start else")
        poly_modulus_degree = POLY_MODULUS_DEGREE
        coeff_mod_bit_sizes = COEFF_MOD_BIT_SIZES
    context = ts.context(
        ts.SCHEME_TYPE.CKKS,
        poly_modulus_degree,
        -1,
        coeff_mod_bit_sizes
        # encryption_type=ts.ENCRYPTION_TYPE.ASYMMETRIC,
    )
    
    # 将相应的context对象包装为秘钥，序列化后转换为字符串
    pk_serialized = context.serialize(save_public_key=True, save_secret_key=False, save_relin_keys=False).hex()
    sk_serialized = context.serialize(save_public_key=False, save_secret_key=True, save_relin_keys=False).hex()
    evk_serialized = context.serialize(save_public_key=False, save_secret_key=False, save_relin_keys=True).hex()
    # pk = ts.context_from(pk_proto)
    # sk = ts.context_from(sk_proto)
    # evk = ts.context_from(evk_proto)
    result = {
        'pk': pk_serialized,
        'sk': sk_serialized,
        'evk': evk_serialized
    }
#    return 'finish'   
    return jsonify(result)    

# 加密
@app.route('/encrypt', methods=['POST'])
def encrypt(
    # plain_vec = None,
    # public_key = None,
    # scale = None
):
    data_encrypt = request.json
    plain_vec = data_encrypt['plain_vec']
    public_key = data_encrypt['public_key']
    scale = data_encrypt.get('scale')
    # print("plain_vec:", plain_vec)
    context = ts.context_from(bytes.fromhex(public_key))
    ciphertext = ts.ckks_vector(context, plain_vec, scale)

    # 将字节对象转换为字符串
    ciphertext_serialized = ciphertext.serialize().hex()
    result = {
        'ciphertext': ciphertext_serialized
    }

    return jsonify(result)

# 解密
@app.route('/decrypt', methods=['POST'])
def decrypt():
    data_decrypt = request.json
    ciphertext = data_decrypt['ciphertext']
    secret_key = data_decrypt['secret_key']
    # print("plain_vec:", plain_vec)

    context = ts.context_from(bytes.fromhex(secret_key))
#    secret_key = secret_key.encode('utf-8')
    print("type(secret_key):", type(secret_key))
#    print("secret_key:", secret_key)
#    context = ts.context_from(secret_key)

    # 将ciphertext转换回bytes
    ciphertext_serialized = bytes.fromhex(ciphertext)
    # 将ciphertext反序列化
    ciphertext = ts.ckks_vector_from(context, ciphertext_serialized)

    secret_key = context.secret_key()
    decrypted_vec = ciphertext.decrypt(secret_key)

    result = {
        'decrypted_vec' : decrypted_vec
    }

    return jsonify(result)

# 运算
@app.route('/operation', methods=['POST'])
def operation():
    data_operation = request.json
    operand1 = data_operation['operand1']
    operand2 = data_operation['operand2']
    evaluation_key = data_operation['evaluation_key']
    operation_type = data_operation['operation_type']

    context = ts.context_from(bytes.fromhex(evaluation_key))
    # 将operand转换回bytes
    operand1_serialized = bytes.fromhex(operand1)
    operand2_serialized = bytes.fromhex(operand2)

    # 将operand反序列化
    operand1 = ts.ckks_vector_from(context, operand1_serialized)
    operand2 = ts.ckks_vector_from(context, operand2_serialized)

    if operation_type == 'add':
        operation_result = operand1 + operand2
    if operation_type == 'substract':
        operation_result = operand1 - operand2
    if operation_type == 'multiply':
        print("multiply")
        operation_result = operand1 * operand2

    # #测试 在此处解密解密
    # secret_key = data_operation['secret_key']
    # sk = ts.context_from(bytes.fromhex(secret_key))   
    # sk = sk.secret_key()
    # operand1 =  operand1.decrypt(sk)
    # operand2 =  operand2.decrypt(sk)
    # res = operation_result.decrypt(sk)
    # print("operand1:",operand1)
    # print("operand2:",operand2)
    # print("res:",res)

    # 将字节对象转换为字符串
    operation_result = operation_result.serialize().hex()
    result = {
        'operation_result': operation_result
    }
    return jsonify(result)
#    return 'finish'

if __name__ == '__main__':
    app.run(debug=True)
