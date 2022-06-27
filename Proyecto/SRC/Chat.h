#include <string>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <memory>
#include <list>
#include "Serializable.h"
#include "Socket.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/**
 *  Mensaje del protocolo de la aplicación de Chat
 *
 *  +-------------------+
 *  | Tipo: uint8_t     | 0 (login), 1 (mensaje), 2 (logout)
 *  +-------------------+
 *  | Nick: char[8]     | Nick incluido el char terminación de cadena '\0'
 *  +-------------------+
 *  |                   |
 *  | Mensaje: char[80] | Mensaje incluido el char terminación de cadena '\0'
 *  |                   |
 *  +-------------------+
 *
 */
class ChatMessage : public Serializable
{
public:
    static const size_t MESSAGE_SIZE = sizeof(char) * (88) /*Suma de NICK_SIZE + TEXT_SIZE*/ + sizeof(uint8_t);

    enum MessageType
    {
        LOGIN = 0,
        MESSAGE = 1,
        LOGOUT = 2,
        START = 3,
        WORDSELECTED = 4,
        GUESS = 5,
        FINISH = 6,
        REJECTED =7
    };

    ChatMessage(){};

    ChatMessage(const std::string &n, const std::string &m) : _nick(n), _message(m){};

    void to_bin();

    int from_bin(char *bobj);

    uint8_t _type;

    std::string _nick;
    std::string _message;

private:
    const int TEXT_SIZE = 80;
    const int NICK_SIZE = 8;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 *  Clase para el servidor de chat
 */
class ChatServer
{
public:
    ChatServer(const char *s, const char *p) : socket(s, p)
    {
        socket.bind();
    };

    /**
     *  Thread principal del servidor recive mensajes en el socket y
     *  lo distribuye a los clientes. Mantiene actualizada la lista de clientes
     */
    void do_messages();

private:
    /**
     *  Lista de clientes conectados al servidor de Chat, representados por
     *  su socket
     */
    std::vector<std::unique_ptr<Socket>> clients;

    /**
     * Socket del servidor
     */
    Socket socket;
    std::unique_ptr<Socket> host;
    int turn = 0;
    bool started = false;
    bool selected = false;
    int fails=0;
    int corrects=0;
    bool finish();
    void guessLetter(char c);
    std::string word;
    std::list<char> correctLetters;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 *  Clase para el cliente de chat
 */
class ChatClient
{
public:
    /**
     * @param s dirección del servidor
     * @param p puerto del servidor
     * @param n _nick del usuario
     */
    ChatClient(const char *s, const char *p, const char *n) : socket(s, p),
                                                              _nick(n){};

    /**
     *  Envía el mensaje de login al servidor
     */
    void login();

    /**
     *  Envía el mensaje de logout al servidor
     */
    void logout();

    /**
     *  Rutina principal para el Thread de E/S. Lee datos de STDIN (std::getline)
     *  y los envía por red vía el Socket.
     */
    void input_thread();

    /**
     *  Rutina del thread de Red. Recibe datos de la red y los "renderiza"
     *  en STDOUT
     */
    void net_thread();

private:
    void printMan(int fails);
    void print(int fails);
    void guessLetter(char c);
    /**
     * Socket para comunicar con el servidor
     */
    Socket socket;

    /**
     * Nick del usuario
     */
    std::string _nick;
    int fails = 0;
    bool started = false;
    bool playing = false;
    bool finish=false;
    std::vector<char> status;
    std::vector<char> failsC;
    std::string word;
};
