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
// las direcciones multicast rango: 224.0.0.1 a 239.255.255.255

typedef struct bloque {
          char tx[12];//identidad del transmisor multicast
          char narch[32];//nombre del archivo transmitido
          int nb;// número del bloque transmitido
          int bb;// numero de bytes en el bloque, bb==0 => terminó el archivo
          char bytes[1024];//bloque de bytes del archivo
}bloque;


extern int errno;
char bufer[1024];

main(int argc, char * argv[]){
        struct  sockaddr_in server;
        struct ip_mreq multi;
        int adrl, sock,archivo,l;
        bloque datos;
/********************************************************************/
        sock= socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
                perror ("no se puede crear socket");
                exit (1);
        }
/********************************************************************/
        bzero(&server, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
//       server.sin_port = 0; //deja que el sistema operativo le asigne numero de puerto
        server.sin_port = htons(atoi(argv[2])); //deja que el sistema operativo le asigne numero de puerto
/********************************************************************/
        multi.imr_multiaddr.s_addr=inet_addr(argv[1]);
        multi.imr_interface.s_addr=INADDR_ANY;
        if(setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&multi, sizeof(multi))<0){
                perror("no se puede agregar multicast");
                exit(1);
        }
/********************************************************************/
        if (bind(sock, (struct  sockaddr *)&server, sizeof(server))<0){
                close(sock);
                perror("no se puede hacer bind del socket");
                exit(1);
        }
/********************************************************************/

        adrl = sizeof (struct sockaddr_in);
        getsockname(sock, (struct  sockaddr *)&server, &adrl);
        printf("Me asignaron el puerto: %d\n",ntohs(server.sin_port));

/********************************************************************/
/********************************************************************/

	if(read(sock,&datos,sizeof(datos))<0)perror("recibiendo datagrama");
	archivo = open(datos.narch,O_WRONLY | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
	int n  = datos.nb;
	while(1){
	  if (read(sock,&datos,sizeof(datos))<0)perror("recibiendo datagrama");
	  write(archivo,&datos.bytes,datos.bb);
	  if(datos.bb <= 0 ){
	    lseek(archivo,0,SEEK_SET);
	    continue;
          }
	  if(datos.nb == n){
	    break;
	    }
	  
	  
        }
        close(archivo);


}
