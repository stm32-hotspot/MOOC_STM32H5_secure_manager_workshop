import argparse
from Crypto.Cipher import AES
import binascii

# Hardcoded key
key_hex = '463B412911767D57A0B33969E674FFE7845D313B88C6FE312F3D724BE68E1FCA'

# AES CBC decryption function
def aes_cbc_decrypt(ciphertext, key, nonce):
    cipher = AES.new(key, AES.MODE_CBC, nonce)
    plaintext = cipher.decrypt(ciphertext)
    return plaintext

# Parse command line arguments
parser = argparse.ArgumentParser(description='Decrypt AES CBC encrypted buffer')
parser.add_argument('input', type=str, help='Concatenated nonce and ciphertext as hexadecimal string')
args = parser.parse_args()

input_hex = args.input.strip()
nonce_hex = input_hex[:32]
ciphertext_hex = input_hex[32:]

# Convert hexadecimal strings to bytes
ciphertext = bytes.fromhex(ciphertext_hex)
nonce = bytes.fromhex(nonce_hex)
key=bytes.fromhex(key_hex)

# Decrypt ciphertext using hardcoded key and provided nonce
plaintext = aes_cbc_decrypt(ciphertext, key, nonce)

ascii_plaintext = plaintext.decode('utf-8')
print(ascii_plaintext)
