import hashlib
import os

from Crypto.PublicKey import RSA
from pyasn1.codec.der import encoder
from pyasn1.type import univ

KEYSIZE = 1024

# Generate an RSA key
rsakey = RSA.generate(KEYSIZE, os.urandom)

# Create an RSAPublicKey structure
pkinfo = univ.Sequence()
pkinfo.setComponentByPosition(0, univ.Integer(rsakey.n))
pkinfo.setComponentByPosition(1, univ.Integer(rsakey.e))

#Convert the key into a bit string
def to_bitstring(num):
  buf = ''
  while num > 1:
    buf = str(num & 1) + buf
    num = num >> 1
  buf = str(num) + buf
  return buf
pklong = long(encoder.encode(pkinfo).encode('hex'), 16)
pkbitstring = univ.BitString("'00%s'B" % to_bitstring(pklong))

# Get the rsaEncryption identifier:
idrsaencryption = univ.ObjectIdentifier('1.2.840.113549.1.1.1')

# Get the AlgorithmIdentifier for rsaEncryption
idinfo = univ.Sequence()
idinfo.setComponentByPosition(0, idrsaencryption)
idinfo.setComponentByPosition(1, univ.Null(''))

# Get the SubjectPublicKeyInfo structure
publickeyinfo = univ.Sequence()
publickeyinfo.setComponentByPosition(0, idinfo)
publickeyinfo.setComponentByPosition(1, pkbitstring)

# Encode the public key structure
publickey = encoder.encode(publickeyinfo)

digest = hashlib.sha256(publickey).hexdigest()[:32]
extension_id = ''.join(["abcdefghijklmnop"[int(i,16)] for i in digest])

print ''.join(rsakey.publickey().exportKey().split('\n')[1:-1])
print extension_id
