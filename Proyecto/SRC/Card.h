#include <string>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <memory>
class Card
{
public:
    enum CardColor:uint8_t
    {
        RED   = 0,
        GREEN = 1,
        BLUE  = 2,
        YELLOW =3,
        MULTICOLOR=4
    };
    enum CardType:uint8_t{
        ZERO=0,
        UNO=1,
        TWO=2,
        THREE=3,
        FOUR=4,
        FIVE=5,
        SIX=6,
        SEVEN=7,
        EIGHT=8,
        NINE=9,
        PLUS2=10,
        STOP=11,
        REVERSE=12,
        PLUS4=13,
        CHANGECOLOR=14
    };
    Card(){};

    Card(const CardColor& color, CardType& type):_color(color),_type(type){};
    Card(const uint8_t& color, const uint8_t type):_color(color),_type(type){};
    const uint8_t getColor(){return _color;}
    const uint8_t getCardType(){return _type;}
    const CardColor getColorC(){return static_cast<CardColor>(_color);}
    const CardType getCardT(){return static_cast<CardType>(_type);}
    private:
    uint8_t _color;
    uint8_t _type;
};
