#include <iostream>
#include <mutex>
#include <string>
#include <cctype>
#include <algorithm>
#include <condition_variable>

#include <pthread.h>
#include <unistd.h>
#include <sched.h>

#include "gameboard.h"

#include <sio_client.h>

#define DEBOUNCE_TIME_USECS 500000

volatile bool up_pressed = false;
volatile bool down_pressed = false;
volatile bool left_pressed = false;
volatile bool right_pressed = false;
volatile bool upleft_pressed = false;
volatile bool upright_pressed = false;
volatile bool downleft_pressed = false;
volatile bool downright_pressed = false;

std::mutex up_mutex;
std::mutex down_mutex;
std::mutex left_mutex;
std::mutex right_mutex;
std::mutex upleft_mutex;
std::mutex upright_mutex;
std::mutex downleft_mutex;
std::mutex downright_mutex;

std::mutex cout_mutex;

std::mutex _conn_lock;
std::condition_variable_any _conn_wait;
bool conn_finish = false;

void *up_thread_fn(void*);
void *down_thread_fn(void*);
void *left_thread_fn(void*);
void *right_thread_fn(void*);
void *upleft_thread_fn(void*);
void *upright_thread_fn(void*);
void *downleft_thread_fn(void*);
void *downright_thread_fn(void*);


//Interrupts
//Just set a signal to say the button was pressed cause want them short
void up_button_isr() {
    up_pressed = true;
}
	
void down_button_isr() {
    down_pressed = true;
}

void left_button_isr() {
    left_pressed = true;
}

void right_button_isr() {
    right_pressed = true;
}

void upleft_button_isr() {
    upleft_pressed = true;
}

void upright_button_isr() {
    upright_pressed = true;
}

void downleft_button_isr() {
    downleft_pressed = true;
}

void downright_button_isr() {
    downright_pressed = true;
}

sio::socket::ptr socket_ptr;

void on_connect(){
	_conn_lock.lock();
	_conn_wait.notify_all();
	conn_finish = true;
	_conn_lock.unlock();
}

int main() {
    struct sched_param parameters;
    sched_setscheduler(0, SCHED_RR, &parameters);

    int exit_code = 0; 
	
	std::string room_code;
	std::cout << "Enter a Room Code to Join: " << std::endl;
	do {
		std::cin >> room_code;
	} while( std::find_if_not(room_code.begin(), room_code.end(),isalnum) != room_code.end());
	
	std::string display_name;
	std::cout << "What would you like your display name to be?" << std::endl;
	do {
		std::cin >> display_name;
	} while( std::find_if_not(display_name.begin(), display_name.end(), isalnum) != display_name.end());
	
	sio::client client_conn;
	client_conn.set_open_listener(on_connect);
    client_conn.connect("http://ec2-3-83-68-92.compute-1.amazonaws.com:4000/");
	_conn_lock.lock();
	if(!conn_finish) {
		_conn_wait.wait(_conn_lock);
	}
	_conn_lock.unlock();
	
	socket_ptr = client_conn.socket();
	
	socket_ptr->emit("join room", sio::string_message::create(room_code));
	socket_ptr->emit("set username", sio::string_message::create(display_name));
	
    controller::gameboard &mat = controller::gameboard::getInstance();

    //Set up interrupts for the button pins
    mat.registerISR(controller::gameboard::up_button, &up_button_isr);
    mat.registerISR(controller::gameboard::down_button, &down_button_isr);
    mat.registerISR(controller::gameboard::left_button, &left_button_isr);
    mat.registerISR(controller::gameboard::right_button, &right_button_isr);
	mat.registerISR(controller::gameboard::upleft_button, &upleft_button_isr);
	mat.registerISR(controller::gameboard::upright_button, &upright_button_isr);
	mat.registerISR(controller::gameboard::downleft_button, &downleft_button_isr);
	mat.registerISR(controller::gameboard::downright_button, &downright_button_isr);

    //Create threads for handling interrupts
    pthread_t up_thread;
    pthread_t down_thread;
    pthread_t left_thread;
    pthread_t right_thread;
	pthread_t upleft_thread;
	pthread_t upright_thread;
	pthread_t downleft_thread;
	pthread_t downright_thread;

    int up_err = pthread_create(&up_thread, NULL, up_thread_fn, NULL);
    int down_err = pthread_create(&down_thread, NULL, down_thread_fn, NULL);
    int left_err = pthread_create(&left_thread, NULL, left_thread_fn, NULL);
    int right_err = pthread_create(&right_thread, NULL, right_thread_fn, NULL);
	int upleft_err = pthread_create(&upleft_thread, NULL, upleft_thread_fn, NULL);
	int upright_err = pthread_create(&upright_thread, NULL, upright_thread_fn, NULL);
	int downleft_err = pthread_create(&downleft_thread, NULL, downleft_thread_fn, NULL);
	int downright_err = pthread_create(&downright_thread, NULL, downright_thread_fn, NULL);


    if( up_err || down_err || left_err || right_err ||
		upleft_err || upright_err || downleft_err || downright_err ) {
        std::cout << "ERROR:: error in thread creation; aborting..." << std::endl;
        if(!up_err)
        { pthread_cancel(up_thread); }
        if(!down_err) 
        { pthread_cancel(down_thread); }
        if(!left_err)
        { pthread_cancel(left_thread); }
        if(!right_err)
        { pthread_cancel(right_thread); }
		if(!upleft_err)
        { pthread_cancel(upleft_thread); }
		if(!upright_err)
        { pthread_cancel(upright_thread); }
		if(!downleft_err)
        { pthread_cancel(downleft_thread); }
		if(!downright_err)
        { pthread_cancel(downright_thread); }

        pthread_exit(NULL);
       
    }
	
	//socket_ptr->emit("initialize");
	//client_conn.sync_close();
    //Waits for all threads to exit
    pthread_exit(NULL);
}// End main


void *up_thread_fn(void *arg){

    while(1) {

        if(up_pressed) {

            //Replace this with what I actually want to happen
            cout_mutex.lock();
            std::cout << "Up button pressed" << std::endl;
			cout_mutex.unlock();
					
			socket_ptr->emit("move",sio::string_message::create("up"));

            up_mutex.lock();
            up_pressed = false;
            up_mutex.unlock();

            //This is sleeps the thread for the debounce time
            usleep(DEBOUNCE_TIME_USECS);
        }
        else{
            usleep(100000);
        }

        //Some of the functions used may also be cancellation points
        //But create a manual one for safety
        pthread_testcancel();
    }
}

void *down_thread_fn(void *arg){

    //Do some socket things

    while(1) {

        if(down_pressed) {

            //Replace this with what I actually want to happen
            cout_mutex.lock();
            std::cout << "down button pressed" << std::endl;
            cout_mutex.unlock();
			
			socket_ptr->emit("move",sio::string_message::create("down"));

            down_mutex.lock();
            down_pressed = false;
            down_mutex.unlock();

            //This is sleeps the thread for the debounce time
            usleep(DEBOUNCE_TIME_USECS);
        }
        else{
            usleep(100000);
        }

        //Some of the functions used may also be cancellation points
        //But create a manual one for safety
        pthread_testcancel();
    }
}

void *left_thread_fn(void *arg){		

    //Do some socket things
		
    while(1) {

        if(left_pressed) {

            //Replace this with what I actually want to happen
            cout_mutex.lock();
            std::cout << "Left button pressed" << std::endl;
            cout_mutex.unlock();
			
			socket_ptr->emit("move",sio::string_message::create("left"));

            left_mutex.lock();
            left_pressed = false;
            left_mutex.unlock();

            //This is sleeps the thread for the debounce time
            usleep(DEBOUNCE_TIME_USECS);
        }
        else{
            usleep(100000);
        }

        //Some of the functions used may also be cancellation points
        //But create a manual one for safety
        pthread_testcancel();
    }
}

void *right_thread_fn(void* arg){

    //Do some socket things

    while(1) {

        if(right_pressed) {

            //Replace this with what I actually want to happen
            cout_mutex.lock();
            std::cout << "Right button pressed" << std::endl;
            cout_mutex.unlock();

			socket_ptr->emit("move",sio::string_message::create("right"));
			
            right_mutex.lock();
            right_pressed = false;
            right_mutex.unlock();

            //This is sleeps the thread for the debounce time
            usleep(DEBOUNCE_TIME_USECS);
        }
        else{
            usleep(100000);
        }

        //Some of the functions used may also be cancellation points
        //But create a manual one for safety
        pthread_testcancel();
    }
}
	
void *upleft_thread_fn(void* arg){

    //Do some socket things

    while(1) {

        if(upleft_pressed) {

            //Replace this with what I actually want to happen
            cout_mutex.lock();
            std::cout << "Up Left button pressed" << std::endl;
            cout_mutex.unlock();

			socket_ptr->emit("move",sio::string_message::create("up left"));
			
            upleft_mutex.lock();
            upleft_pressed = false;
            upleft_mutex.unlock();

            //This is sleeps the thread for the debounce time
            usleep(DEBOUNCE_TIME_USECS);
        }
        else{
            usleep(100000);
        }

        //Some of the functions used may also be cancellation points
        //But create a manual one for safety
        pthread_testcancel();
    }
}

void *upright_thread_fn(void* arg){

    //Do some socket things

    while(1) {

        if(upright_pressed) {

            //Replace this with what I actually want to happen
            cout_mutex.lock();
            std::cout << "Up Right button pressed" << std::endl;
            cout_mutex.unlock();

			socket_ptr->emit("move",sio::string_message::create("up right"));
			
            upright_mutex.lock();
            upright_pressed = false;
            upright_mutex.unlock();

            //This is sleeps the thread for the debounce time
            usleep(DEBOUNCE_TIME_USECS);
        }
        else{
            usleep(100000);
        }

        //Some of the functions used may also be cancellation points
        //But create a manual one for safety
        pthread_testcancel();
    }
}	

void *downleft_thread_fn(void* arg){

    //Do some socket things

    while(1) {

        if(downleft_pressed) {

            //Replace this with what I actually want to happen
            cout_mutex.lock();
            std::cout << "Up Left button pressed" << std::endl;
            cout_mutex.unlock();

			socket_ptr->emit("move",sio::string_message::create("down left"));
			
            downleft_mutex.lock();
            downleft_pressed = false;
            downleft_mutex.unlock();

            //This is sleeps the thread for the debounce time
            usleep(DEBOUNCE_TIME_USECS);
        }
        else{
            usleep(100000);
        }

        //Some of the functions used may also be cancellation points
        //But create a manual one for safety
        pthread_testcancel();
    }
}	

void *downright_thread_fn(void* arg){

    //Do some socket things

    while(1) {

        if(downright_pressed) {

            //Replace this with what I actually want to happen
            cout_mutex.lock();
            std::cout << "Down Right button pressed" << std::endl;
            cout_mutex.unlock();

			socket_ptr->emit("move",sio::string_message::create("down right"));
			
            downright_mutex.lock();
            downright_pressed = false;
            downright_mutex.unlock();

            //This is sleeps the thread for the debounce time
            usleep(DEBOUNCE_TIME_USECS);
        }
        else{
            usleep(100000);
        }

        //Some of the functions used may also be cancellation points
        //But create a manual one for safety
        pthread_testcancel();
    }
}	