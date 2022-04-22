//Agustin Moure Rodriguez
#include <sys/types.h>

#include <sys/socket.h>

#include <netdb.h>

#include <string.h>

#include <unistd.h>

#include <iostream>

using namespace std;

void exercise1( char **argv){

    struct addrinfo hints;

    struct addrinfo *res;

    char host[NI_MAXHOST];

    char service[NI_MAXSERV];



    memset(&hints, 0, sizeof(struct addrinfo));



    hints.ai_family = AF_UNSPEC;



    hints.ai_protocol = 0;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);





    if(rc != 0)

    {

        cerr << gai_strerror(rc) << "\n";

        return;

    }

    struct addrinfo *rp;

    int aux;

    for (rp = res; rp != NULL; rp = rp->ai_next){

        

       aux = getnameinfo(rp->ai_addr, rp->ai_addrlen,host,NI_MAXHOST,service,NI_MAXSERV,NI_NUMERICHOST | NI_NUMERICSERV);

       if (aux != 0){

           cerr << "Error: Name or service not known\n";

       }

       else { 

           cerr << host << "  " << rp->ai_family << "  " << rp->ai_socktype << "\n";

       }

        

    }

}
int main(int argc, char **argv){

    if(argc==1){}
    exercise1(argv);
    return 0;
}