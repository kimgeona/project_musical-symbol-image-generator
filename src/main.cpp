#include <iostream>
#include <opencv2/opencv.hpp>
#include <msig.hpp>

int main(){
    std::cout << "Making Muscial-Symbols Dataset" << std::endl;
    
    // create Note
    msig::Note n = msig::Note();
    
    // show Note
    n.show();
    
    // return none error
    return 0;
}
