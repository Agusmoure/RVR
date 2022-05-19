#include <string>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <memory>

#include "Serializable.h"
/**
 *  Mensaje del protocolo de la aplicación de Chat
 *  +-------------------+
 *  | Tipo: uint8_t     | Tipo de carta (número correspondiente o carta especial)
 *  +-------------------+
 *  | Nick: std::string | Tipo de carta (número correspondiente o carta especial)
 *  +-------------------+
 *  | Color: uint8_t    | Color de la carta ya sea rojo, azul, verde, amarillo o multicolor
 *  +-------------------+
 *  |TipoCarta: uint8_t | Tipo de carta (número correspondiente o carta especial)
 *  +-------------------+
 */
class Message:public Serializable
{
public:
    static const size_t MESSAGE_SIZE = sizeof(char) * (8) + sizeof(uint8_t)*3;
    enum MessageType
    {
        LOGIN   = 0,
        MESSAGE = 1,
        LOGOUT  = 2
    };

    Message(){};

    Message(const std::string& n, const uint8_t& color,const uint8_t& cardType):_nick(n),_color(color),_cardType(cardType){};

    Message(const std::string&n, Card& card);
    void to_bin();

    int from_bin(char * bobj);
    std::string getNick(){return _nick;};
    uint8_t getColor(){return _color;};
    uint8_t getCardType(){return _cardType;};
    private:
    uint8_t _type;
    std::string _nick;
    uint8_t _color;
    uint8_t _cardType;
    const int NICK_SIZE=8;
};