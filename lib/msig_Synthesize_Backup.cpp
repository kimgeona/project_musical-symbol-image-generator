#include <msig_Synthesize.hpp>

namespace msig
{


// msig_Note_Backup.cpp
void Note::save_config(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 기존 symbol_dataset_config.txt 삭제
    if (exists(this->dir_ds_config)) remove(this->dir_ds_config);
        
    // 새로운 symbol_dataset_config.txt 생성
    fstream fout(this->dir_ds_config, ios::out|ios::app);
    if (!fout){
        cout << "msig: ";
        cout << "can't open " << this->dir_ds_config << " for writing.";
        cout << endl;
        return;
    }
    
    // config 데이터 저장
    for (auto& m : this->ds_complete){
        string dir = my_dir(m.second.dir);
        string config = to_string(m.second.x)+"_"+to_string(m.second.y)+"_"+to_string(m.second.rotate)+"_"+to_string(m.second.scale);
        fout << endl << dir + "=" + config;
    }
    for (auto& m : this->ds_piece){
        string dir = my_dir(m.second.dir);
        string config = to_string(m.second.x)+"_"+to_string(m.second.y)+"_"+to_string(m.second.rotate)+"_"+to_string(m.second.scale);
        fout << endl << dir + "=" + config;
    }
    fout.close();
}
void Note::add_config(){
    using namespace std;
    using namespace cv;
    using namespace std::filesystem;
    
    // 기존 내용 읽기
    fstream fin(this->dir_ds_config, ios::in);
    string content = "";
    if (fin){
        string line;
        while (getline(fin, line))
            content += '\n' + line;
    }
    fin.close();
    
    // 기존 symbol_dataset_config.txt 삭제
    if (exists(this->dir_ds_config)) remove(this->dir_ds_config);
        
    // 새로운 symbol_dataset_config.txt 생성
    fstream fout(this->dir_ds_config, ios::out|ios::app);
    if (!fout){
        cout << "msig: ";
        cout << "can't open " << this->dir_ds_config << " for writing.";
        cout << endl;
        return;
    }
    
    // config 데이터 저장
    for (auto& m : this->ds_complete){
        string dir = my_dir(m.second.dir);
        string config = to_string(m.second.x)+"_"+to_string(m.second.y)+"_"+to_string(m.second.rotate)+"_"+to_string(m.second.scale);
        fout << endl << dir + "=" + config;
    }
    for (auto& m : this->ds_piece){
        string dir = my_dir(m.second.dir);
        string config = to_string(m.second.x)+"_"+to_string(m.second.y)+"_"+to_string(m.second.rotate)+"_"+to_string(m.second.scale);
        fout << endl << dir + "=" + config;
    }
    
    // 기존 config 데이터 저장
    fout << endl << endl << content;
    
    fout.close();
}




}