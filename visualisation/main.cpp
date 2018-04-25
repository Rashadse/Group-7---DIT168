#include <iostream>
#include <map>
#include <fstream>
#include <string>

#include "visualisation/visualisation.hpp"


using namespace std;
int main(int /*argc*/, char** /*argv*/) {
   
    shared_ptr<VIZService> vizService = make_shared<VIZService>("192.168.43.212", "7");
		
    using namespace std::chrono_literals;
    while (true) {        
        // delay
        std::this_thread::sleep_for(500ms);
    		


	}
}
