#include <iostream>

using namespace std;

string EncryptDescrypt(string toEncrypt)
{
    char Password[] = {'P', 'E', 'R', 'A'};
    string OutPut = toEncrypt;

    for(int i = 0; i < toEncrypt.size(); i++)
    {
        OutPut[i] = toEncrypt[i] ^ Password[i % (sizeof(Password) / sizeof(char))];
        cout << "Original : " << toEncrypt[i] << " -> nuevo: "
        << OutPut[i] << " usando: " << Password[i % (sizeof(Password) / sizeof(char))] << endl;
    }

    return OutPut;
}

int main()
{
    string cifrado = EncryptDescrypt("Esto es mi texto original");
    cout << "Encrypted: " << cifrado << "\n";

    string descifrado = EncryptDescrypt(cifrado);
    cout << "Decrypted: " << descifrado << "\n";

    return 0;
}
