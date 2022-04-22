//Agustin Moure Rodriguez
#include <sys/types.h>

#include <sys/socket.h>

#include <netdb.h>

#include <string.h>

#include <unistd.h>

#include <iostream>


#include <netinet/in.h>

#include <arpa/inet.h>

#include <string>

#include <thread>

#include <vector>

using namespace std;

class Thread_TCP{

    public:

    Thread_TCP(int _sd) {

        sd = _sd;

    }

    ~Thread_TCP(){};

    void crea_mensaje(){



        //Rececpcion mensaje cliente



        char buffer[80];

        char host[NI_MAXHOST];

        char service[NI_MAXSERV];



        ssize_t bytes;



        while(bytes > 0){



            bytes = recv(sd,(void*)buffer,79 * sizeof(char), 0);

            buffer[bytes]= '\0';

            if(bytes >0){

            cout << "Thread número: " <<this_thread::get_id() << " recibiendo mensaje\n";

            }

            sleep(1);



            send(sd, (void*)buffer, strlen(buffer), 0);



        }

        cout << "Cerrando el thread número: " << this_thread::get_id() << "\n";

        close(sd);

    }

    

    bool active = true;

    private:

    socklen_t client_len = sizeof(struct sockaddr_in);

    int sd;

};

void exercise7( char **argv){

    struct addrinfo hints;

    struct addrinfo *res;



    //INICIALIZACION SOCKET Y BIND 



    memset(&hints, 0 ,sizeof(struct addrinfo));



    hints.ai_family = AF_INET;

    hints.ai_socktype = SOCK_STREAM;



    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);



    if(rc != 0){

        cerr << "getaddrinfo: " << gai_strerror(rc) << "\n";

        return;

    }



    //res contiene la representacion como sockaddr de dirección + PUERTO



    int sd =  socket(res->ai_family, res->ai_socktype, res->ai_protocol);



    if(bind(sd,res->ai_addr,res->ai_addrlen) != 0)

    {

        cerr << "bind: \n";

        return;

    }

    freeaddrinfo(res);



    struct sockaddr_in client_addr;

    socklen_t client_len = sizeof(struct sockaddr_in);



    listen(sd,16);

    while(true)

    {

        int sd_client = accept(sd, (struct sockaddr *) &client_addr, &client_len); 

        Thread_TCP tcp(sd_client);

        cout << "Conexion desde IP: " << inet_ntoa(client_addr.sin_addr) << " PUERTO: " << ntohs(client_addr.sin_port) << "\n";

        thread t(&Thread_TCP::crea_mensaje,tcp);

        t.detach();

    }

}

int main(int argc, char **argv){
    if(argv==1){}
    exercise7(argv);
    return 0;
}