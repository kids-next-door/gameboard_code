#include <iostream>
#include <mutex>
#include "gameboard.h"

volatile bool up_pressed = false;
volatile bool down_pressed = false;
volatile bool left_pressed = false;
volatile bool right_pressed = false;

std::mutex up_mutex;
std::mutex down_mutex;
std::mutex left_mutex;
std::mutex right_mutex;

//Interrupts
//Just set a signal to say the button was pressed cause want them short
void up_button_isr() {
    up_pressed = true;
}

int main() {

    int exit_code = 0; 
    controller::gameboard &mat = controller::gameboard::getInstance();
    mat.registerISR(controller::gameboard::up_button, &up_button_isr);

    try {
        while(1) {
            if(up_pressed) {
                std::cout << "Up button pressed" << std::endl;

                up_mutex.lock();
                up_pressed = false;
                up_mutex.unlock();
            }
        }
    } 
    catch ( std::runtime_error const &e) {
        std::cerr << "ERROR:: " << e.what() << "; aborting..." << std::endl;
        exit_code = 1;
    }
    catch ( ... ) {
        std::cerr << "ERROR:: Unexpected exception thrown; aborting... " << std::endl;
    }

    return exit_code;
}
