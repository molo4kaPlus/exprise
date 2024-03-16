#include <iostream>
#include <string>
#include <time.h>
#include <vector>
#include <conio.h>

using namespace std;

string encrypt(string input) {
    string word = input;
    string alphabet = "abcdefghijklmnopqrstuvwxyz1234567890";

    for (int i = 0; i < word.size(); i++) {
        for (int j = 0; j < alphabet.size(); j++) {
            if(word[j] == alphabet[j])
            {
                word[i] = alphabet[0];
                break;
            }        
        }       
    }
    return word;
} 

// Ðàñøèôðîâêà
string decrypt(string input) {
    string word = input;
    string alphabet = "abcdefghijklmnopqrstuvwxyz1234567890";

    for (int i = 0; i < word.size(); i++) {
        for (int j = 0; j < alphabet.size(); j++) {
            if(word[j] == alphabet[j])
            {
                word[i] = alphabet[(j - i) % 37];
                break;
            }        
        }       
    }
    return word;
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
