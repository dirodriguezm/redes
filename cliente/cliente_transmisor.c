#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct bloque {
          char tx[12];//identidad del transmisor multicast
          char narch[32];//nombre del archivo transmitido
          int nb;// número del bloque transmitido
          int bb;// numero de bytes en el bloque, bb==0 => terminó el archivo
          char bytes[1024];//bloque de bytes del archivo
}bloque;

int conectarCon(char * host, char * puerto){
        int conexion, l;
        struct  sockaddr_in client;
        struct  hostent *hp, *gethostbyname();
        bzero(&client, sizeof(client));
        client.sin_family = AF_INET;
        client.sin_port = htons(atoi(puerto));
        if (isdigit(host[0]))
                client.sin_addr.s_addr = inet_addr(host);
        else {
                hp = gethostbyname(host);
                if (hp == NULL) {
                        perror ("Â¿Cual host ? ");
                        exit (1);
                }
                bcopy(hp->h_addr, &client.sin_addr, hp->h_length);
        }
/*******************************************************************/
        conexion = socket(AF_INET, SOCK_STREAM, 0);
        if (conexion < 0) {
                perror ("no es posible crear socket");
                exit (1);
        }
/*******************************************************************/
        connect(conexion, (struct  sockaddr *)&client, sizeof(client)); //se conecta con el servidor
        return conexion;
}

main(int argc, char * argv[]){

        int sock,conexion, archivo,archivo2, l;
        bloque datos;
        printf("Se va a conectar al servidor \n");
        conexion=conectarCon(argv[1], argv[2]);
        printf("Se conectó\n");
        read(conexion, &datos.narch , sizeof(datos.narch));    
        archivo = open(datos.narch,O_WRONLY | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        while(1){
                datos.bb=read(conexion, &datos.bytes , sizeof(datos.bytes));
                if( datos.bb <= 0 ){
                        break;
                }
                write(archivo, &datos.bytes ,datos.bb);

        }
        close(conexion);
        close(archivo);

        struct  sockaddr_in name;
        struct  hostent *hp, *gethostbyname();

/********************************************************************/
        sock= socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
                perror ("no se puede crear socket");
                exit (1);
        }
/********************************************************************/
        hp = gethostbyname(argv[3]);
        if (hp == 0) {
                fprintf(stderr, "%s: host desconocido\n", argv[1]);
                exit(2);
        }
	bcopy(hp->h_addr,&name.sin_addr, hp -> h_length);
        name.sin_family = AF_INET;
        name.sin_port = htons(atoi(argv[4]));
        //archivo a enviar.

        archivo =  open(datos.narch,O_RDONLY , S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        int cont = 0;
        while(1){
            datos.bb = read(archivo, &datos.bytes, sizeof(datos.bytes));
            datos.nb = cont;
            if(sendto(sock,&datos, sizeof(datos), 0, (struct sockaddr * )&name, sizeof(name))<0)perror("enviando el datagrama");
            if(datos.bb<= 0){
                lseek(archivo,0,SEEK_SET);
                cont = 0;
                continue;
            }
            cont++;
            printf("%d %d \n", datos.bb,datos.nb);
        }
        close(archivo);
        close(sock);
}
