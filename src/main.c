/* This is header file for libnetc. 
 * Distribued with GNU public license.

 * It's used for lightweight usage of sockets
 * wihout any knowledge of network programming.
 * Creators: Martin Vasko and David Bolvansky 
 * Email: matovidlo2@gmail.com/xvasko12@stud.fit.vutbr.cz
 */

#include"libnetc.h"

void *run_udp_client(void *thread_args) {
    struct thread_args *arguments = (struct thread_args *) thread_args;
    int socket = arguments->socket;
    struct sockaddr *address = arguments->peer;
    socklen_t address_length = sizeof(*address);

    while(is_exiting()) {
        sleep(1);
        char *buffer = malloc(100 * sizeof(char));
        strcpy(buffer, "This is an extra long long message, that could not be readed at once");
        sendto(socket, (void *)buffer, strlen(buffer), 0, 
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
    struct thread_args *arguments = (struct thread_args *) thread_args;
    int socket = arguments->socket;
    struct sockaddr *address = arguments->peer;
    socklen_t address_length = sizeof(*address);
    char *recv = NULL;

    while(is_exiting()) {
        // Get incomming packet length
        int length = UDP_recieved_packet_legth(socket, address);
        if(recv == NULL) {
            recv = malloc(length + 1);
            memset(recv, 0, length);
        }
        // change length of truncenated message to make more efficient reading and sending
        length = recvfrom(socket, recv, length, 0, address, &address_length);
        recv[length] = '\0';
        sendto(socket, recv, length, 0, address, address_length);
        if(recv != NULL) {
            printf("%s\n", recv);
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
    int *result = malloc(sizeof(int));
    *result = 130;
    return result;
}

void udp_client_server() {
    udp_client(false, true, "127.0.0.1", 1234, run_udp_client);
    udp_server(false, true, "127.0.0.1", 1234, run_udp_server);
}

void *run_tcp_client(void *thread_args) {
    struct thread_args *arguments = (struct thread_args *) thread_args;
    int socket = arguments->socket;
    struct sockaddr *address = arguments->peer;
    socklen_t peer_addr_len = arguments->peer_addr_length;

    if(connect(socket, address, peer_addr_len) == -1) {
        perror("Could not connect\n");
        return NULL;
    }

    while(is_exiting()) {
        sleep(1);
        char *buffer = malloc(100 * sizeof(char));
        strcpy(buffer, "This is an extra long long message, that could not be readed at once");
        send(socket, (void *)buffer, strlen(buffer), 0);
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

void *run_tcp_server(void *thread_args) {
    struct thread_args *arguments = (struct thread_args *) thread_args;
    int socket = arguments->socket;
    struct sockaddr *address = arguments->peer;
    socklen_t peer_addr_len = sizeof(*address);
    char *recv_buffer = NULL;

    int connection_socket = accept(socket, address, &peer_addr_len);
    if(connection_socket < 0) {
        perror("TCP server accept failed!\n");
    }

    while(is_exiting()) {
        int length = TCP_recieved_packet_legth(connection_socket);
        if(recv_buffer == NULL) {
            recv_buffer = malloc(length + 1);
            memset(recv_buffer, 0, length + 1);
        }
        // change length of truncenated message to make more efficient reading and sending
        length = recv(connection_socket, recv_buffer, length, 0);
        recv_buffer[length] = '\0';
        send(connection_socket, recv_buffer, length, 0);
        if(recv_buffer != NULL) {
            printf("%s\n", recv_buffer);
            free(recv_buffer);
            recv_buffer = NULL;
        }
    }

    if(connection_socket)
        close(connection_socket);
    if(address)
        free(address);
    if(arguments)
        free(arguments);
    int *result = malloc(sizeof(int));
    *result = 127;
    return result;
}

void tcp_client_server() {
    // IPv6 server and client
    tcp_server(true, true, "::1", 1235, run_tcp_server);
    tcp_client(true, true, "::1", 1235, run_tcp_client);
}

void result_process() {
    void **results;
    // Use joiner to join threads
    results = joiner(NULL);
    // Get results
    for(int i=0; i < 4; ++i) {
        void *retval = results[i];
        if(retval != NULL) {
            printf("Server ended with: %d\n", *((int *)retval));
            free(retval);
        }
    }
    free(results);
}

int main(){
    udp_client_server();
    tcp_client_server();
    result_process();
    return 0;
}