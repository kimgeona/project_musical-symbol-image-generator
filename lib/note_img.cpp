#include <note.hpp>

namespace msig
{


void Note::save_as_img(string file_name){}
void Note::show(){}

Mat Note::cv_Mat(){
    return draw();
}


}
