#include "roboticscape.h"

int main() {
    if(rc_initialize()){
        fprintf(stderr,"ERROR: failed to run rc_initialize(), are you root?\n");
        return -1;
    }
    rc_blink();

    rc_cleanup();
}