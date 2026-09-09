import tkinter as tk
from Crypto.Cipher import AES
import binascii

key_hex_default = '463B412911767D57A0B33969E674FFE7845D313B88C6FE312F3D724BE68E1FCA'

# AES CBC decryption function
def aes_cbc_decrypt(ciphertext, key, nonce):
    cipher = AES.new(key, AES.MODE_CBC, nonce)
    plaintext = cipher.decrypt(ciphertext)
    return plaintext

# Decrypt input and update output label
def decrypt_input():
    # Get input from input field
    input_hex = input_field.get('1.0', 'end-1c').strip()

    # Split input into nonce and ciphertext
    nonce_hex = input_hex[:32]
    ciphertext_hex = input_hex[32:]

    # Convert hexadecimal strings to bytes
    nonce = bytes.fromhex(nonce_hex)
    ciphertext = bytes.fromhex(ciphertext_hex)
    key = bytes.fromhex(key_field.get('1.0', 'end-1c'))

    # Decrypt ciphertext using provided key and nonce
    plaintext = aes_cbc_decrypt(ciphertext, key, nonce)

    try:
        # Convert plaintext to ASCII string
        ascii_plaintext = plaintext.decode('utf-8')
    except UnicodeDecodeError as e:
        # Display error message if decoding fails
        ascii_plaintext = f'Error: {e}'

    # Update output label
    output_label.config(text=ascii_plaintext)
	# Add border around output label
    output_label.config(borderwidth=2, relief='groove')

# Create main window
root = tk.Tk()
root.title('STM32H5 Workshop AES CBC Decrypt')

# Create input field
input_label = tk.Label(root, text='Input (nonce + ciphertext):')
input_label.pack()
input_field = tk.Text(root, height=2)
input_field.pack()

# Create key field
key_label = tk.Label(root, text='Key:')
key_label.pack()
key_field = tk.Text(root, height=1)
key_field.insert(1.0, key_hex_default);
key_field.pack()

# Create decrypt button
decrypt_button = tk.Button(root, text='Decrypt', command=decrypt_input)
decrypt_button.pack(side='left', padx=20)

# Create output label
output_label = tk.Label(root, text='', height=6, width=20, justify='left')
output_label.pack(side='left', padx=20, pady=12)
# Add border around output label
output_label.config(borderwidth=2, relief='groove')

# Start main loop
root.mainloop()

