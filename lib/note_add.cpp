#include <note.hpp>

namespace note
{


void Page::add_type(bool type){
    buffer_type.push_back(type);
}
void Page::add_pitch(string pitch){
    buffer_pitch.push_back(pitch);
}
void Page::add_beat(string beat){
    buffer_beat.push_back(beat);
}
void Page::add_clef(string clef){
    buffer_clef.push_back(clef);
}
void Page::add_time(string time){
    buffer_time.push_back(time);
}
void Page::add(bool type, string pitch, string beat, string clef, string time){
    add_type(type);
    add_pitch(pitch);
    add_beat(beat);
    add_clef(clef);
    add_time(time);
}


}
