#include <iostream>
#include <cstdint>
#include <chrono>
#include <thread>

/**
 * Test for time delay values and how to convert them.
 */

int main() {
    uint64_t time = 1000;
    std::chrono::milliseconds duration(time);
    
    std::cout << "Time was = " << unsigned(time) << std::endl;
    std::cout << "Sleeping for that duration..." << std::endl;
    std::this_thread::sleep_for(duration);
    std::cout << "Finished sleeping!" << std::endl;
}
