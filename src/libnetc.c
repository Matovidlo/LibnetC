/* This is header file for libnetc. 
 * Distribued with GNU public license.

 * It's used for lightweight usage of sockets
 * wihout any knowledge of network programming.
 * Creators: Martin Vasko and David Bolvansky 
 * Email: matovidlo2@gmail.com<xvasko12@stud.fit.vutbr.cz>
 **/

#include"libnetc.h"

// Global variables
extern struct libnetc_globals libnet_globals;


struct addrinfo initialize_addrinfo(bool is_icmp, bool is_udp, bool is_raw) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = hints.ai_flags;
    hints.ai_protocol = 0;
    if (is_icmp) {
        hints.ai_protocol = IPPROTO_ICMP;
    }
    if (is_udp) {
        hints.ai_socktype = SOCK_DGRAM;
        if (is_raw)
            hints.ai_socktype = SOCK_RAW;
    } else {
        hints.ai_socktype = SOCK_STREAM;
        if (is_raw)
            hints.ai_socktype = SOCK_RAW;
    }
    return hints;
}

struct sockaddr *create_ip_connection(const char *node, const char *port, 
                                      struct addrinfo hints, bool is_ipv6, 
                                      int *client_sock) {
    struct addrinfo *result, *iterator;
    struct sockaddr *peer_addr = malloc(sizeof(struct sockaddr));
    int response;

    response = getaddrinfo(node, port, &hints, &result);
    if (response != 0) {
        perror("Getaddrinfo failed!");
        return peer_addr;
    }
    for(iterator = result; iterator != NULL; iterator = iterator->ai_next) {
        // peer_addr = iterator->ai_addr;
        memcpy(peer_addr, iterator->ai_addr, sizeof(struct sockaddr));
        if(!peer_addr) {
            perror("No peer address found!");
            break;
        }
        if(is_ipv6 && iterator->ai_family == AF_INET6) {
            (*client_sock) = socket(iterator->ai_family,
                                    iterator->ai_socktype,
                                    iterator->ai_protocol);
            if(!(*client_sock)){
                perror("Socket could not be created!");
                break;
            }
        } else if(!is_ipv6 && iterator->ai_family == AF_INET) {
            (*client_sock) = socket(iterator->ai_family,
                                    iterator->ai_socktype,
                                    iterator->ai_protocol);
            if(!(*client_sock)) {
                perror("Socket could not be created!");
                break;
            }
        }
        // TODO: need probably bind to check wether connnection is correct.
    }

    // if (iterator == NULL) {
    //     perror("Could not find any address\n");
    //     return peer_addr;
    // }
    freeaddrinfo(result);
    return peer_addr;
}

void signal_handler(int signal_number) {
    if(signal_number == SIGINT) {
        pthread_mutex_lock(&(libnet_globals.lock));
        libnet_globals.exiting_program = false;
        pthread_mutex_unlock(&(libnet_globals.lock));
    }
    // Cancel all created (detached) threads when SIGNINT sent.
    // int pthread_cancel_value = 0;
    // for(int i = 0; i < NUMBER_OF_THREAD; ++i) {
    //     pthread_cancel_value = pthread_cancel(thread_id_glob[i]);
    //     if(pthread_cancel_value != 0)
    //         break;
    // }
}

// Return exiting_program state.
bool is_exiting() {
    return libnet_globals.exiting_program;
}

void *runner(bool is_concurrent, struct thread_args arguments, callback_fn run) {
    void *result = NULL;
    int return_value;
    // No function passed inside
    if(run == NULL)
        return result;
    if(is_concurrent) {
        if(pthread_mutex_init(&(libnet_globals.lock), NULL)) {
            perror("Could not create mutex lock\n");
            return result;
        }
        struct thread_args *pass_argument = malloc(sizeof(struct thread_args));
        memcpy(pass_argument, &arguments, sizeof(struct thread_args));
        return_value = pthread_create(&libnet_globals.thread_id_glob[libnet_globals.thread_id_counter],
                                      NULL, run, (void *)pass_argument);
        if(return_value != 0)
            perror("Pthread_create failed!\n");

        libnet_globals.thread_id_counter++;
    } else {
        result = run((void *)&arguments);
    }
    return result;
}

unsigned long long UDP_recieved_packet_legth(int socket, struct sockaddr *address) {
    socklen_t address_length = sizeof(*address);
    int length = 1024; // This sould be maximum amount of one packet
    char *recieved_string = malloc(length);
    memset(recieved_string, 0, length);
    int recieved_length = length;
    fd_set rd_flag;
    // set timeval to 1 second
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    // select socket to read from it. When no data timeout.
    fcntl(socket, F_GETFD, 0);
    FD_ZERO(&rd_flag);
    FD_SET(socket, &rd_flag);
    int retval = select(socket + 1, &rd_flag, NULL, (fd_set *)0, &timeout);

    if(retval){
        recieved_length = recvfrom(socket, recieved_string, length,
                                    MSG_PEEK | MSG_TRUNC, 
                                    (struct sockaddr *)address, &address_length);
        if(recieved_length < 0) {
            perror("Recieve of packet failed\n");
        }
    } else {
        printf("No data within 1 second\n");
    }
    
    free(recieved_string);
    return recieved_length;
}

unsigned long long TCP_recieved_packet_legth(int socket) {
    int length = 1024; // This sould be maximum amount of one packet
    char *recieved_string = malloc(length);
    memset(recieved_string, 0, length);
    int recieved_length = length;
    fd_set rd_flag;
    // set timeval to 1 second
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    // select socket to read from it. When no data timeout.
    fcntl(socket, F_GETFD, 0);
    FD_ZERO(&rd_flag);
    FD_SET(socket, &rd_flag);
    int retval = select(socket + 1, &rd_flag, NULL, (fd_set *)0, &timeout);

    if(retval){
        recieved_length = recv(socket, recieved_string, length,
                               MSG_PEEK | MSG_TRUNC);
        if(recieved_length < 0) {
            perror("Recieve of packet failed\n");
        }
    } else {
        printf("No data within 1 second\n");
    }
    
    free(recieved_string);
    return recieved_length;
}

void **joiner(callback_fn process_result) {
    int pthread_cancel_value = 0;
    void **array_results = (void **) malloc(sizeof(void **) * 1024);
    memset(array_results, 0, sizeof(*array_results));
    void *result;

    for(int i = 0; i < libnet_globals.thread_id_counter; ++i) {
        pthread_cancel_value = pthread_join(libnet_globals.thread_id_glob[i],
                                            &result);
        // Call function only when implemented
        if(process_result != NULL) {
            result = process_result(result);
        }
        // Set result to array
        array_results[i] = result;
        // Probably all of created threads are joined. When yes end this loop.
        if(pthread_cancel_value != 0)
            break;
    }
    libnet_globals.exiting_program = true;
    // TODO: destroy mutex(lock)
    return array_results;
}

/**
 * UDP Client and server implementation
 */
void *udp_client(bool is_ipv6, bool is_concurrent, const char *ip_address, 
                 uint16_t port, callback_fn run) {
    struct sockaddr *peer;
    struct addrinfo hints;
    int socket;
    void *result = NULL;
    /* FIXME: setsockopt can be also called */
    hints = initialize_addrinfo(false, true, false);
    char port_number[10];
    sprintf(port_number, "%u", port);
    peer = create_ip_connection(ip_address, port_number, hints, is_ipv6, &socket);
    // Log information
    // printf("UDP CLIENT:%d, %s\n", socket, peer->sa_data);
    if(!peer) {
        return result;
    }
    signal(SIGINT, signal_handler);
    struct thread_args arguments = {socket, peer};
    /* Before end of client run thread or function without thread. */
    result = runner(is_concurrent, arguments, run);
    return result;
}

// Maybe do IPv6 and IPv4 socket together
// TODO:
void *udp_server(bool is_ipv6, bool is_concurrent, const char*ip_address,
                 uint16_t port, callback_fn run) {
    struct sockaddr *peer;
    socklen_t peer_addr_len;
    struct addrinfo hints;
    int socket;
    void *result = NULL;
    /* FIXME: setsockopt can be also called */
    hints = initialize_addrinfo(false, true, false);
    char port_number[10];
    sprintf(port_number, "%u", port);
    peer = create_ip_connection(ip_address, port_number, hints, is_ipv6, &socket);
    
    // int optval = 0;
    // setsockopt(socket, SOL_IPV6, IPV6_V6ONLY, &optval, sizeof(int));
    if(!peer) {
        return result;
    }
    peer_addr_len = sizeof(*peer);
    // Log information
    // printf("UDP SERVER:%d, %s, %u\n", socket, peer->sa_data, peer_addr_len);
    if(bind(socket, peer, peer_addr_len) == -1) {
        perror("Could not bind socket\n");
        return result;
    }
    signal(SIGINT, signal_handler);
    struct thread_args arguments = {socket, peer};
    /* Before end of client run thread or function without thread. */
    result = runner(is_concurrent, arguments, run);
    return result;
}

/**
 * TCP client and server implementation
 */
void *tcp_client(bool is_ipv6, bool is_concurrent, const char*ip_address, 
                 uint16_t port, callback_fn run) {
    struct sockaddr *peer;
    struct addrinfo hints;
    int socket;
    void *result = NULL;

    hints = initialize_addrinfo(false, false, false);
    char port_number[10];
    sprintf(port_number, "%u", port);
    peer = create_ip_connection(ip_address, port_number, hints, is_ipv6, &socket);
    if(!peer) {
        return result;
    }
    /*
     * NOTE:
     * This code could be used only in separated TCP client server connection!
     */
    // socklen_t peer_addr_len;
    // peer_addr_len = sizeof(peer);
    // if(connect(socket, peer, peer_addr_len) == -1) {
    //     perror("Could not connect\n");
    //     return result;
    // }
    
    signal(SIGINT, signal_handler);
    struct thread_args arguments = {socket, peer};
    /* Before end of client run thread or function without thread. */
    result = runner(is_concurrent, arguments, run);
    return result;
}

void *tcp_server(bool is_ipv6, bool is_concurrent, const char*ip_address,
                 uint16_t port, callback_fn run) {
    struct sockaddr *peer;
    socklen_t peer_addr_len;
    struct addrinfo hints;
    int socket;
    void *result = NULL;
    /* FIXME: setsockopt can be also called */
    hints = initialize_addrinfo(false, false, false);
    char port_number[10];
    sprintf(port_number, "%u", port);
    peer = create_ip_connection(ip_address, port_number, hints, is_ipv6, &socket);
    
    // int optval = 0;
    // setsockopt(socket, SOL_IPV6, IPV6_V6ONLY, &optval, sizeof(int));
    if(!peer) {
        return result;
    }
    peer_addr_len = sizeof(*peer);
    // Log information
    // printf("TCP Server:%d, %s, %u\n", socket, peer->sa_data, peer_addr_len);
    // Bind to socket
    if(bind(socket, peer, peer_addr_len) == -1) {
        perror("TCP Server could not bind socket\n");
        return result;
    }
    // Listen to socket
    if(listen(socket, 5) == -1) {
        perror("TCP server could not listen to socket!\n");
        return result;
    }
    /* NOTE:
     * This code could be used only on separated server connection!
     */
    // Accept connection from client
    // int connection_socket = accept(socket, address, &peer_addr_len);
    // if(connection_socket < 0) {
    //     perror("TCP server accept failed!\n");
    // }

    signal(SIGINT, signal_handler);
    struct thread_args arguments = {socket, peer};

    /* Before end of client run thread or function without thread. */
    result = runner(is_concurrent, arguments, run);
    return result;
}
