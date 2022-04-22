//Agustin Moure Rodriguez
#include <sys/types.h>

#include <sys/socket.h>

#include <netdb.h>

#include <string.h>

#include <unistd.h>

#include <iostream>

#include <netinet/in.h>

#include <arpa/inet.h>

using namespace std;

void exercise4( char **argv){

    struct addrinfo hints;

    struct addrinfo *res;



    char host[NI_MAXHOST];

    char service[NI_MAXSERV];





    struct sockaddr_in client_addr;

    socklen_t client_len = sizeof(struct sockaddr_in);



    memset(&hints, 0, sizeof(struct addrinfo));



    hints.ai_family = AF_UNSPEC;



    hints.ai_protocol = 0;



    hints.ai_socktype = SOCK_STREAM;

  



    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);





    if(rc != 0)

    {

        cerr << gai_strerror(rc) << "\n";

        return;

    }



   int sd = socket(res->ai_family, res->ai_socktype,res->ai_protocol);



   if(bind(sd,res->ai_addr,res->ai_addrlen) != 0)

   {

       cerr << "bind: " << "\n";

       return;

   }

   freeaddrinfo(res);

    

   listen(sd,16);





   int sd_client = accept(sd, (struct sockaddr *) &client_addr, &client_len);



  



    cout << "Conexion desde IP: " << inet_ntoa(client_addr.sin_addr) << " PUERTO: " << ntohs(client_addr.sin_port) << "\n";



    char buffer[80];

    ssize_t bytes = 1 ;



    while(bytes > 0){

        

        getnameinfo((struct sockaddr *) &client_addr, client_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);



        bytes = recv(sd_client, (void*) buffer, sizeof(char)*79,0);

        buffer[bytes]= '\0';



        send(sd_client, (void*)buffer, bytes, 0);

        

        

    }



    cerr << "Conexion terminada\n";

}

int main(int argc, char **argv){
    if(argc==1){}
    exercise4(argv);
    return 0;
}