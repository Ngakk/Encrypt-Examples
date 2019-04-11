#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <windows.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>
#include "AES.h"

using namespace std;

SOCKET  client;
SOCKET  server;

unsigned char message_listen[100];
unsigned char message_send[100];
unsigned char key[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x011,
    0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };

unsigned char* EncryptMessage(unsigned char* _msg)
{
  AES aes(256);
  unsigned char *plain = _msg;

  unsigned int len = 0;
  unsigned char *out = aes.EncryptECB(plain, 16 * sizeof(unsigned char), key, len);

  return out;
}

unsigned char* DecryptMessage(unsigned char* _msg)
{
    AES aes(256);
    unsigned char plain[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };

    unsigned int len = 0;
    unsigned char *out = aes.DecryptECB(plain, 16 * sizeof(unsigned char), key, len);

    return out;
}
//TODO: arreglar errores del unsigned char
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
            unsigned char* decrypted_message = DecryptMessage(message_listen);
            printf("Servidor: %s", decrypted_message);

            delete[] decrypted_message;
        }

    }
}

void* thread_function_send(void *id)
{
    while(strncmp(message_listen, "salir", 5) != 0 || strncmp(message_send, "salir", 5) != 0)
    {
        ///ahora nosotros contestamos
        fgets(message_send, 100, stdin);

        char* encrypted_msg = EncryptMessage(message_send);

        if(send(client, encrypted_msg, sizeof(encrypted_msg), 0) != sizeof(encrypted_msg))
        {
            puts("Conexion perdida");
            closesocket(client);
            WSACleanup();
            Sleep(4000);
            break;
        }

        delete[] encrypted_msg;
    }
}

//Servidor
int main()
{
    WSADATA wsData;


    sockaddr_in endpoint;

    ///Iniciar winsock version 2.2
    int error = WSAStartup(MAKEWORD(2, 2), &wsData);

    if(error != 0) // checar errores
    {
        printf("Winsock error %d ocurrido\n", WSAGetLastError());
        WSACleanup();
        Sleep(4000);
        return EXIT_FAILURE;
    }
    else
        puts("SISTEMA DE SERVIDOR");

    ///AF_INET = usar IP V4
    ///SOCK_STREAM mejora TCP,, puede usarse SOCK_DGRAM para mejorar UPD
    ///IPPROTO_TCP , IPPROTO_UDP
    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    endpoint.sin_addr.s_addr = htonl(INADDR_ANY); ///Esperando asignacion de IP
    endpoint.sin_family = AF_INET;
    endpoint.sin_port = htons(6590); ///Usando puerto 6590

    if(bind(server, (struct sockaddr*)&endpoint, sizeof(endpoint)) != 0)
    {
        printf("Winsock error %d occurrio en bind!", WSAGetLastError());
        Sleep(4000);
        return EXIT_FAILURE;
    }

    listen(server, 1); ///Activar listening

    puts("Esperamos cliente");
    client = accept(server, NULL, NULL); ///Esperamos conexion de cliente

    ///Creamos hilos
    pthread_t thread_listen, thread_send;

    pthread_create( &thread_listen, NULL, thread_function_listen, (void *)0 );
    pthread_create( &thread_send, NULL, thread_function_send, (void *)0 );

    pthread_join( thread_listen, NULL);
    pthread_join( thread_send, NULL);

    ///Cerramos conexiones
    closesocket(server);
    closesocket(client);
    WSACleanup();
    system("pause");
    return 0;
}
