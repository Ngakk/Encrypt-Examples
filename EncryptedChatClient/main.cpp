#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <windows.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>
extern "C"

{

#include "aes.h"

}

using namespace std;

SOCKET client;

char message_listen[100];
char message_send[100];
aes_context ctx;

void EncryptMessage(char* _msg)
{
    int n, i;

    WCHAR *in = TEXT("abc" );

    WCHAR *out;

    WCHAR *secret_key = TEXT("aa" );



    /* Encrypt */

    aes_encrypt( &ctx, buf, buf );

    /* Decrypt */

    aes_decrypt( &ctx, buf, buf );

    out=(WCHAR*)buf;
}

void DecryptMessage(char* _msg)
{

}

void InitAes(){
    unsigned char buf[16];

    unsigned char key[32];

    memset(buf,0,16);

    memset(key,0,32);

    memcpy( buf, in, 16);

    /* Set the key */

    memcpy( key, secret_key, 32);

    aes_set_key( &ctx, key, 256);
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
