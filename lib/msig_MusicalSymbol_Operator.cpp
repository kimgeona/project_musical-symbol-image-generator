#include <msig_MusicalSymbol.hpp>

namespace msig
{


// msig_MusicalSymbol_Operator.cpp
MusicalSymbol&  MusicalSymbol::operator=(const MusicalSymbol& other) {
    if (this != &other) {
        dir             = other.dir;
        dir_config      = other.dir_config;
        img             = other.img.clone();
        x               = other.x;
        y               = other.y;
        scale           = other.scale;
        rotate          = other.rotate;
    }
    return *this;
}
bool            MusicalSymbol::operator==(const MusicalSymbol& other) {
    return (dir==other.dir && dir_config==other.dir_config);
}


}
