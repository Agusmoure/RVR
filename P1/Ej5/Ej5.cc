//Agustin Moure Rodriguez
#include <sys/types.h>

#include <sys/socket.h>

#include <netdb.h>

#include <string.h>

#include <unistd.h>

#include <iostream>

using namespace std;


#include <netinet/in.h>

#include <arpa/inet.h>

#include <iostream>

void exercise5( char **argv){

    struct addrinfo hints;

    struct addrinfo* res;



    memset(&hints, 0, sizeof(struct addrinfo));



    hints.ai_family = AF_UNSPEC;



    hints.ai_protocol = 6;





    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);





    if(rc != 0)

    {

        cerr << gai_strerror(rc) << "\n";

        return;

    }



   int sd = socket(res->ai_family, res->ai_socktype,res->ai_protocol);



    int connected = 3;

    connected = connect(sd,  res->ai_addr, res->ai_addrlen);



    if(connected != 0)

    {

        cerr << strerror(connected) << "\n";

    }



    char buffer[80];

    ssize_t bytes = 0 ;



    int c = 1;



    while(c == 1){



        cin >> buffer;

 

        if (buffer[0] == 'q' && buffer[1] == '\0'){

            c = 0;

        }



        else{

        sendto(sd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);

        bytes = recv(sd, (void *) buffer, sizeof(char)*79,0);

        buffer[bytes]= '\0';

        cout << buffer<< "\n";

        }



    }



    freeaddrinfo(res);

}

int main(int argc, char **argv){
    if(argc==1){}
    exercise5(argv);
    return 0;
}