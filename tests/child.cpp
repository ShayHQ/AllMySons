#include <iostream>

int main(){
    int exit;

    std::cin >> exit;
    while (exit){
        std::cout << "Thanks for not exiting " << exit << "\n";
        std::cin >> exit;
    }
    
    return 0;
}