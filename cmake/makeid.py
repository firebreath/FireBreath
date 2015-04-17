import hashlib
import os

from Crypto.PublicKey import RSA

KEYSIZE = 1024

# Generate an RSA key
rsakey = RSA.generate(KEYSIZE, os.urandom)

# grab the public key portion
publickey = rsakey.publickey()

digest = hashlib.sha256(publickey.exportKey('DER')).hexdigest()[:32]
extension_id = ''.join(["abcdefghijklmnop"[int(i,16)] for i in digest])

print ''.join(publickey.exportKey().split('\n')[1:-1])
print extension_id
