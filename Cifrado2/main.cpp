#include <iostream>

using namespace std;

const int f = 6;
const int c = 5;
void ordenar_matriz(char *m)
{
    /// la idea es que si su orden actual es
    /// 0, 1, 2, 3 ,4
    /// pase a ser
    /// 3, 0, 1, 4, 2
    int orden[c] = {0};

    orden[0] = 3;
    orden[1] = 0;
    orden[2] = 1;
    orden[3] = 4;
    orden[4] = 2;

    /// Aplicamos el nuevo orden que figura en el array orden a la matriz nueva.
    char nueva[f][c];

    for(int i = 0; i < f; ++i)
    {
        for(int j = 0; j < c; ++j)
        {
            nueva[i][j] = m[i*c + orden[j]];
        }
    }

    ///Imprimimos
    for(int i = 0; i < f; ++i)
    {
        for(int j = 0; j < c; ++j)
        {
            cout << nueva[i][j] << ' ';
        }
        cout << '\n';
    }
    cout << '\n';
}
int main()
{
    char matriz[f][c] = {
        { 'H','O','L','A',' ' },
        { 'M','U','N','D','O' },
        { ' ','E','S','T','O' },
        { ' ','E','S',' ','U' },
        { 'N',' ','T','E','X' },
        { 'T','O',' ',':',')' }
    };

    ordenar_matriz(&matriz[0][0]);
    return 0;
}
