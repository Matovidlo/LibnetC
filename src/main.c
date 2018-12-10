/* This is header file for libnetc. 
 * Distribued with GNU public license.

 * It's used for lightweight usage of sockets
 * wihout any knowledge of network programming.
 * Creators: Martin Vasko and David Bolvansky 
 * Mail: matovidlo2@gmail.com/xvasko12@stud.fit.vutbr.cz
 */

#include"libnetc.h"

// int socket, struct sockaddr *in_address
void *run_udp_client(void *thread_args){
    printf("Udp client run\n");
    while(is_exiting()) {
        ;
    }
    printf("Udp client end\n");
    return NULL;
}

int main(){
    udp_client(false, true, "127.0.0.0", 1234, run_udp_client);
    udp_client(false, true, "127.0.0.0", 1234, run_udp_client);
    return 0;
}