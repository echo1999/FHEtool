import tenseal.sealapi as sealapi

def is_close_enough(out, expected):
    for idx in range(len(expected)):
        assert abs(expected[idx] - out[idx]) < 0.1
def helper_params_ckks(poly_modulus_degree=8192):
    parms = sealapi.EncryptionParameters(sealapi.SCHEME_TYPE.CKKS)
    parms.set_poly_modulus_degree(poly_modulus_degree)
    coeff = sealapi.CoeffModulus.Create(poly_modulus_degree, [60, 40, 40, 60])
    parms.set_coeff_modulus(coeff)

    return parms

def helper_context_ckks(poly_modulus_degree=8192):
    return sealapi.SEALContext(
        helper_params_ckks(poly_modulus_degree), True, sealapi.SEC_LEVEL_TYPE.TC128
    )
def helper_generate_evaluator(ctx):
    evaluator = sealapi.Evaluator(ctx)

    keygen = sealapi.KeyGenerator(ctx)
    public_key = sealapi.PublicKey()
    keygen.create_public_key(public_key)
    secret_key = keygen.secret_key()
    relin_keys = sealapi.RelinKeys()
    keygen.create_relin_keys(relin_keys)

    decryptor = sealapi.Decryptor(ctx, secret_key)
    encryptor = sealapi.Encryptor(ctx, public_key, secret_key)

    return evaluator, encryptor, decryptor, relin_keys

def helper_encode(scheme, ctx, test):
    plaintext = sealapi.Plaintext()
    if scheme == sealapi.SCHEME_TYPE.CKKS:
        encoder = sealapi.CKKSEncoder(ctx)
        encoder.encode(test, 2**40, plaintext)
    else:
        encoder = sealapi.BatchEncoder(ctx)
        encoder.encode(test, plaintext)
    return plaintext

def helper_decode(scheme, ctx, test):
    if scheme == sealapi.SCHEME_TYPE.CKKS:
        encoder = sealapi.CKKSEncoder(ctx)
        return encoder.decode_double(test)

    encoder = sealapi.BatchEncoder(ctx)
    return encoder.decode_int64(test)
# @pytest.mark.parametrize("scheme, ctx", [(sealapi.SCHEME_TYPE.CKKS, helper_context_ckks(8192))])
# @pytest.mark.parametrize("left", [[10, 100, 500, 600], [i for i in range(200)]])
def test_evaluator_rescale(scheme, ctx, left):
    evaluator, encryptor, _, relin_keys = helper_generate_evaluator(ctx)

    # cipher rescale to next in place
    cleft = sealapi.Ciphertext(ctx)
    cout = sealapi.Ciphertext(ctx)

    pleft = helper_encode(scheme, ctx, left)
    encryptor.encrypt(pleft, cleft)
    evaluator.square(cleft, cout)
    evaluator.relinearize_inplace(cout, relin_keys)

    before = cout.scale
    evaluator.rescale_to_next_inplace(cout)
    after = cout.scale

    assert after < before

    # cipher rescale to next
    cleft = sealapi.Ciphertext(ctx)
    cout = sealapi.Ciphertext(ctx)
    cfinal = sealapi.Ciphertext(ctx)

    pleft = helper_encode(scheme, ctx, left)
    encryptor.encrypt(pleft, cleft)
    evaluator.square(cleft, cout)
    evaluator.relinearize_inplace(cout, relin_keys)

    before = cout.scale
    evaluator.rescale_to_next(cout, cfinal)
    after = cfinal.scale

    assert after < before

    # cipher rescale to last in place
    cleft = sealapi.Ciphertext(ctx)
    cout = sealapi.Ciphertext(ctx)

    pleft = helper_encode(scheme, ctx, left)
    encryptor.encrypt(pleft, cleft)
    evaluator.square(cleft, cout)
    evaluator.relinearize_inplace(cout, relin_keys)

    before = cout.scale
    evaluator.rescale_to_inplace(cout, ctx.last_parms_id())
    after = cout.scale

    assert after < before

    # cipher rescale to last
    cleft = sealapi.Ciphertext(ctx)
    cout = sealapi.Ciphertext(ctx)
    cfinal = sealapi.Ciphertext(ctx)

    pleft = helper_encode(scheme, ctx, left)
    encryptor.encrypt(pleft, cleft)
    evaluator.square(cleft, cout)
    evaluator.relinearize_inplace(cout, relin_keys)

    before = cout.scale
    evaluator.rescale_to(cout, ctx.last_parms_id(), cfinal)
    after = cfinal.scale

    assert after < before

def test_rescale(scheme, ctx, left):
    # evaluator, encryptor, _, relin_keys = helper_generate_evaluator(ctx)
    evaluator, encryptor, decryptor, relin_keys = helper_generate_evaluator(ctx)
    
    # cipher rescale to next in place
    cleft = sealapi.Ciphertext(ctx)
    cout = sealapi.Ciphertext(ctx)

    pleft = helper_encode(scheme, ctx, left)
    encryptor.encrypt(pleft, cleft)
    evaluator.square(cleft, cout)
    # evaluator.relinearize_inplace(cout, relin_keys)

    out = sealapi.Plaintext()
    decryptor.decrypt(cout, out)
    out = helper_decode(scheme, ctx, out)
    print("out:", out)

    # evaluator.square(cleft, cout)
    # evaluator.relinearize_inplace(cout, relin_keys)

    # before = cout.scale
    evaluator.rescale_to_next_inplace(cout)



    # cipher rescale to next
    cleft = sealapi.Ciphertext(ctx)
    cout = sealapi.Ciphertext(ctx)
    cfinal = sealapi.Ciphertext(ctx)

    pleft = helper_encode(scheme, ctx, left)
    encryptor.encrypt(pleft, cleft)
    evaluator.square(cleft, cout)
    # cout = cleft * cleft
    evaluator.relinearize_inplace(cout, relin_keys)

    before = cout.scale
    # print("before scale:", before)
    evaluator.rescale_to_next(cout, cfinal)
    after = cfinal.scale
    # print("after scale:", after)

    # assert after < before

    # cipher rescale to last
    cleft = sealapi.Ciphertext(ctx)
    cout = sealapi.Ciphertext(ctx)
    cfinal = sealapi.Ciphertext(ctx)

    pleft = helper_encode(scheme, ctx, left)
    encryptor.encrypt(pleft, cleft)
    evaluator.square(cleft, cout)
    evaluator.relinearize_inplace(cout, relin_keys)

    before = cout.scale
    evaluator.rescale_to(cout, ctx.last_parms_id(), cfinal)
    after = cfinal.scale

    assert after < before

def test_evaluator_relin(scheme, ctx, left, right):
    evaluator, encryptor, decryptor, relin_keys = helper_generate_evaluator(ctx)

    expected = [left[idx] * right[idx] for idx in range(len(left))]

    # relin inplace
    cleft = sealapi.Ciphertext(ctx)
    cright = sealapi.Ciphertext(ctx)

    pleft = helper_encode(scheme, ctx, left)
    pright = helper_encode(scheme, ctx, right)

    encryptor.encrypt(pleft, cleft)
    encryptor.encrypt(pright, cright)

    evaluator.multiply_inplace(cleft, cright)
    assert cleft.size() > 2
    evaluator.relinearize_inplace(cleft, relin_keys)
    assert cleft.size() == 2

    out = sealapi.Plaintext()
    decryptor.decrypt(cleft, out)
    out = helper_decode(scheme, ctx, out)

    is_close_enough(out[: len(left)], expected)

    # relin
    cleft = sealapi.Ciphertext(ctx)
    cright = sealapi.Ciphertext(ctx)
    cout = sealapi.Ciphertext(ctx)

    pleft = helper_encode(scheme, ctx, left)
    pright = helper_encode(scheme, ctx, right)

    encryptor.encrypt(pleft, cleft)
    encryptor.encrypt(pright, cright)

    evaluator.multiply_inplace(cleft, cright)
    assert cleft.size() > 2
    evaluator.relinearize(cleft, relin_keys, cout)
    assert cout.size() == 2

    out = sealapi.Plaintext()
    decryptor.decrypt(cout, out)
    out = helper_decode(scheme, ctx, out)

    is_close_enough(out[: len(left)], expected)

scheme = sealapi.SCHEME_TYPE.CKKS
ctx = helper_context_ckks(8192)
left = [1, 2, 3, 4]
# test_evaluator_rescale(scheme, ctx, left)
test_rescale(scheme, ctx, left)