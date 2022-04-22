//Agustin Moure Rodriguez
#include <sys/types.h>

#include <sys/socket.h>

#include <netdb.h>

#include <string.h>

#include <unistd.h>

#include <iostream>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <stdio.h>

#include <stdlib.h>


using namespace std;

void exercise3( char **argv){

    struct addrinfo hints;

    struct addrinfo *res;



    char buffer[80];



    struct sockaddr_in server_addr;

    socklen_t server_len = sizeof(struct sockaddr_in);

    ssize_t bytes;



  



    memset(&hints, 0, sizeof(struct addrinfo));



    hints.ai_family = AF_UNSPEC;

    hints.ai_protocol = 17;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);





    if(rc != 0)

    {

        cerr << gai_strerror(rc) <<"\n";

        return;

    }



    

    int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);





    server_addr.sin_family = AF_UNSPEC;



    server_addr.sin_port = htons(atoi(argv[2]));

    server_addr.sin_addr.s_addr = inet_addr(argv[1]);



    sendto(sd, argv[3], 2, 0, (struct sockaddr *)&server_addr, server_len);

    bytes = recvfrom(sd,buffer,79 * sizeof(char), 0,(struct sockaddr *) &server_addr, &server_len);

    buffer[bytes]= '\0';

    cerr << buffer << "\n";

    freeaddrinfo(res);

}

int main(int argc, char **argv){
    if(argc==1){}
    exercise3();
    return 0;
}