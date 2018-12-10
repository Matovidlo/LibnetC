# LibnetC
# Build success
TODO Travis
# Usage
LibnetC is used for leightweight usage of network principles like
client-server model on both ipv4/ipv6 implementation for UNIX/Linux
based systems. Also you can use both TCP or UDP client/server model
just by overriding run function.
This allows us to implement concurrent server or client where
responses are processed by one thread and sending is solved by
another thread.
# Simple use
#include"libnet.h"
For including libnet features. Then you are allowed to use functions.
Simply call should do all necessieties of creating connection to remote
server/client.
# Example
Let's assume this small program.
`udp_client(false, true, "127.0.0.0", 1234, run_udp_client);`
This creates udp based client where he connects to 127.0.0.1. First false means that we want an
ipv4 client. The 2nd parameter which is true means that we want concurrent client, so there is
option to run client method on separate thread.
All we need is to `override` function `void *run_udp_client(void *thread_args);`.
We can assume just empty thread doing infinite while loop.
```
void *run_udp_client(void *thread_args){
    printf("Udp client run\n");
    while(is_exiting()) {
        ;
    }
    printf("Udp client end\n");
    return NULL;
}
```
This code is handling our own implementation of concurrent processing between client and some server
located on 127.0.0.1 with opened port 1234. It could be any type of comunication from HTTP trough SSH
or something else. Thread arguments has 2 things inside. The socket value and struct sockaddr address to which
we can use `sendto(...)` function.
## While loop examination
While loop has special thing inside. This is due to signal processing and slow deatach of resources. When we use
this type of construction after send signal SIGINT(Ctrl+C) the client end while loop, do everything 
necessary of dealocation and return NULL. This is the best thing that we can provide some result after the end of program.

Isn't it very simple??
I think it is!
# Testing


LibnetC is lightweight network library for C/C++.
