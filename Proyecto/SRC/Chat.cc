#include "Chat.h"
#include <algorithm>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos _type, _nick y _message en el buffer _data
    char *tmp = _data;

    memcpy(tmp, &_type, sizeof(uint8_t));
    tmp += sizeof(uint8_t);

    // _nick[NICK_SIZE] = '\0';
    memcpy(tmp, _nick.c_str(), sizeof(char) * NICK_SIZE);
    tmp += sizeof(char) * NICK_SIZE;
    // _message[TEXT_SIZE] = '\0';
    memcpy(tmp, _message.c_str(), TEXT_SIZE * sizeof(char));
    // tmp += TEXT_SIZE *sizeof(char);
}

int ChatMessage::from_bin(char *bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    _size = MESSAGE_SIZE;

    char *tmp = _data;

    memcpy(&_type, tmp, sizeof(uint8_t));
    tmp += sizeof(uint8_t);
    _nick = tmp;
    // memcpy(&_nick[0], tmp, sizeof(char) * NICK_SIZE);
    tmp += sizeof(char) * NICK_SIZE;
    // _nick[NICK_SIZE] = '\0';
    _message = tmp;
    // memcpy(&_message[0], tmp, sizeof(char) * TEXT_SIZE);
    // tmp += sizeof(char) * TEXT_SIZE;
    // _message[TEXT_SIZE] = '\0';

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */
        ChatMessage cmsg;
        Socket *outSocket = &socket;

        //Recibir Mensajes en y en función del tipo de mensaje
        socket.recv(cmsg, outSocket);
        std::unique_ptr<Socket> cliente(outSocket);
        switch (cmsg._type)
        {
            // - LOGIN: Añadir al vector clients
        case ChatMessage::LOGIN /*LOGIN*/:
        {
            if (!started)
            {
                clients.push_back(std::move(cliente));
                std::cout << cmsg._nick << " LOGIN" << std::endl;
            }
            else
            {
                std::cout << cmsg._nick << " F" << std::endl;
                ChatMessage msg = ChatMessage("server", "Losiento la partida ya ha empezado");
                msg._type = ChatMessage::REJECTED;
                socket.send(msg, *cliente);
            }
            break;
        }
            // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor) si el juego no ha empezado
        case ChatMessage::MESSAGE:
        {
            if (started && !selected && *cliente == *host)
            {
                word = cmsg._message;
                selected = true;
                std::cout << cmsg._nick << " ha elegido la palabra " << word << std::endl;
                for (auto it = clients.begin(); it != clients.end(); ++it)
                {

                    if (!(**it == *cliente))
                    {
                        ChatMessage msg = ChatMessage("server", "La palabra ha sido elegida");
                        msg._type = ChatMessage::MESSAGE;
                        socket.send(msg, **it);
                        //enviar la palabra
                        cmsg._type = ChatMessage::WORDSELECTED;
                        socket.send(cmsg, **it);
                    }
                }
            }
            else
                for (auto it = clients.begin(); it != clients.end(); ++it)
                {

                    if (!(**it == *cliente))
                    {
                        std::cout << cmsg._nick << " ha enviado un mensaje" << std::endl;

                        socket.send(cmsg, **it);
                    }
                }
            break;
        }

        // - LOGOUT: Eliminar del vector clients
        case ChatMessage::LOGOUT:
        {
            for (auto it = clients.begin(); it != clients.end(); ++it)
            {
                if (**it == *cliente)
                {
                    std::cout << cmsg._nick << " se ha desconectado" << std::endl;
                    clients.erase(it);
                    break;
                }
            }
            if (clients.empty())
                return;
            break;
        }
        case ChatMessage::START:
        {
            if (clients.size() >= 2)
            {

                started = true;
                for (auto it = clients.begin(); it != clients.end(); ++it)
                {
                    ChatMessage msg = ChatMessage("server", "aaa");
                    msg._type = ChatMessage::START;
                    socket.send(msg, **it);
                }
                host = std::move(cliente);
                std::cout << "el host es: " << cmsg._nick << std::endl;
                ChatMessage msg = ChatMessage("server", "elija una palabra");
                msg._type = ChatMessage::MESSAGE;
                socket.send(msg, *host);
                std::cout << "aqui\n";
                if (*host == *clients[0])
                {
                    turn = 1;
                }
            }
            else
            {
                ChatMessage msg = ChatMessage("server", "espera a que haya más jugadores");
                msg._type = ChatMessage::MESSAGE;
                socket.send(msg, *cliente);
            }
            break;
        }
        case ChatMessage::GUESS:
        {
            if (*cliente == *clients.at(turn))
            {
                guessLetter(cmsg._message[0]);
                if (!finish())
                {
                    //Le toca a ese jugador
                    turn = ((turn + 1) % clients.size());
                    if (*clients[turn] == *host)
                    { //si el siguiente cliente es el host se lo salta
                        turn = ((turn + 1) % clients.size());
                    }
                    for (auto it = clients.begin(); it != clients.end(); ++it)
                    {
                        if (!(**it == *host))
                        {

                            std::cout << cmsg._nick << " ha enviado una letra" << std::endl;
                            socket.send(cmsg, **it);
                        }
                        if (*clients[turn] == **it)
                        {
                            ChatMessage msg("Server", "Es tu turno");
                            msg._type = ChatMessage::MESSAGE;
                            socket.send(msg, **it);
                        }
                        else
                        {
                            if (!(**it == *host))
                            {
                                ChatMessage msg("Server", "No es tu turno");
                                msg._type = ChatMessage::MESSAGE;
                                socket.send(msg, **it);
                            }
                        }
                    }
                }
                else
                {
                    std::cout << "ENDGAME\n";
                    std::string txt = fails >= 7 ? "Losiento habeis perdido la palabra era: " + word : "Enhorabuena habeis adivinado la palabra: " + word;
                    ChatMessage msg("Server", txt);
                    ChatMessage msg2("Server", "Tus amigos han terminado la partida");
                    msg._type = ChatMessage::FINISH;
                    msg2._type = ChatMessage::FINISH;
                    for (auto it = clients.begin(); it != clients.end(); ++it)
                        if (!(**it == *host))
                            socket.send(msg, **it);
                        else
                            socket.send(msg2, **it);
                }
            }
            else
            {
                ChatMessage msg("Server", "Espera tu turno");
                msg._type = ChatMessage::MESSAGE;
                socket.send(msg, *cliente);
            }
            break;
        }
        default:
            std::cerr << "ERROR tipo de mensaje no reconocido\n";
            break;
        }
    }
}
bool ChatServer::finish()
{
    std::cout << "fails " << fails << " corrects " << corrects << "\n";
    return (fails >= 7 || corrects >= word.size());
}
void ChatServer::guessLetter(char c)
{
    bool findone = false;
    for (size_t i = 0; i < word.size(); i++)
    {
        if (word[i] == c)
        {
            auto it = std::find(correctLetters.begin(), correctLetters.end(), c);
            if (it == correctLetters.end()) //Decir una letra correcta ya dicha cuenta como pasar sino mira cuantas son correctas
            {
                for (size_t i = 0; i < word.size(); i++)
                {
                    if (word[i] == c)
                    {
                        std::cout << "entra en correctas con " << word[i] << "=" << c << std::endl;
                        corrects++;
                    }
                }
                correctLetters.push_back(c);
            }

            findone = true;
        }
    }
    if (!findone)
    {
        fails++;
    }
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(_nick, msg);
    em._type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar
    std::string msg;

    ChatMessage em(_nick, msg);
    em._type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}
void ChatClient::input_thread()
{
    signal(SIGINT, SIG_IGN);
    while (!finish)
    {
        if (playing)
        {
            std::cout << "\x1B[2J\x1B[H";
            print(fails);
            std::cout << "Cual es la siguiente letra que deseas intentar\n";
            std::string c;
            std::cin >> c;
            ChatMessage em(_nick, c);
            em._type = ChatMessage::GUESS;
            if (!finish) //por si ha cambiado mientras esperaba input
                socket.send(em, socket);
        }
        else
        {

            // Leer stdin con std::getline
            std::string msg;
            std::getline(std::cin, msg);
            // Enviar al servidor usando socket
            ChatMessage em(_nick, msg);
            if (!started && msg == "start")
                em._type = ChatMessage::START;
            else
                em._type = ChatMessage::MESSAGE;
            if (!playing) //por si ha cambiado mientras esperaba input
                socket.send(em, socket);
        }
    }
    if (finish)
        logout();
    std::cout << "Termina input\n";
}

void ChatClient::net_thread()
{
    while (!finish)
    {
        //Recibir Mensajes de red
        ChatMessage em;

        socket.recv(em);

        //Mostrar en pantalla el mensaje de la forma "_nick: mensaje"
        if (em._type == ChatMessage::MESSAGE || em._type == ChatMessage::REJECTED)
        {
            if (!playing)
                std::cout << em._nick << ": " << em._message << std::endl;
            else
            {
                if (em._nick == "Server")
                    std::cout << em._message << std::endl;
            }
        }
        if (em._type == ChatMessage::START)
        {
            std::cout << em._nick << ": "
                      << "ha empezado la partida" << std::endl;
            started = true;
        }
        if (em._type == ChatMessage::WORDSELECTED)
        {
            word = em._message;
            playing = true;
            std::cout << "Pulsa Enter para empezar\n";
            for (size_t i = 0; i < word.size(); i++)
            {
                status.push_back('-');
            }
        }
        if (em._type == ChatMessage::GUESS)
        {
            guessLetter(em._message[0]);
            std::cout << "\x1B[2J\x1B[H";
            print(fails);
        }
        if (em._type == ChatMessage::FINISH)
        {
            finish = true;
            std::cout << "\x1B[2J\x1B[H";
            std::cout << em._nick << ": " << em._message << std::endl;
        }
    }
    std::cout << "Termina net\n";
}
void ChatClient::printMan(int fails)
{
    std::cout << " _________\n"
              << "|         | \n";
    if (fails < 1)
    {
        std::cout << "|\n";
    }
    else
        std::cout << "|         0\n";
    if (fails < 2)
    {
        std::cout << "|\n";
    }
    else
    {
        if (fails >= 4)
            std::cout << "|       --|--\n";
        else if (fails >= 3)
            std::cout << "|       --|\n";
        else
            std::cout << "|         |\n";
    }
    if (fails < 5)
        std::cout << "|\n";
    else
    {
        if (fails >= 7)
            std::cout << "|        / \\\n";
        else if (fails >= 6)
            std::cout << "|        / \n";
    }
    std::cout << "|\n";

    std::cout << "|\n"
              << "|_________\n";
}
void ChatClient::guessLetter(char c)
{
    std::cout << "Letra " << c << "\n";
    bool findone = false;
    for (size_t i = 0; i < word.size(); i++)
    {
        if (word[i] == c)
        {
            status[i] = c;
            std::cout << "esta\n";
            findone = true;
        }
    }
    if (!findone)
    {
        std::cout << "no esta\n";
        fails++;
        failsC.push_back(c);
    }
}
void ChatClient::print(int fails)
{
    printMan(fails);
    std::cout << "Palabra a adivinar: ";
    for (size_t i = 0; i < status.size(); i++)
    {
        std::cout << status[i];
    }
    std::cout << "\n";
    std::cout << " Letras falladas: ";
    for (size_t i = 0; i < failsC.size(); i++)
    {
        std::cout << failsC[i] << " ";
    }
    std::cout << "\n";
}