#include "gameboard.h"

namespace controller {

    //Constructor
    gameboard :: gameboard() {
        //Initialize wiringPi library
        wiringPi::wiringPiSetup();

        //Set the pins we want to use to inputs
        //Bc we don't know what state they were left in
        wiringPi::pinMode( up_button, INPUT );
        wiringPi::pinMode( down_button, INPUT );
        wiringPi::pinMode( left_button, INPUT );
        wiringPi::pinMode( right_button, INPUT ); 
		wiringPi::pinMode( upleft_button, INPUT);
		wiringPi::pinMode( upright_button, INPUT);
		wiringPi::pinMode( downleft_button, INPUT);
		wiringPi::pinMode( downright_button, INPUT);
		
        //Enable the internal pull down resistors
        wiringPi::pullUpDnControl(up_button, PUD_DOWN );
        wiringPi::pullUpDnControl(down_button, PUD_DOWN );
        wiringPi::pullUpDnControl(left_button, PUD_DOWN );
        wiringPi::pullUpDnControl(right_button, PUD_DOWN );
		wiringPi::pullUpDnControl(upleft_button, PUD_DOWN );
		wiringPi::pullUpDnControl(upright_button, PUD_DOWN );
		wiringPi::pullUpDnControl(downleft_button, PUD_DOWN );
		wiringPi::pullUpDnControl(downright_button, PUD_DOWN );
    } // End constructor

    //Destructor
    gameboard :: ~gameboard() {
        //Undo the things we did with the constructor
        wiringPi::pinMode( up_button, INPUT );
        wiringPi::pinMode( down_button, INPUT );
        wiringPi::pinMode( left_button, INPUT );
        wiringPi::pinMode( right_button, INPUT ); 
		wiringPi::pinMode( upleft_button, INPUT );
		wiringPi::pinMode( upright_button, INPUT );
		wiringPi::pinMode( downleft_button, INPUT );
		wiringPi::pinMode( downright_button, INPUT );

        wiringPi::pullUpDnControl(up_button, PUD_OFF );
        wiringPi::pullUpDnControl(down_button, PUD_OFF );
        wiringPi::pullUpDnControl(left_button, PUD_OFF );
        wiringPi::pullUpDnControl(right_button, PUD_OFF );
		wiringPi::pullUpDnControl(upleft_button, PUD_OFF );
		wiringPi::pullUpDnControl(upright_button, PUD_OFF );
		wiringPi::pullUpDnControl(downleft_button, PUD_OFF );
		wiringPi::pullUpDnControl(downright_button, PUD_OFF );

    }

    //Return static instance of class
    gameboard & gameboard :: getInstance() {
        static gameboard instance;
        return instance;
    }

}
