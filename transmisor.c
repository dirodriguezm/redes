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

#define DATA "Este el super mensaje en datagrama"

main(int argc, char * argv[]){
        int  sock;
        struct  sockaddr_in name;
        struct  hostent *hp, *gethostbyname();

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
        bcopy(hp->h_addr, &name.sin_addr, hp->h_length);
        name.sin_family = AF_INET;
        name.sin_port = htons(atoi(argv[2]));

        if(sendto(sock, (void *)DATA, sizeof(DATA), 0, (struct sockaddr * )&name, sizeof(name))<0)
                perror("enviando el datagrama");
        close(sock);
}
