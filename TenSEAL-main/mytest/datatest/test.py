# !pip install tabulate

import tenseal as ts
import tenseal.sealapi as seal
import random
import pickle
import numpy as np
import math
from IPython.display import HTML, display
import tabulate
import pytest
 
def convert_size(size_bytes):
    if size_bytes == 0:
        return "0B"
    size_name = ("B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB")
    i = int(math.floor(math.log(size_bytes, 1024)))
    p = math.pow(1024, i)
    s = round(size_bytes / p, 2)
    return "%s %s" % (s, size_name[i])

enc_type_str = {
    ts.ENCRYPTION_TYPE.SYMMETRIC : "symmetric", 
    ts.ENCRYPTION_TYPE.ASYMMETRIC : "asymmetric",
}

scheme_str = {
    ts.SCHEME_TYPE.CKKS : "ckks", 
    ts.SCHEME_TYPE.BFV : "bfv",
}

def decrypt(enc):
    return enc.decrypt()

ctx_size_benchmarks = [["Encryption Type", "Scheme Type", "Polynomial modulus", "Coefficient modulus sizes", "Saved keys", "Context serialized size", ]]

for enc_type in [ts.ENCRYPTION_TYPE.SYMMETRIC, ts.ENCRYPTION_TYPE.ASYMMETRIC]:
    for (poly_mod, coeff_mod_bit_sizes) in [
        (8192, [40, 21, 21, 21, 21, 21, 21, 40]),
        (8192, [40, 20, 40]),
        (8192, [20, 20, 20]),
        (8192, [17, 17]),
        (4096, [40, 20, 40]),
        (4096, [30, 20, 30]),
        (4096, [20, 20, 20]),
        (4096, [19, 19, 19]),
        (4096, [18, 18, 18]),
        (4096, [18, 18]),
        (4096, [17, 17]),
        (2048, [20, 20]),
        (2048, [18, 18]),
        (2048, [16, 16]),
    ]:
        context = ts.context(
            scheme=ts.SCHEME_TYPE.CKKS,
            poly_modulus_degree=poly_mod,
            coeff_mod_bit_sizes=coeff_mod_bit_sizes,
            encryption_type=enc_type,
        )
        context.generate_galois_keys()
        context.generate_relin_keys()
        
        ser = context.serialize(save_public_key=True, save_secret_key=True, save_galois_keys=True, save_relin_keys=True)
        ctx_size_benchmarks.append([enc_type_str[enc_type], scheme_str[ts.SCHEME_TYPE.CKKS], poly_mod, coeff_mod_bit_sizes, "all", convert_size(len(ser))])
        
        if enc_type is ts.ENCRYPTION_TYPE.ASYMMETRIC:
            ser = context.serialize(save_public_key=True, save_secret_key=False, save_galois_keys=False, save_relin_keys=False)
            ctx_size_benchmarks.append([enc_type_str[enc_type], scheme_str[ts.SCHEME_TYPE.CKKS], poly_mod, coeff_mod_bit_sizes, "Public key", convert_size(len(ser))])
  
        ser = context.serialize(save_public_key=False, save_secret_key=True, save_galois_keys=False, save_relin_keys=False)
        ctx_size_benchmarks.append([enc_type_str[enc_type], scheme_str[ts.SCHEME_TYPE.CKKS], poly_mod, coeff_mod_bit_sizes, "Secret key",  convert_size(len(ser))])
                                    
        ser = context.serialize(save_public_key=False, save_secret_key=False, save_galois_keys=True, save_relin_keys=False)
        ctx_size_benchmarks.append([enc_type_str[enc_type], scheme_str[ts.SCHEME_TYPE.CKKS], poly_mod, coeff_mod_bit_sizes, "Galois keys",  convert_size(len(ser))])
                                                                      
        ser = context.serialize(save_public_key=False, save_secret_key=False, save_galois_keys=False, save_relin_keys=True)
        ctx_size_benchmarks.append([enc_type_str[enc_type], scheme_str[ts.SCHEME_TYPE.CKKS], poly_mod, coeff_mod_bit_sizes, "Relin keys",  convert_size(len(ser))])
  
        ser = context.serialize(save_public_key=False, save_secret_key=False, save_galois_keys=False, save_relin_keys=False)
        ctx_size_benchmarks.append([enc_type_str[enc_type], scheme_str[ts.SCHEME_TYPE.CKKS], poly_mod, coeff_mod_bit_sizes, "none", convert_size(len(ser))])
        
    for (poly_mod, coeff_mod_bit_sizes) in [
        (8192, [40, 21, 21, 21, 21, 21, 21, 40]),
        (8192, [40, 21, 21, 21, 21, 21, 40]),
        (8192, [40, 21, 21, 21, 21, 40]),
        (8192, [40, 21, 21, 21, 40]),
        (8192, [40, 21, 21, 40]),
        (8192, [40, 20, 40]),
        (4096, [40, 20, 40]),
        (4096, [30, 20, 30]),
        (4096, [20, 20, 20]),
        (4096, [19, 19, 19]),
        (4096, [18, 18, 18]),
        (2048, [20, 20]),
    ]:
        context = ts.context(
            scheme=ts.SCHEME_TYPE.BFV,
            poly_modulus_degree=poly_mod,
            plain_modulus=786433,
            coeff_mod_bit_sizes=coeff_mod_bit_sizes,
            encryption_type=enc_type,
        )
        
        context.generate_galois_keys()
        context.generate_relin_keys()
        
        ser = context.serialize(save_public_key=True, save_secret_key=True, save_galois_keys=True, save_relin_keys=True)
        ctx_size_benchmarks.append([enc_type_str[enc_type], scheme_str[ts.SCHEME_TYPE.BFV], poly_mod, coeff_mod_bit_sizes, "all", convert_size(len(ser))])
        
        if enc_type is ts.ENCRYPTION_TYPE.ASYMMETRIC:
            ser = context.serialize(save_public_key=True, save_secret_key=False, save_galois_keys=False, save_relin_keys=False)
            ctx_size_benchmarks.append([enc_type_str[enc_type], scheme_str[ts.SCHEME_TYPE.BFV], poly_mod, coeff_mod_bit_sizes, "Public key", convert_size(len(ser))])
  
        ser = context.serialize(save_public_key=False, save_secret_key=True, save_galois_keys=False, save_relin_keys=False)
        ctx_size_benchmarks.append([enc_type_str[enc_type], scheme_str[ts.SCHEME_TYPE.BFV], poly_mod, coeff_mod_bit_sizes, "Secret key",  convert_size(len(ser))])
                                    
        ser = context.serialize(save_public_key=False, save_secret_key=False, save_galois_keys=True, save_relin_keys=False)
        ctx_size_benchmarks.append([enc_type_str[enc_type], scheme_str[ts.SCHEME_TYPE.BFV], poly_mod, coeff_mod_bit_sizes, "Galois keys",  convert_size(len(ser))])
                                                                      
        ser = context.serialize(save_public_key=False, save_secret_key=False, save_galois_keys=False, save_relin_keys=True)
        ctx_size_benchmarks.append([enc_type_str[enc_type], scheme_str[ts.SCHEME_TYPE.BFV], poly_mod, coeff_mod_bit_sizes, "Relin keys",  convert_size(len(ser))])
  
        ser = context.serialize(save_public_key=False, save_secret_key=False, save_galois_keys=False, save_relin_keys=False)
        ctx_size_benchmarks.append([enc_type_str[enc_type], scheme_str[ts.SCHEME_TYPE.BFV], poly_mod, coeff_mod_bit_sizes, "none", convert_size(len(ser))])
        

display(HTML(tabulate.tabulate(ctx_size_benchmarks, tablefmt='html')))