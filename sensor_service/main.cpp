#include "roboticscape.h"
#include <iostream>

int main() {
    if(rc_initialize()){
        std::cout << "BLINK YOU ****" << std::endl;
        return -1;
    }
    rc_blink();

    rc_cleanup();
}