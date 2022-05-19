#include "Message.h"
Message::Message(const std::string&n, Card& c) {
    _nick=n;
    _color=c.getColor();
    _cardType=c.getCardType();}

void Message::to_bin(){
    int32_t total=NICK_SIZE+3*sizeof(uint8_t);
    alloc_data(total);
    char *tmp=_data;
    //add type to data
    memcpy(tmp,&_type,sizeof(_type));
    tmp+=sizeof(_type);
    //add player nick to data
    memcpy(tmp,_nick.c_str(),sizeof(char)*NICK_SIZE);
    tmp += NICK_SIZE * sizeof(char);
    //add color to data
    memcpy(tmp,&_color,sizeof(_color));
    tmp+=sizeof(_color);
    //add typecard to data
    memcpy(tmp,&_cardType,sizeof(_cardType));
}

int Message::from_bin(char* bobj){

    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    _size = MESSAGE_SIZE;

    char * tmp = _data;

    memcpy(&_type, tmp , sizeof(uint8_t));
    tmp += sizeof(uint8_t);
    memcpy(&_nick,tmp,NICK_SIZE*sizeof(char));
    tmp+=sizeof(char)*NICK_SIZE;
    memcpy(&_color,tmp,sizeof(uint8_t));
    tmp+=sizeof(uint8_t);
    memcpy(&_cardType,tmp,sizeof(uint8_t));
    tmp+=sizeof(uint8_t);
}