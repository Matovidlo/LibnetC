/* This is header file for libnetc. 
 * Distribued with GNU public license.

 * It's used for lightweight usage of sockets
 * wihout any knowledge of network programming.
 * Creators: Martin Vasko and David Bolvansky 
 * Mail: matovidlo2@gmail.com/xvasko12@stud.fit.vutbr.cz
 */

#include"libnetc.h"
// extern struct libnetc_globals libnet_globals;

// int socket, struct sockaddr *in_address
void *run_udp_client(void *thread_args) {
    struct thread_args *arguments = (struct thread_args *) thread_args;
    int socket = arguments->socket;
    struct sockaddr *address = arguments->peer;
    socklen_t address_length = sizeof(*address);
    // printf("RunClient socket: %d, address: %s\n", socket, address->sa_data);

    while(is_exiting()) {
        sleep(1);
        char *buffer = malloc(100 * sizeof(char));
        strcpy(buffer, "hello World");
        // printf("%s\n", buffer);
        sendto(socket, (void *)buffer, sizeof(buffer), 0, 
               (struct sockaddr *)address, address_length);
        free(buffer);
    }

    if(socket)
        close(socket);
    if(address)
        free(address);
    if(arguments)
        free(arguments);
    return NULL;
}

void *run_udp_server(void *thread_args) {
    printf("Udp server run\n");
    struct thread_args *arguments = (struct thread_args *) thread_args;
    int socket = arguments->socket;
    struct sockaddr *address = arguments->peer;
    socklen_t address_length = sizeof(*address);
    char *recv = NULL;
    int length = 64;

    while(is_exiting()) {
        if(recv == NULL) {
            recv = malloc(length);
            memset(recv, 0, length);
        }
        int length = recvfrom(socket, recv, length, MSG_PEEK | MSG_TRUNC,
                              (struct sockaddr *)address, &address_length);
        if(length < 0) {
            perror("Recieve of packet failed\n");
        }
        if(recv != NULL) {
            free(recv);
            recv = NULL;
        }
        recv = malloc(length + 1);
        memset(recv, 0, length);
        length = recvfrom(socket, recv, length, 0, address, &address_length);
        recv[length] = '\0';
        sendto(socket, recv, length, 0, address, address_length);
        if(recv != NULL) {
            free(recv);
            recv = NULL;
        }
    }

    if(socket)
        close(socket);
    if(address)
        free(address);
    if(arguments)
        free(arguments);
    return NULL;
}

int main(){
    udp_client(false, true, "127.0.0.1", 1234, run_udp_client);
    joiner(NULL);
    // udp_server(false, true, false, "127.0.0.1", 1234, run_udp_server);
    // joiner(NULL);

    // udp_client(false, true, "127.0.0.1", 1234, run_udp_client);
    return 0;
}