#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <windows.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>

using namespace std;

SOCKET  client;
SOCKET  server;

char message_listen[100];
char message_send[100];


void* thread_function_listen(void* id)
{
    while(strncmp(message_listen, "salir", 5) != 0 || strncmp(message_send, "salir", 5) != 0)
    {
        ///ahora esperamos mensaje del cliente
        memset(message_listen, 0, sizeof(message_listen)); //limpiamos buffer
        if(recv(client, message_listen, sizeof(message_listen), 0) != sizeof(message_listen))
        {
            puts("Nada recibido");
            closesocket(client);
            closesocket(server);
            WSACleanup();
            Sleep(4000);
            break;
        }
        else
        {
            printf("Cliente: %s", message_listen);
        }
    }
}

void* thread_function_send(void *id)
{
    while(strncmp(message_send, "salir", 5) != 0 || strncmp(message_listen, "salir", 5) != 0)
    {
        ///ahora nosotros contestamos
        fgets(message_send, 100, stdin);

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
