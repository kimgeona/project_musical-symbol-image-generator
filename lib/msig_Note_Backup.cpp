#include <msig_Note.hpp>

namespace msig
{


// msig_Note_Backup.cpp
void Note::save_config(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 기존 symbol_dataset_config.txt 삭제
    if (exists(dir_ds_config)) remove(dir_ds_config);
        
    // 새로운 symbol_dataset_config.txt 생성
    fstream fout(dir_ds_config, ios::out|ios::app);
    if (!fout){
        cout << "msig: ";
        cout << "can't open " << dir_ds_config << " for writing.";
        cout << endl;
        return;
    }
    
    // config 데이터 저장
    for (auto& ms : this->ds_complete){
        string dir = ms.my_dir(ms.dir);
        string config = to_string(ms.x)+"_"+to_string(ms.y)+"_"+to_string(ms.rotate)+"_"+to_string(ms.scale);
        fout << endl << dir + "=" + config;
    }
    for (auto& ms : this->ds_piece){
        string dir = ms.my_dir(ms.dir);
        string config = to_string(ms.x)+"_"+to_string(ms.y)+"_"+to_string(ms.rotate)+"_"+to_string(ms.scale);
        fout << endl << dir + "=" + config;
    }
    fout.close();
}


}
