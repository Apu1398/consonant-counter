#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>  // for open
#include <unistd.h> // for close
#include<time.h>

#define BUFFSIZE 1
#define ERROR -1

char buffer[BUFFSIZE];

void recibirArchivo(int);
int contarConsonantes();

char *token;
char ruta[100];

int main()
{

   int fd, fd2, longitud_cliente, puerto;    

    FILE *ptr;
    char str[200];
    ptr = fopen("/home/apu/Documents/Operativos/consonant-counter/Server/config-file", "r");

    if (NULL == ptr)
    {
        printf("file can't be opened \n");
    }    
    else
    {
        fgets(str, 200, ptr);

        token = strtok(str, ";");

        puerto = atoi(token);

        struct sockaddr_in server;
        struct sockaddr_in client;

        // Configuracion del servidor
        server.sin_family = AF_INET;         // Familia TCP/IP
        server.sin_port = htons(puerto);     // Puerto
        server.sin_addr.s_addr = INADDR_ANY; // Cualquier cliente puede conectarse
        bzero(&(server.sin_zero), 8);        // Funcion que rellena con 0's

        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("Error de apertura de socket");
            exit(-1);
        }

        if (bind(fd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
        {
            printf("error en bind() \n");
            exit(-1);
        }

        if (listen(fd, 5) == -1)
        {
            printf("error en listen()\n");
            exit(-1);
        }

        printf("Servidor corriendo en %d\n", puerto);

        // Aceptar conexiones
        while (1)
        {
            longitud_cliente = sizeof(struct sockaddr_in);
            if ((fd2 = accept(fd, (struct sockaddr *)&client, &longitud_cliente)) == -1)
            {
                printf("error en accept()\n");
                exit(-1);
            }

            recibirArchivo(fd2);

            int consonantes = contarConsonantes();

            send(fd2, &consonantes, sizeof(consonantes), 0); // Aca envia una respuesta simulando la cantidad de consonantes que encontro

            printf("Resultado enviado...\n");

            close(fd2); 
        }
        close(fd);
    }

    return 0;
}

void recibirArchivo(int SocketFD)
{

    char buffer[1];
    int recibido = -1;

    token = strtok(NULL, ";");

    time_t t = time(NULL);

    const char* str2 = ctime(&t);
   
    strcat(strcpy(ruta, token), str2);
    //printf("%s\n", buffer);

    FILE *file = fopen(ruta, "wb");

    recv(SocketFD, buffer, 1, 0);

    while (buffer[0] != '|')
    {
        fwrite(buffer, sizeof(char), 1, file);
        recv(SocketFD, buffer, 1, 0);
    } // Termina la recepción del archivo

    fclose(file);
}

int contarConsonantes()
{

    int consonantes = 0;
    FILE *archivo;
    archivo = fopen(ruta, "r");

    if (!archivo)
    {
        perror("Error al abrir el archivo:");
        exit(EXIT_FAILURE);
    }

    fread(buffer, sizeof(char), BUFFSIZE, archivo);

    while (!feof(archivo))
    {
        char *pPosition = strchr("bcdfghjklmnpqrstvwxyzBCDFGHJKLMNPQRSTVWXYZ", buffer[0]);

        if (pPosition != NULL)
        {
            consonantes++;
        }
        fread(buffer, sizeof(char), BUFFSIZE, archivo);
    }

    return consonantes;
}