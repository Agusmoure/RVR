#include "Serializable.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * n, int16_t x, int16_t y):_x(x),_y(y)
    {
        strncpy(_name, n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t total=MAX_NAME*sizeof(char)+2*sizeof(int16_t);
        alloc_data(total);
        char *tmp=_data;
        memcpy(tmp,_name,MAX_NAME);
        tmp += MAX_NAME * sizeof(char);
        memcpy(tmp,&_x,sizeof(_x));
        tmp+=sizeof(_x);
        memcpy(tmp,&_y,sizeof(_y));
    }

    int from_bin(char * data)
    {
        char * tmp = data;

        memcpy(_name, tmp, MAX_NAME);
        tmp += MAX_NAME * sizeof(char);

        memcpy(&_x, tmp, sizeof(_x));
        tmp += sizeof(_x);

        memcpy(&_y, tmp, sizeof(_y));
        tmp += sizeof(_y);

        return 0;
    }
    const char* getName(){return _name;}
    void setName(const char * n){ strncpy(_name, n, MAX_NAME);}
    int getX(){return _x;}
    int getY(){return _y;}
    void setX(int x){_x=x;}
    void setY(int y){ _y=y;}
private:
    static const size_t MAX_NAME=20;
    char _name[MAX_NAME];

    int16_t _x;
    int16_t _y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 12, 98);

    // 1. Serializar el objeto one_w
    one_w.to_bin();
    // 2. Escribir la serialización en un fichero
    std::fstream fs;
    fs.open("PLAYER_ONE",std::fstream::out);
    fs.write(one_w.data(),one_w.size());
    fs.close();
    // 3. Leer el fichero
    char data[one_w.size()];

    fs.open("Player_ONE", std::fstream::in);

    fs.read(data, one_w.size());

    fs.close();
    // 4. "Deserializar" en one_r
    one_r.from_bin(data);

    // 5. Mostrar el contenido de one_r
    std::cout << "Name: "<< one_r.getName() << std::endl;
    std::cout << "X: "<< one_r.getX() << std::endl;
    std::cout << "Y: "<< one_r.getY() << std::endl;

    return 0;
}

