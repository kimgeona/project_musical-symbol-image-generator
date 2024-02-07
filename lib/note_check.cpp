#include <note.hpp>

namespace note
{


bool Page::check_buffer_count(){
    if ((buffer_type.size() == buffer_pitch.size()) &&
        (buffer_pitch.size() == buffer_beat.size()) &&
        (buffer_beat.size() == buffer_clef.size()) &&
        (buffer_clef.size() == buffer_time.size()))
        return true;
    else return false;
}
bool Page::check_measure(){}

bool Page::check(){
    if (check_buffer_count()) return true;
    if (check_measure()) return true;
    return false;
}

}
