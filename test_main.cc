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

std::mutex up_mutex;
std::mutex down_mutex;
std::mutex left_mutex;
std::mutex right_mutex;
std::mutex cout_mutex;

std::mutex _conn_lock;
std::condition_variable_any _conn_wait;
bool conn_finish = false;

void *up_thread_fn(void*);
void *down_thread_fn(void*);
void *left_thread_fn(void*);
void *right_thread_fn(void*);


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

    //Create threads for handling interrupts
    pthread_t up_thread;
    pthread_t down_thread;
    pthread_t left_thread;
    pthread_t right_thread;

    int up_err = pthread_create(&up_thread, NULL, up_thread_fn, NULL);
    int down_err = pthread_create(&down_thread, NULL, down_thread_fn, NULL);
    int left_err = pthread_create(&left_thread, NULL, left_thread_fn, NULL);
    int right_err = pthread_create(&right_thread, NULL, right_thread_fn, NULL);

    if( up_err || down_err || left_err || right_err) {
        std::cout << "ERROR:: error in thread creation; aborting..." << std::endl;
        if(!up_err)
        { pthread_cancel(up_thread); }
        if(!down_err) 
        { pthread_cancel(down_thread); }
        if(!left_err)
        { pthread_cancel(left_thread); }
        if(!right_err)
        { pthread_cancel(right_thread); }

        pthread_exit(NULL);
       
    }
 
    //Waits for all threads to exit
    pthread_exit(NULL);
}// End main


void *up_thread_fn(void *socket_connection){

    while(1) {

        if(up_pressed) {

            //Replace this with what I actually want to happen
            cout_mutex.lock();
            std::cout << "Up button pressed" << std::endl;
            cout_mutex.unlock();

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

void *down_thread_fn(void *socket_connection){

    //Do some socket things

    while(1) {

        if(down_pressed) {

            //Replace this with what I actually want to happen
            cout_mutex.lock();
            std::cout << "down button pressed" << std::endl;
            cout_mutex.unlock();

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

void *left_thread_fn(void *socket_connection){

    //Do some socket things

    while(1) {

        if(left_pressed) {

            //Replace this with what I actually want to happen
            cout_mutex.lock();
            std::cout << "Left button pressed" << std::endl;
            cout_mutex.unlock();

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

void *right_thread_fn(void *socket_connection){

    //Do some socket things

    while(1) {

        if(right_pressed) {

            //Replace this with what I actually want to happen
            cout_mutex.lock();
            std::cout << "Right button pressed" << std::endl;
            cout_mutex.unlock();

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
