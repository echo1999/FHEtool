import tenseal as ts
import numpy as np

def _almost_equal(vec1, vec2, m_pow_ten):
    if len(vec1) != len(vec2):
        return False

    upper_bound = pow(10, -m_pow_ten)
    for v1, v2 in zip(vec1, vec2):
        if abs(v1 - v2) > upper_bound:
            return False
    return True

def precision():
    return 1

def test_high_degree_polynomial(data, polynom, precision):
    # special context for higher depth
    context = ts.context(
        ts.SCHEME_TYPE.CKKS, 16384, coeff_mod_bit_sizes=[60, 40, 40, 40, 40, 40, 60]
    )
    context.global_scale = pow(2, 40)
    ct = ts.ckks_vector(context, data)
    expected = [np.polyval(polynom[::-1], x) for x in data]
    print("expected:", expected)
    result = ct.polyval(polynom)

    decrypted_result = result.decrypt()
    print("decrypted_result:", decrypted_result)
    if len(polynom) >= 13:
        # we allow greater error since some polynomial has terms with a high exponent
        error_tolerance = -1
    else:
        error_tolerance = 1
    assert _almost_equal(
        decrypted_result, expected, error_tolerance
    ), "Polynomial evaluation is incorrect."

mydata = [0,-1,1,-2,2]
# mypolynom = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1]
mypolynom = [0,0,0,0,1]
test_high_degree_polynomial(mydata, mypolynom, precision())