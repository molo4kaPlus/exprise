#include <iostream>
#include <string>
#include <time.h>
#include <vector>
#include <conio.h>

using namespace std;

string encrypt(string input) {
    vector<char> word(input.begin(), input.end());
    string alphabet = "abcdefghijklmnopqrstuvwxyz1234567890";

    for (int i = 0; i < (int)input.length(); i++) {
        for (int j = 0; j < (int)alphabet.length(); j++) {
            if (word[i] == alphabet[j]) {
                word[i] = alphabet[(j + 3) % 37];

                break;
            }
        }
    }
    string str(word.begin(), word.end());
    return str;
} 

// Ðàñøèôðîâêà
string decrypt(string input) {
    vector<char> word(input.begin(), input.end());
    string alphabet = "abcdefghijklmnopqrstuvwxyz1234567890";

    for (int i = 0; i < (int)input.length(); i++) {
        for (int j = 0; j < (int)alphabet.length(); j++) {
            if (word[i] == alphabet[j]) {
                word[i] = alphabet[(j - 3) % 37];
                break;
            }
        }
    }
    string str(word.begin(), word.end());
    return str;
}


int main() {
    int start_time = time(NULL);
    int end_time = start_time + (3600 * 24);
    string MAC = "a8a159119c5f";

    string encrypted = MAC + to_string(end_time);

    cout << "To encrypt: " << encrypted << endl;
    encrypted = encrypt(encrypted);
    cout << "Encrypted: " << encrypted << endl;

    string decrypted = decrypt(encrypted);
    cout << "Decrypted: " << decrypted << endl;

    

    return 0;
}
