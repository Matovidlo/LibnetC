/* This is header file for libnetc. 
 * Distribued with GNU public license.

 * It's used for lightweight usage of sockets
 * wihout any knowledge of network programming.
 * Creators: Martin Vasko and David Bolvansky 
 * EMail: matovidlo2@gmail.com/xvasko12@stud.fit.vutbr.cz
 **/
#ifndef LIBCNET
#define LIBCNET

#include<sys/types.h>
#include<sys/socket.h>
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

typedef void *(*callback_fn)(void *);

struct thread_args {
    int socket;
    struct sockaddr *peer;
};

/* macro for number of threads variable. */
#define NUMBER_OF_THREAD 16384
struct libnetc_globals {
    bool exiting_program;
    int thread_id_counter;
    pthread_mutex_t lock;
    pthread_t thread_id_glob[NUMBER_OF_THREAD];
};
/* global variable to detect program exiting state, mutex
 * save all of the threads and it's counter.
 **/
static struct libnetc_globals libnet_globals = {true, 0, PTHREAD_MUTEX_INITIALIZER, {0, }};

/* Signal catcher. Set's is_exiting variable when reporting Ctrl+C. */
void signal_handler(int signal_number);

/*
 *  Create ipv4 or ipv6 connection based on value of parameter
 * This function should be private within class context 
 **/

struct addrinfo initialize_addrinfo(bool is_icmp, bool is_udp, bool is_raw);
// struct sockaddr_in fill_ipv4(struct addrinfo *rp, struct hostent *peer, uint_16 port);
// struct sockaddr_in6 fill_ipv6(struct addrinfo *rp, struct hostent *peer, uint_16 port);
struct sockaddr *create_ip_connection(const char *node, const char *port,
                                      struct addrinfo hints, bool is_ipv6,
                                      int *socket);

/*
 * Used when singal handler is reached. Slowly end infinite while loop
 * before detaching all of resources in concurent thread.
 **/
bool is_exiting();

/* Runs thread on separate thread_id. Last thread_id is number of 
 * global variable 'thread_id_counter' 
 **/
void *runner(bool is_concurrent, struct thread_args arguments, void *(*run)(void *));
/* Joiner joins all of created threads till they end or signal SIGINT is pressed.
 * This is not needed when no concurrent threads are created.
 * (no client or server has argument is_concurrent set on true). This is good
 * only when part of clients and servers has to be run and after that other
 * computation is executed.
 * !!! IMPORTANT !!!
 * Joiner discards all of return values from threads when no function passed as argument.
 **/
void joiner(void *(*process_result)(void *));

/* 
 * Return null, when could not establish any addres or 
 * when could not send any message. Run method gives in void * -> struct thread_args.
 * There is need to retype to int socket and struct sockaddr *peer.
 **/
void *udp_client(bool is_ipv6, bool is_concurrent, const char*ip_address,
                 uint16_t port, void *(*run)(void *));
void *tcp_client(bool is_ipv6, bool is_concurrent, const char*ip_address,
                 uint16_t port, void *(*run)(void *));
void *udp_server(bool is_ipv6, bool is_concurrent, bool get_packet_length, 
                 const char*ip_address, uint16_t port, void *(*run)(void *));
void *tcp_server(bool is_ipv6, bool is_concurrent, const char*ip_address,
                 uint16_t port, void *(*run)(void *));
/* TODO: ICMP server and client */

/* This function should be overriden when c++ */
void *run_udp_server(void *thread_args);
void *run_udp_client(void *thread_args);

#endif
