# Name        : Netwig, Chad
# Project     : Esper
# Date        : April 2, 2023
# File Name   : Esper.py

import argparse
import os
import time
import sys
import string

# Handle command-line arguments
parser = argparse.ArgumentParser()
parser.add_argument("-d", "--decrypt", help='Decrypt a file', required=False)
parser.add_argument("-e", "--encrypt", help='Encrypt a file', required=False)
parser.add_argument("-o", "--outfile", help='Output file', required=False)

args = parser.parse_args()

encrypting = True

try:
    ciphertext = open(args.decrypt, "rb").read()
    try:
        plaintext = open(args.encrypt, "rb").read()
        print("You can't specify both -e and -d")
        exit(1)
    except Exception:
        encrypting = False
except Exception:
    try:
        plaintext = open(args.encrypt, "rb").read()
    except Exception:
        print("Input file error (did you specify -e or -d?)")
        exit(1)

def lrot(n, d):
    return ((n << d) & 0xff) | (n >> (8 - d))

if encrypting:
    #
    # Esper cipher
    #

    # Get the key
    keybytes = bytes(os.urandom(8))
    keyrotate = keybytes[0] % 7 + 1
    keyxor = []
    key = ""
    # Rotate the key
    for i in range(1, 8):
        keyxor.append(ord(string.ascii_letters[keybytes[i] % len(string.ascii_letters)]))
        key = key + chr(keyxor[i-1])
    print("The key is %s rotated by %d bits." % (key, keyrotate))

    ciphertext = []
    for i in range(0, len(plaintext)):
        ciphertext.append(lrot(plaintext[i], keyrotate) ^ keyxor[i % len(keyxor)])

    with open(args.outfile, "wb") as output:
        output.write(bytes(ciphertext))
        output.close()
else:
    #
    # Esper cipher decryption
    #

    # Get the key
    key = "rORTrfA"
    keyrotate = 2
    keyxor = []
    for i in range(1, 8):
        keyxor.append(ord(key[i-1]))

    plaintext = []
    for i in range(0, len(ciphertext)):
        plaintext.append(lrot(ciphertext[i] ^ keyxor[i % len(keyxor)], 8 - keyrotate))

    with open(args.outfile, "wb") as output:
        output.write(bytes(plaintext))
        output.close()
        
    #print("Decryption is not implemented!")
    exit(1)
