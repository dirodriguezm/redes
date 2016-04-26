#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>

extern int errno;

typedef struct pdu{
  int n_bytes;
  char buffer[1024];
}pdu;

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
int main(int argc, char * argv[]){
        int numero=555;
        int conexion, archivo, l;
        pdu datos;
        printf("Se va a conectar al servidor \n");
        conexion=conectarCon(argv[1], argv[2]);
        printf("Se conectó\n");
        archivo = open(argv[3],O_WRONLY | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        while(1){

          l=read(conexion, &datos.buffer , sizeof(datos.buffer));
          if( l <= 0 ){
            break;
          }
          printf("Recibiendo %d\n",l);
          write(archivo, &datos.buffer ,l);

        }
        close(archivo);
        close(conexion);
        return 0;
}
