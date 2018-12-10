/* This is header file for libnetc. 
 * Distribued with GNU public license.

 * It's used for lightweight usage of sockets
 * wihout any knowledge of network programming.
 * Creators: Martin Vasko and David Bolvansky 
 * Mail: matovidlo2@gmail.com/xvasko12@stud.fit.vutbr.cz
 */
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

struct thread_args {
    int socket;
    struct sockaddr *peer;
};
/* global thread_id variable */
#define NUMBER_OF_THREAD 16384

/* Signal catcher. Set's is_exiting variable when reporting Ctrl+C. */
void signal_handler(int signal_number);

/*
 *  Create ipv4 or ipv6 connection based on value of parameter
 * This function should be private within class context 
 */

struct addrinfo initialize_addrinfo(bool is_icmp, bool is_udp, bool is_raw);
// struct sockaddr_in fill_ipv4(struct addrinfo *rp, struct hostent *peer, uint_16 port);
// struct sockaddr_in6 fill_ipv6(struct addrinfo *rp, struct hostent *peer, uint_16 port);
struct sockaddr *create_ip_connection(const char *node, const char *port, struct addrinfo hints, bool is_ipv6, int *socket);

/*
 * Used when singal handler is reached. Slowly end infinite while loop
 * before detaching all of resources in concurent thread.
 */
bool is_exiting();

/* Runs thread on separate thread_id. Last thread_id is number of global variable 'thread_id_counter' */
void *runner(bool is_concurrent, struct thread_args arguments, void *(*run)(void *));
/* 
 * Return null, when could not establish any addres or when could not send any message.
 * the run method gives in void * -> struct thread_args. There is need to retype to int socket and
 * struct sockaddr *peer.
 */
void *udp_client(bool is_ipv6, bool is_concurrent, const char*ip_address, uint16_t port, void *(*run)(void *));
/* Return -1, when could not establish any addres, -2 when could not connect client to server. */
void *tcp_client(bool is_ipv6, bool is_concurrent, const char*ip_address, uint16_t port, void *(*run)(void *));
void *udp_server(bool is_ipv6, bool is_concurrent, const char*ip_address, uint16_t port);
void *tcp_server(bool is_ipv6, bool is_concurrent, const char*ip_address, uint16_t port);
/* TODO: ICMP server and client */

/* This function should be overriden when c++ */
void *run_udp_server(void *thread_args);
void *run_udp_client(void *thread_args);

#endif
