#include "Card.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char **argv)
{
    Card carta=Card(Card::CardColor::MULTICOLOR,Card::CardType::CHANGECOLOR);
    Message msg= Message("Agus",carta);
    Message msg2= Message();
    // 1. Serializar el objeto one_w
    msg.to_bin();
    // 2. Escribir la serialización en un fichero
    std::fstream fs;
    fs.open("PLAYER_ONE",std::fstream::out);
    fs.write(msg.data(),msg.size());
    fs.close();
    // 3. Leer el fichero
    char data[msg.size()];

    fs.open("Player_ONE", std::fstream::in);

    fs.read(data, msg.size());

    fs.close();
    // 4. "Deserializar" en one_r
    msg2.from_bin(data);

    // 5. Mostrar el contenido de one_r
    std::cout<<"Nick"<<msg2.getNick()<<std::endl;
    std::cout<<"Card"<<msg2.getCardType()<<std::endl;
    std::cout<<"Color"<<msg2.getColor()<<std::endl;
    return 0;
}

