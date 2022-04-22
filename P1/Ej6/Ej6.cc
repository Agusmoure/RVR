//Agustin Moure Rodriguez
#include <sys/types.h>

#include <sys/socket.h>

#include <netdb.h>

#include <string.h>

#include <unistd.h>

#include <iostream>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <pthread.h>

#include <string>

#include <thread>

#include <vector>

#include <time.h>

using namespace std;

class Thread_UDP{

    public:

    Thread_UDP(int _sd) {

        sd = _sd;

    }

    ~Thread_UDP(){};

    void crea_mensaje(){



    //Rececpcion mensaje cliente



    char buffer[80];

    char host[NI_MAXHOST];

    char service[NI_MAXSERV];



    time_t rawtime;

    struct tm * timeinfo;

    ssize_t bytes;



    char message[80];



    struct sockaddr_in client_addr;

    socklen_t client_len = sizeof(struct sockaddr_in);



    while(active){



    bytes = recvfrom(sd,buffer,79 * sizeof(char), 0,(struct sockaddr *) &client_addr, &client_len);

    buffer[bytes]= '\0';

    if(bytes != -1){



        getnameinfo((struct sockaddr *)&client_addr, client_len,host,NI_MAXHOST,service,NI_MAXSERV,NI_NUMERICHOST | NI_NUMERICSERV);

        cerr << bytes << " bytes de " << inet_ntoa(client_addr.sin_addr)<< ":" << ntohs(client_addr.sin_port) << "\n";



    }

    

    switch(buffer[0])

    {

        case 't':

        time(&rawtime);

        timeinfo = localtime (&rawtime);

        strftime(message,80, "%r",timeinfo);

        break;

        case 'd':

        time(&rawtime);

        timeinfo = localtime (&rawtime);

        strftime(message,80, "%F",timeinfo);

        break;

        default:

        memset(message,'-',1);

        cerr << "Comando no soportado X" << "\n";

        break;

    }



    cout << "Thread número: " <<this_thread::get_id() << "\n";

    sleep(2);



    sendto(sd, message, strlen(message), 0, (struct sockaddr *)&client_addr, client_len);

    }



    }

    

    bool active = true;

    private:

    int sd;

};

void exercise6( char **argv){

    struct addrinfo hints;

    struct addrinfo *res;



    //INICIALIZACION SOCKET Y BIND 



    memset(&hints, 0 ,sizeof(struct addrinfo));



    hints.ai_family = AF_INET;

    hints.ai_socktype = SOCK_DGRAM;



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



    Thread_UDP udp(sd);



    //POOL DE THREADS



    vector<thread> pool;



    for(int i = 0;i<5;++i)

    {

        pool.push_back(thread(&Thread_UDP::crea_mensaje,udp));

    }



    char c[80];

    do{

    cin >> c;

    } while (c[0]!= 'q');



    udp.active = false;



    for(auto &t:pool){

        t.detach();

    }

    

    close(sd);

    

}

int main(int argc,char **argv){
    if(argc==1){}
    exercise6(argv);
    return 0;

}