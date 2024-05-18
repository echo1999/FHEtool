import sympy

def generate_prime_number(digits):
    lower_bound = 10**(digits - 1)
    upper_bound = 10**digits - 1
    prime = sympy.randprime(lower_bound, upper_bound)
    return prime

thirteen_digit_prime = generate_prime_number(13)
print("13位数的素数:", thirteen_digit_prime)