/* This is header file for libnetc. 
 * Distribued with GNU public license.

 * It's used for lightweight usage of sockets
 * wihout any knowledge of network programming.
 * Creators: Martin Vasko and David Bolvansky 
 * EMail: matovidlo2@gmail.com<xvasko12@stud.fit.vutbr.cz>
 */

#include"libnetc.h"

// Global variables
// Maybe use one global structure with those types.
static pthread_t thread_id_glob[NUMBER_OF_THREAD];
static int thread_id_counter = 0;
static bool exiting_program = true;
static pthread_mutex_t lock;


struct addrinfo initialize_addrinfo(bool is_icmp, bool is_udp, bool is_raw){
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = hints.ai_flags;
    hints.ai_protocol = 0;
    if(is_icmp){
        hints.ai_protocol = IPPROTO_ICMP;
    } 
    if(is_udp){
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

/* TODO: Maybe do filling with sockaddr_storage */
// struct sockaddr fill_ipv4(struct addrinfo *rp, uint_16 port){
//     struct sockaddr_in peer_addr;

//     bzero((char *) &peer_addr, sizeof(peer_addr));
//     peer_addr->sin_family = rp->ai_family;
//     bcopy((char *) peer->h_addr, (char *)&peer_addr.sin_addr.s_addr, peer->h_length);
//     peer_addr->sin_port = htons(port);
//     return peer_addr;
// }

// struct sockaddr fill_ipv6(struct addrinfo *rp, uint_16 port){
//     struct sockaddr_in6 peer_addr6;
//     int status;
//     memset(&peer_addr6, 0, szeof(peer_addr6));
//     if((status = inet_pton(AF_INET6, rp->ai_addr)))

// }

struct sockaddr *create_ip_connection(const char *node, const char *port, struct addrinfo hints, bool is_ipv6, int *client_sock){
    struct addrinfo *result, *iterator;
    struct sockaddr *peer_addr = NULL;
    int response;

    response = getaddrinfo(node, port, &hints, &result);
    if (response != 0){
        perror("Getaddrinfo failed!");
        return peer_addr;
    }
    for(iterator = result; iterator != NULL; iterator = iterator->ai_next){
        // TODO:: copy sockaddr
        peer_addr = iterator->ai_addr;
        if(!peer_addr){
            perror("No peer address found!");
            break;
        }
        if(is_ipv6 && iterator->ai_family == AF_INET6){
            (*client_sock) = socket(iterator->ai_family, iterator->ai_socktype, iterator->ai_protocol);
            if(!(*client_sock)){
                perror("Socket could not be created!");
                break;
            }
        } else if(!is_ipv6 && iterator->ai_family == AF_INET) {
            (*client_sock) = socket(iterator->ai_family, iterator->ai_socktype, iterator->ai_protocol);
            if(!(*client_sock)){
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
        pthread_mutex_lock(&lock);
        exiting_program = false;
        pthread_mutex_unlock(&lock);
    }
}

bool is_exiting() {
    return exiting_program;
}

void *runner(bool is_concurrent, struct thread_args arguments, void *(*run)(void *)) {
    void *result = NULL;
    if(is_concurrent) {
        // TODO: maybe only 1 init
        if(pthread_mutex_init(&lock, NULL)) {
            perror("Could not create mutex lock\n");
            return result;
        }
        pthread_create(&thread_id_glob[thread_id_counter], NULL, run, (void *)&arguments);
        pthread_join(thread_id_glob[thread_id_counter++], &result);
        // pthread_detach(thread_id_glob[thread_id_counter++]);
    } else {
        run((void *)&arguments);
    }
    return result;
}

void *udp_client(bool is_ipv6, bool is_concurrent, const char *ip_address, uint16_t port, void *(*run)(void *)){
    struct sockaddr *peer;
    struct addrinfo hints;
    int socket;
    void *result = NULL;
    /* FIXME: setsockopt can be also called */
    hints = initialize_addrinfo(false, true, false);
    char port_number[10];
    sprintf(port_number, "%u", port);
    peer = create_ip_connection(ip_address, port_number, hints, is_ipv6, &socket);
    if(!peer){
        return result;
    }
    signal(SIGINT, signal_handler);
    struct thread_args arguments = {socket, peer};
    /* Before end of client run thread or function without thread. */
    result = runner(is_concurrent, arguments, run);
    return result;
}

void *tcp_client(bool is_ipv6, bool is_concurrent, const char*ip_address, uint16_t port, void *(*run)(void *)) {
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
    if(!peer){
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
// void *udp_server(bool is_ipv6, bool is_concurrent, const char*ip_address, uint_16t port);
// void *tcp_server(bool is_ipv6, bool is_concurrent, const char*ip_address, uint_16t port);
// /* TODO: ICMP server and client */
