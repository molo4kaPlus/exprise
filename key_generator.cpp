#include "cryptopp/aes.h"
#include "cryptopp/modes.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include "cryptopp/cryptlib.h"
#include "cryptopp/hex.h"
#include "cryptopp/filters.h"
#include "cryptopp/secblock.h"
#include "cryptopp/sha.h"
#include "cryptopp/md5.h"
#include "cryptopp/hmac.h"
#include <cstddef>
#include <cassert>
#include <cstdint>
#include <time.h>

using namespace std;
using namespace CryptoPP;

string encryptStringAES(const string key, const string plaintext) {
    byte aesKey[AES::DEFAULT_KEYLENGTH];
    memset(aesKey, 0x00, AES::DEFAULT_KEYLENGTH);
    memcpy(aesKey, key.c_str(), min(key.length(), static_cast<size_t>(AES::DEFAULT_KEYLENGTH)));

    string ciphertext;

    try {
        ECB_Mode<AES>::Encryption encryptor;
        encryptor.SetKey(aesKey, AES::DEFAULT_KEYLENGTH);

        StringSource(plaintext, true,
            new StreamTransformationFilter(encryptor,
                new HexEncoder(
                    new StringSink(ciphertext)
                )
            )
        );
    }
    catch(const CryptoPP::Exception& e) {
        cerr << e.what() << endl;
        exit(1);
    }

    return ciphertext;
}

string decryptStringAES(const string key, const string ciphertext) {
    byte aesKey[AES::DEFAULT_KEYLENGTH];
    memset(aesKey, 0x00, AES::DEFAULT_KEYLENGTH);
    memcpy(aesKey, key.c_str(), min(key.length(), static_cast<size_t>(AES::DEFAULT_KEYLENGTH)));

    string decryptedtext;

    try {
        ECB_Mode<AES>::Decryption decryptor;
        decryptor.SetKey(aesKey, AES::DEFAULT_KEYLENGTH);

        StringSource(ciphertext, true,
            new HexDecoder(
                new StreamTransformationFilter(decryptor,
                    new StringSink(decryptedtext)
                )
            )
        );
    }
    catch(const CryptoPP::Exception& e) {
        return "0";
    }

    return decryptedtext;
}

int main() {
    string key = "0fffb92f6d1";
    //string key = "0fffb92f6d1";
    string plaintext = to_string(time(NULL) + 86400);

    string ciphertext = encryptStringAES(key, plaintext);
    cout << "Ciphertext: " << ciphertext << endl;

    cout << "text: " << decryptStringAES(key, ciphertext) << endl;

    return 0;
    // мой ключ 651482DA86310434F16C5793069321C1
}