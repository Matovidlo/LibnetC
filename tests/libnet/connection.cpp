/* This is test file for libnetc. 
 * Distribued with GNU public license.

 * It's used for lightweight usage of sockets
 * wihout any knowledge of network programming.
 * Creators: Martin Vasko and David Bolvansky 
 * Email: matovidlo2@gmail.com/xvasko12@stud.fit.vutbr.cz
 **/
extern "C" {
#include"../../src/libnetc.h"
}
#include"gtest/gtest.h"
/*#ifdef TEST
#define INIT test_initialize
#else
#define INIT initialize_addrinfo
#endif
*/
struct addrinfo (*test_initialize)(bool is_icmp, bool is_udp, bool is_raw) = initialize_addrinfo;

TEST (establish_connection, test_address_info) {
    struct addrinfo hints;
    hints = initialize_addrinfo(false, false, false);
    ASSERT_EQ(hints.ai_socktype, SOCK_STREAM);
    ASSERT_EQ(hints.ai_protocol, 0);
    hints = initialize_addrinfo(true, false, false);
    ASSERT_EQ(hints.ai_socktype, SOCK_STREAM);
    ASSERT_EQ(hints.ai_protocol, IPPROTO_ICMP);
    hints = initialize_addrinfo(false, true, false);
    ASSERT_EQ(hints.ai_socktype, SOCK_DGRAM);
    ASSERT_EQ(hints.ai_protocol, 0);
    hints = initialize_addrinfo(false, true, true);
    ASSERT_EQ(hints.ai_socktype, SOCK_RAW);
    ASSERT_EQ(hints.ai_protocol, 0);
    // The SOCK_DGRAM will be overwritten to SOCK_RAW
    hints = initialize_addrinfo(true, true, true);
    ASSERT_EQ(hints.ai_socktype, SOCK_RAW);
    ASSERT_EQ(hints.ai_protocol, IPPROTO_ICMP);
}

TEST(establish_connection, test_create_ip_connection) {
    struct sockaddr *peer;
    struct addrinfo hints;
    int socket = -1;
    socklen_t peer_addr_length;
    char address[1024];
    
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = 0;
    // Create TCP connection IPv4
    peer = create_ip_connection("127.0.0.1", "12345", hints, &socket,
                                &peer_addr_length, false);
    ASSERT_GE(socket, 0);
    inet_ntop(AF_INET, &((struct sockaddr_in *)peer)->sin_addr,
              address, peer_addr_length);
    ASSERT_TRUE(address != nullptr);

    // Create TCP connection IPv6 with wrong settings
    peer = create_ip_connection("127.0.0.1", NULL, hints, &socket,
                                &peer_addr_length, true);
    ASSERT_EQ(socket, -1);
    ASSERT_TRUE(peer == nullptr);

    // Create TCP connection IPv6
    peer = create_ip_connection("::1", NULL, hints, &socket,
                                &peer_addr_length, true);
    inet_ntop(AF_INET6, &((struct sockaddr_in6 *)peer)->sin6_addr,
              address, peer_addr_length);
    ASSERT_GE(socket, 0);
    ASSERT_TRUE(address != nullptr);
}

struct addrinfo tcp_init(bool is_icmp, bool is_udp, bool is_raw) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_protocol = 0;
    hints.ai_socktype = SOCK_STREAM;
    return hints;
}
//struct sockaddr *create_ip_connection

TEST(establish_connection, test_udp_client) {
    struct sockaddr *peer;
    struct addrinfo hints;

    // mock the initialize_addrinfo function
    // test_initialize = tcp_init;
    // peer = create_ip_connection("127.0.0.1", "12345", hints, &socket,
    //                             &peer_addr_length, false);
    
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}