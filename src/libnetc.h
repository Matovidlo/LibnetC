/* This is header file for libnetc. 
 * Distribued with GNU public license.

 * It's used for lightweight usage of sockets
 * wihout any knowledge of network programming.
 * Creators: Martin Vasko and David Bolvansky 
 * Email: matovidlo2@gmail.com/xvasko12@stud.fit.vutbr.cz
 **/
#ifndef LIBCNET
#define LIBCNET

#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/* Include boolean lib for bool return value from functions */
#include<stdbool.h>
#include<errno.h>
/* Signal handling libs */
#include<signal.h>
#include<unistd.h>
#include<pthread.h>
#include<fcntl.h>
#include"../log.c/src/log.h"

#ifdef DEBUG
#define DEBUG_PRINT 1
#else
#define DEBUG_PRINT 0
#endif
/* macro for number of threads variable. */
#define NUMBER_OF_THREAD 16384
#define PDU_LENGTH 1024
#define RECIEVE_TIMEOUT_S 1
#define RECIEVE_TIMEOUT_US 0
#define debug_print(...) \
            do { if (DEBUG_PRINT) log_debug(__VA_ARGS__); } while (0)

typedef void *(*callback_fn)(void *);

struct thread_args {
    int socket;
    struct sockaddr *peer;
    socklen_t peer_addr_length;
};

/* Globals for libned for killing and slowly detaching resources */
struct libnetc_globals {
    bool running_program;                       /* Detect wheter program should 
                                                   be exited (signal catched)*/
    int thread_id_counter;                      /* Thread identifier counter */
    pthread_mutex_t lock;                       /* Lock for changing 
                                                   sensitive values of threads.*/
    pthread_t thread_id_glob[NUMBER_OF_THREAD]; /* All created thread. 
                                                   Used for joiner/pthread_detach mostly*/
};

/* Signal catcher. Set's is_exiting variable when reporting Ctrl+C. */
void signal_handler(int signal_number);

/*
 * Create ipv4 or ipv6 connection based on value of parameter
 * This function should be private within class context 
 **/

struct addrinfo initialize_addrinfo(bool is_icmp, bool is_udp, bool is_raw);
struct sockaddr *create_ip_connection(const char *node, const char *port,
                                      struct addrinfo hints, bool is_ipv6,
                                      int *socket, socklen_t *peer_len);

/*
 * Used when singal handler is reached. Slowly end infinite while loop
 * before detaching all of resources in concurent thread.
 **/
bool is_exiting();

/* 
 * Runs thread on separate thread_id. Last thread_id is number of 
 * global variable 'thread_id_counter' 
 **/
void *runner(bool is_concurrent, struct thread_args arguments, void *(*run)(void *));

/* 
 * This is support function for get recieved packet length.
 * Mostly it should be used when we need to know how many bytes of recieved message comes.
 * Be carefull, it has a higher complexity than guessing or saying how many bytes should come.
 **/
unsigned long long UDP_recieved_packet_legth(int socket, struct sockaddr *address);
unsigned long long TCP_recieved_packet_legth(int socket);

/* 
 * Joiner joins all of created threads till they end or signal SIGINT is pressed.
 * This is not needed when no concurrent threads are created.
 * (no client or server has argument is_concurrent set on true). This is good
 * only when part of clients and servers has to be run and after that other
 * computation is executed.
 **/
void **joiner(void *(*process_result)(void *));

/* 
 * Return null, when could not establish any addres or 
 * when could not send any message. Run method gives in void * -> struct thread_args.
 * There is need to retype to int socket and struct sockaddr *peer.
 **/
void *udp_client(bool is_ipv6, bool is_concurrent, const char*ip_address,
                 uint16_t port, void *(*run)(void *));
void *tcp_client(bool is_ipv6, bool is_concurrent, const char*ip_address,
                 uint16_t port, void *(*run)(void *));
void *imcp_client(bool is_ipv6, bool is_concurrent, const char*ip_address,
                 uint16_t port, void *(*run)(void *));
void *udp_server(bool is_ipv6, bool is_concurrent, const char*ip_address,
                 uint16_t port, void *(*run)(void *));
void *tcp_server(bool is_ipv6, bool is_concurrent, const char*ip_address,
                 uint16_t port, void *(*run)(void *));
void *imcp_server(bool is_ipv6, bool is_concurrent, const char*ip_address,
                 uint16_t port, void *(*run)(void *));

/* This function should be overriden when c/c++ */
void *run_udp_server(void *thread_args);
void *run_udp_client(void *thread_args);
void *run_tcp_server(void *thread_args);
void *run_tcp_client(void *thread_args);
void *run_icmp_server(void *thread_args);
void *run_icmp_client(void *thread_args);

#endif
