#ifndef CAPSTONE_GAMEBOARD_H
#define CAPSTONE_GAMEBOARD_H

namespace wiringPi {
    #include <wiringPi.h>
}

namespace controller {
class gameboard {
    public:
        //Singleton design pattern
        static gameboard& getInstance();

        //Destructor
        ~gameboard();

        //Button pin wiringPi mappings
        enum button_t {
            //White wires
            up_button = 22,
            //Green Wires
            down_button = 21,
            //Blue
            left_button = 24,
            //Red 
            right_button = 23,
            upleft_button = 26,
			upright_button = 27,
            downleft_button = 28,
            downright_button = 29
        };

        //Wrapper for registering wiringPi ISRs
        inline int registerISR(button_t b, void (*isr)()) {
            wiringPi::wiringPiISR(b, INT_EDGE_RISING,isr);
        }

    private:
        //Default constructor is private so only access is through getInstance()
        gameboard();

        //Delete copy constructor so can't create another by copying
        gameboard(gameboard const &) =  delete;
        void operator=(gameboard const &) = delete;

}; //end class
} //end namespace

#endif 
