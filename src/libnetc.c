/* This is header file for libnetc. 
 * Distribued with GNU public license.

 * It's used for lightweight usage of sockets
 * wihout any knowledge of network programming.
 * Creators: Martin Vasko and David Bolvansky 
 * EMail: matovidlo2@gmail.com<xvasko12@stud.fit.vutbr.cz>
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
    if(is_icmp) {
        hints.ai_protocol = IPPROTO_ICMP;
    } 
    if(is_udp) {
        hints.ai_socktype = SOCK_DGRAM;
        if(is_raw)
            hints.ai_socktype = SOCK_RAW;
    } else {
        hints.ai_socktype = SOCK_STREAM;
        if(is_raw)
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

void *runner(bool is_concurrent, struct thread_args arguments, void *(*run)(void *)) {
    void *result = NULL;
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
        pthread_create(&libnet_globals.thread_id_glob[libnet_globals.thread_id_counter],
                       NULL, run, (void *)pass_argument);

        libnet_globals.thread_id_counter++;
    } else {
        run((void *)&arguments);
    }
    return result;
}

void joiner(void *(*process_result)(void *)) {
    int pthread_cancel_value = 0;
    void *result;
    for(int i = 0; i < libnet_globals.thread_id_counter; ++i) {
        pthread_cancel_value = pthread_join(libnet_globals.thread_id_glob[i],
                                            &result);
        // Call function only when implemented
        if(process_result != NULL) {
            process_result(result);
        }
        // Probably all of created threads are joined. When yes end this loop.
        if(pthread_cancel_value != 0)
            break;
    }
    libnet_globals.exiting_program = true;
    // TODO: destroy mutex(lock)
}

void *udp_client(bool is_ipv6, bool is_concurrent, const char *ip_address, 
                 uint16_t port, void *(*run)(void *)) {
    struct sockaddr *peer;
    struct addrinfo hints;
    int socket;
    void *result = NULL;
    /* FIXME: setsockopt can be also called */
    hints = initialize_addrinfo(false, true, false);
    char port_number[10];
    sprintf(port_number, "%u", port);
    peer = create_ip_connection(ip_address, port_number, hints, is_ipv6, &socket);
    printf("UDP CLIENT:%d, %s\n", socket, peer->sa_data);
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
void *udp_server(bool is_ipv6, bool is_concurrent, bool get_packet_length, const char*ip_address, uint16_t port, void *(*run)(void *)) {
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
    printf("UDP SERVER:%d, %s, %u\n", socket, peer->sa_data, peer_addr_len);
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

void *tcp_client(bool is_ipv6, bool is_concurrent, const char*ip_address, 
                 uint16_t port, void *(*run)(void *)) {
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
    if(!peer) {
        return result;
    }
    peer_addr_len = sizeof(peer);
    if(connect(socket, peer, peer_addr_len) != -1) {
        perror("Could not connect\n");
        return result;
    }
    signal(SIGINT, signal_handler);
    struct thread_args arguments = {socket, peer};
    /* Before end of client run thread or function without thread. */
    result = runner(is_concurrent, arguments, run);
    return result;
}

// void *tcp_server(bool is_ipv6, bool is_concurrent, const char*ip_address, uint_16t port);
// /* TODO: ICMP server and client */
