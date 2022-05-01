#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos _type, _nick y _message en el buffer _data
    char * tmp = _data;

    memcpy(tmp, &_type , sizeof(uint8_t));
    tmp += sizeof(uint8_t);

    // _nick[NICK_SIZE] = '\0';
    memcpy(tmp, _nick.c_str(), sizeof(char) * NICK_SIZE);
    tmp += sizeof(char) * NICK_SIZE;
    // _message[TEXT_SIZE] = '\0';
    memcpy(tmp, _message.c_str(), TEXT_SIZE * sizeof(char) );
    // tmp += TEXT_SIZE *sizeof(char);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    _size = MESSAGE_SIZE;

    char * tmp = _data;

    memcpy(&_type, tmp , sizeof(uint8_t));
    tmp += sizeof(uint8_t);
    _nick=tmp;
    // memcpy(&_nick[0], tmp, sizeof(char) * NICK_SIZE);
     tmp += sizeof(char) * NICK_SIZE;
    // _nick[NICK_SIZE] = '\0';
    _message=tmp;
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
        Socket* outSocket=&socket; 

        //Recibir Mensajes en y en función del tipo de mensaje
        socket.recv(cmsg,outSocket);
        std::unique_ptr<Socket> cliente(outSocket); 
        switch(cmsg._type){
        // - LOGIN: Añadir al vector clients
            case ChatMessage::LOGIN/*LOGIN*/:{

            clients.push_back(std::move(cliente));
            std::cout<< cmsg._nick<<" LOGIN"<<std::endl;
            break;
            }
    // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        case ChatMessage::MESSAGE:{

            for(auto it= clients.begin(); it!= clients.end(); ++it)
            {
                
                if(!(**it==*cliente)){
            std::cout<< cmsg._nick<<" ha enviado un mensaje"<<std::endl;

                    socket.send(cmsg,**it);
                }
            }
            break;
        }

        // - LOGOUT: Eliminar del vector clients
        case ChatMessage::LOGOUT:{
            for(auto it= clients.begin(); it!= clients.end(); ++it)
            {
                if(**it==*cliente){
                    std::cout<< cmsg._nick<<" se ha desconectado"<<std::endl;
                    clients.erase(it);
                    break;
                }
            }
            break;
        }
        default:
                        std::cerr << "ERROR tipo de mensaje no reconocido\n";
            break;
        }
            
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
    while (true)
    {
        // Leer stdin con std::getline
        std::string msg; 
        std::getline(std::cin, msg);
        // Enviar al servidor usando socket
        ChatMessage em(_nick, msg);
        em._type = ChatMessage::MESSAGE;
        
        socket.send(em, socket);
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        ChatMessage em;
         
        socket.recv(em);

        //Mostrar en pantalla el mensaje de la forma "_nick: mensaje"
        std::cout << em._nick << ": " << em._message << std::endl;
    }
}

