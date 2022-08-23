/*Pasos para compilar y ejecutar:
1. "gcc -o client client.c"
2. "./client ip puerto"*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <fcntl.h>  // for open
#include <unistd.h> // for close
#include <netdb.h>  //netbd.h es necesitada por la estructura hostent
#include<string.h>

#define BUFFSIZE 1
#define	ERROR	-1

char buffer[BUFFSIZE];
char *text;

int main(int argc, char *argv[])
{
    //Lee argumento 2 (puerto) y empieza a setear el socket
    if (argc > 2)
    {

        char *ip;
        int fd, numbytes, puerto;
        char buf[100];
        puerto = atoi(argv[2]);
        ip = argv[1];    //Aca lee el argumento 1 (ip)

        struct hostent *he;
        struct sockaddr_in server;

        if ((he = gethostbyname(ip)) == NULL)
        {
            printf("gethostbyname() error\n");
            exit(-1);
        }

        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            printf("socket() error\n");
            exit(-1);
        }

        // Datos del servidor
        server.sin_family = AF_INET;
        server.sin_port = htons(puerto);
        server.sin_addr = *((struct in_addr *)he->h_addr);
        bzero(&(server.sin_zero), 8);
        
        if (connect(fd, (struct sockaddr *)&server,
                    sizeof(struct sockaddr)) == -1)
        {
            printf("connect() error\n");
            exit(-1);
        }

        //*****Abriendo el archivo y enviando****
        //Aca habria que implementar logica para que "test.txt" no este quemado

        FILE *archivo = fopen("test.txt", "rb");
        if (!archivo)
        {
            perror("Error al abrir el archivo:");
            exit(EXIT_FAILURE);
        }
        /*Se envia el archivo*/
        fread(buffer, sizeof(char), BUFFSIZE, archivo);
        while (!feof(archivo))
        {

            if (send(fd, buffer, BUFFSIZE, 0) == ERROR)
                perror("Error al enviar el archivo:");
            fread(buffer, sizeof(char), BUFFSIZE, archivo);
        }
        send(fd, "|", BUFFSIZE, 0); //Este caracter le indica al servidor que ya dejo de enviar caracteres. (Se puede cambiar por otro menos comun)
        printf("Archivo enviado\n");

        recv(fd, buffer, 2, 0);  //Aca recibi la respuesta de cuantas consonantes hay. Se puede hacer lo mismo para cualquier otra info que se necesite

        printf("Consonantes: %s\n", buffer);

        close(fd);  //Cierra el socket
    }
    else
    {
        printf("No se ingreso el ip y puerto por parametro\n");
    }
}
