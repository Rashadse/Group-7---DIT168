#include <rc_usefulincludes.h>
#include <roboticscape.h>
#include <iostream>

int main() {

    if (rc_initialize()) {
        return -1;
    }
    std::cout << "BLINK YOU ****" << std::endl;

    rc_cleanup();
}