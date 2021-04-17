#include <iostream>

int main(){
    int age;

    std::cin >> age;

    std::cout << "Your age is " << age << "\n";

    std::cin >> age;

    std::cout << "Your age again is " << age << "\n";
    
    throw std::runtime_error("Hiiiiii i am error\n");
    return 0;
}