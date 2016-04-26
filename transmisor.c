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
          char bytes[2024];//bloque de bytes del archivo
}bloque;

#define DATA "Este el super mensaje en datagrama"

main(int argc, char * argv[]){
        int  sock,l;
        struct  sockaddr_in name;
        struct  hostent *hp, *gethostbyname();
        bloque datos;
/********************************************************************/
        sock= socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
                perror ("no se puede crear socket");
                exit (1);
        }
/********************************************************************/
        hp = gethostbyname(argv[1]);
        if (hp == 0) {
                fprintf(stderr, "%s: host desconocido\n", argv[1]);
                exit(2);
        }
        bcopy(hp->h_addr,&name.sin_addr, hp -> h_length);
        name.sin_family = AF_INET;
        name.sin_port = htons(atoi(argv[2]));


        int f =  open("foto.jpg",O_RDONLY , S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        datos.nb = 0;
        while(1){
          if(datos.bb<= 0){
            lseek(f,0,SEEK_SET);

          }
          l = read(f,&datos.bytes, sizeof(datos.bytes));
          datos.bb = l;
          send(sock,&datos, sizeof(datos), 0);
          printf("%d\n", datos.bb);

        }
        close(f);
        close(sock);
}
