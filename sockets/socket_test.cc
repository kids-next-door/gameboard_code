#include "sio_client.h"
#include "sio_message.h"
#include "sio_socket.h"

int main() {
    sio::client h;
    h.connect("http://ec2-3-83-68-92.compute-1.amazonaws.com:4000/");

}
