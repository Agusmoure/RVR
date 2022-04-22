//Agustin Moure Rodriguez
#include <sys/types.h>

#include <sys/socket.h>

#include <netdb.h>

#include <string.h>

#include <unistd.h>

#include <iostream>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <time.h>
using namespace std;

void exercise2(char **argv){

    struct addrinfo hints;

    struct addrinfo *res;



    char buffer[80];



    char host[NI_MAXHOST];

    char service[NI_MAXSERV];



    struct sockaddr client_addr;

    socklen_t client_len = sizeof(struct sockaddr);

    ssize_t bytes;





    memset(&hints, 0, sizeof(struct addrinfo));



    hints.ai_family = AF_UNSPEC;



    hints.ai_protocol = 17;

  

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);





    if(rc != 0)

    {

        cerr << gai_strerror(rc) << "\n";

        return;

    }

    

    int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);



    if(bind(sd,res->ai_addr,res->ai_addrlen) != 0)

    {

        cerr << "bind: \n";

        return;

    }



    //Recepcion mensaje cliente

    int c = 1;



    time_t rawtime;

    struct tm * timeinfo;





    char message[80];

    while(c == 1){

        bytes = recvfrom(sd,buffer,79 * sizeof(char), 0, &client_addr, &client_len);

        buffer[bytes]= '\0';

        if(bytes != -1){



            getnameinfo(res->ai_addr, res->ai_addrlen,host,NI_MAXHOST,service,NI_MAXSERV,NI_NUMERICHOST | NI_NUMERICSERV);

            cerr << bytes << " bytes de " << host<< ":" << service << "\n";



        }



    

        switch(buffer[0])

        {

            case 'q': 

            c = 0;

            break;

            case 't':

            time(&rawtime);

            timeinfo = localtime (&rawtime);

            strftime(message,80, "%r",timeinfo);

            sendto(sd, message, strlen(message), 0, (struct sockaddr *)&client_addr, client_len);

            break;

            case 'd':

            time(&rawtime);

            timeinfo = localtime (&rawtime);

            strftime(message,80, "%F",timeinfo);

            sendto(sd, message, strlen(message), 0, (struct sockaddr *)&client_addr, client_len);

            break;

            default:

            memset(message,0,80);

            cerr << "Comando no soportado: "<<buffer[0]<<"\n";

            break;

        }



        

      

    }

     freeaddrinfo(res);

     close(sd);

    cerr << "Saliendo... \n";

}
int main(int argc, char **argv){
    if(argc==1){}
    exercise2(argv);
    return 0;
}