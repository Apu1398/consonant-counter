/*Pasos para compilar y ejecutar:
1. "gcc -o client client.c"
2. "./client ip puerto"*/

/*Pasos para crear y ejecutar el docker container (linux):
1. sudo docker image build -t docker-client .
2. sudo docker container run -it --network="host" docker-client */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <fcntl.h>  // for open
#include <unistd.h> // for close
#include <netdb.h>  //netbd.h es necesitada por la estructura hostent
#include <string.h>

#define BUFFSIZE 1
#define ERROR -1

char buffer[BUFFSIZE];
char *text;

int main()
{

    char ip[100];
    char puerto[100];
    struct hostent *he;
    struct sockaddr_in server;
    int fd, numbytes;

    printf("*** Configurando Conexion ***\n");
    printf("-> Digite la direccion ip del servidor: ");
    gets(ip);
    printf("-> Digite el puerto: ");
    fgets(puerto, sizeof(puerto), stdin);

    char direccionArchivo[100];
    printf("-> Ingrese la direccion del archivo o ingrese 'end' para terminar: ");
    gets(direccionArchivo);

    if ((he = gethostbyname("localhost")) == NULL)
    {
        printf("No se puede encontrar el server con el ip: %s\n", ip);
        exit(-1);
    }

    while (strcmp(direccionArchivo, "end") != 0)
    {
        FILE *archivo = fopen(direccionArchivo, "rb");
        if (!archivo)
        {
            int error = 0;
            printf("-->> No se encuentra el archivo, intente de nuevo\n");
        }
        else
        {
            if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            {
                printf("socket() error\n");
                exit(-1);
            }

            // Datos del servidor
            server.sin_family = AF_INET;
            server.sin_port = htons(atoi(puerto));
            server.sin_addr = *((struct in_addr *)he->h_addr);
            bzero(&(server.sin_zero), 8);

            //*****Abriendo el archivo y enviando****

            if (connect(fd, (struct sockaddr *)&server,
                        sizeof(struct sockaddr)) == -1)
            {
                printf("-> No se pudo conectar al socket\n");
                exit(-1);
            }

            /*Se envia el archivo*/
            fread(buffer, sizeof(char), BUFFSIZE, archivo);
            while (!feof(archivo))
            {

                if (send(fd, buffer, BUFFSIZE, 0) == ERROR)
                    perror("->Error al enviar el archivo:");
                fread(buffer, sizeof(char), BUFFSIZE, archivo);
            }
            send(fd, "|", BUFFSIZE, 0); // Este caracter le indica al servidor que ya dejo de enviar caracteres. (Se puede cambiar por otro menos comun)
           //printf("Archivo enviado\n");

            int consonantes = 0;

            recv(fd, &consonantes, sizeof(consonantes), 0); // Aca recibi la respuesta de cuantas consonantes hay. Se puede hacer lo mismo para cualquier otra info que se necesite

            printf("--> Consonantes: %d\n", consonantes);
            close(fd);
        }
        printf("\nIngrese la direccion del archivo o ingrese 'end' para terminar: ");
        gets(direccionArchivo);
    }
}
