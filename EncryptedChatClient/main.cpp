#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <windows.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>
#include "aes.h"

using namespace std;

SOCKET client;

char message_listen[100];
char message_send[100];
unsigned char key[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x011,
    0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
void ConvertToUnsigned(char* _input, unsigned char* _output)
{
    for(int i = 0; i < 100; i++){
        _output[i] = static_cast<unsigned char>(_input[i]);
    }
}

void ConvertToSigned(unsigned char* _input, char* _output)
{
    for(int i = 0; i < 100; i++)
    {
        _output[i] = static_cast<char>(_input[i]);
    }
}

void EncryptMessage(char* _msg)
{
    AES aes(256);

    unsigned char plain[100];

    ConvertToUnsigned(_msg, plain);

    unsigned int len = 0;
    unsigned char *out = aes.EncryptECB(plain, 16 * sizeof(unsigned char), key, len);

    cout << "Mensaje encriptado: " << out << endl;

    ConvertToSigned(out, _msg);

    delete[] out;
}

void DecryptMessage(char* _msg)
{
    AES aes(256);

    unsigned char plain[100];

    ConvertToUnsigned(_msg, plain);

    unsigned int len = 0;
    unsigned char *out = aes.DecryptECB(plain, 16 * sizeof(unsigned char), key, len);

    ConvertToSigned(out, _msg);

    delete[] out;
}

void* thread_function_listen(void *id)
{
    while(strncmp(message_listen, "salir", 5) != 0 || strncmp(message_send, "salir", 5) != 0)
    {
        ///ahora esperamos mensaje del servidor
        memset(message_listen, 0, sizeof(message_listen)); //limpiamos buffer
        if(recv(client, message_listen, sizeof(message_listen), 0) != sizeof(message_listen))
        {
            puts("Nada recibido");
            closesocket(client);
            WSACleanup();
            Sleep(4000);
            break;
        }
        else
        {
            DecryptMessage(message_listen);
            printf("Servidor: %s", message_listen);
        }

    }
}

void* thread_function_send(void *id)
{
    while(strncmp(message_listen, "salir", 5) != 0 || strncmp(message_send, "salir", 5) != 0)
    {
        ///ahora nosotros contestamos
        fgets(message_send, 100, stdin);
        EncryptMessage(message_send);
        if(send(client, message_send, sizeof(message_send), 0) != sizeof(message_send))
        {
            puts("Conexion perdida");
            closesocket(client);
            WSACleanup();
            Sleep(4000);
            break;
        }
    }
}

//Cliente
int main()
{
    WSADATA wsData;

    struct sockaddr_in endpoint;

    char ipAddr[15];
    int port = 6590;    ///puerto que usara el cliente

    int error = WSAStartup(MAKEWORD(2, 2), &wsData);

    memset(ipAddr, 0, sizeof(ipAddr));
    if(error != 0)
    {
        printf("Winsock error %d ocurrido\n", WSAGetLastError());
        WSACleanup();
        Sleep(4000);
        return EXIT_FAILURE;
    }
    else
        puts("SISTEMA DE CLIENTE");

    printf("\nDireccion IP a conectar: ");
    scanf("%s", ipAddr);

    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    endpoint.sin_addr.s_addr = inet_addr(ipAddr); ///asignacion de cualquier IP
    endpoint.sin_family      = AF_INET;
    endpoint.sin_port        = htons(port);

    ///Conexion al servidor
    if(connect(client, (struct sockaddr*)&endpoint, sizeof(endpoint)) != 0) ///Error
    {
        printf("Error %d ocurrido al conectar servidor\n", WSAGetLastError());
        Sleep(4000);
        return EXIT_FAILURE;
    }

    puts("Cliente ya se conecto; \n");


    ///Creamos hilos
    pthread_t thread_listen, thread_send;

    pthread_create( &thread_listen, NULL, thread_function_listen, (void *)0 );
    pthread_create( &thread_send, NULL, thread_function_send, (void *)0 );

    pthread_join( thread_listen, NULL);
    pthread_join( thread_send, NULL);

    //pasa por aqui, cerramos limpiamente el cliente
    Sleep(4000);
    closesocket(client);
    WSACleanup();
    system("pause");
    return 0;
}
