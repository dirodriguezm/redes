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
int numero;

typedef struct bloque {
          char tx[12];//identidad del transmisor multicast
          char narch[32];//nombre del archivo transmitido
          int nb;// número del bloque transmitido
          int bb;// numero de bytes en el bloque, bb==0 => terminó el archivo
          char bytes[1024];//bloque de bytes del archivo
}bloque;


int obtenerSocket(char * puerto){
        struct  sockaddr_in server;
        int x, sock;
/********************************************************************/
        sock= socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
                perror ("no se puede crear socket");
                exit (1);
        }
/********************************************************************/
        bzero(&server, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(atoi(puerto));  //lee numero de puerto desde teclado
        x = bind(sock, (struct  sockaddr *)&server, sizeof(server));
        if (x<0){
                close(sock);
                perror("no se puede hacer bind del socket");
                exit(1);
        }
/********************************************************************/
        if (listen(sock, 5) < 0) {
                perror ("no listen");
                exit (1);
        }
  return sock;
}

int aceptarConexion(int sock){
        struct  sockaddr_in server;
        int adrl, conexion;

/********************************************************************/
        adrl = sizeof (struct sockaddr_in);
        printf("Servidor esperando a cliente\n");
        conexion = accept(sock, (struct  sockaddr *)&server, &adrl); //espera y acepta conexion del cliente
        return conexion;
}



main(int argc, char * argv[]){
        int sock, conexion;
        bloque datos;
/********************************************************************/
        while(1){
        	 sock = obtenerSocket(argv[1]);
        	 conexion = aceptarConexion(sock); //espera y acepta conexion del cliente
        	 if (conexion)printf("llego un cliente!\n");
             //Se abre el archivo a enviar al cliente
        	 int archivo =  open(argv[2],O_RDONLY , S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
             strcpy(datos.narch, argv[2]);
        	 while(1){
        	 	datos.bb = read(archivo, &datos.bytes, sizeof(datos.bytes));
                write(conexion, &datos, sizeof(datos));
                if (datos.bb<=0){
        	 		break;
        	 	}

        	 }
        	 printf("Archivo Enviado\n");
        	 close(archivo);
			 close(conexion);
        }

/********************************************************************/


}
