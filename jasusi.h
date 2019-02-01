#ifndef JASUSI_H
#define JASUSI_H
#include <linux/udp.h>
#include <linux/tcp.h>

#define AUTHOR "Wambui Karuga <wambui@brck.com"
#define DESCRIPTION "Network Traffic Logger"
#define LICENSE "GPL v2"

#define TCP_PROTOCOL 6
#define UDP_PROTOCOL 17



struct device_packets {
    unsigned int src_addr;
    unsigned int dest_addr;
    unsigned char *mac_addr;
    char *interface;
    int protocol;
    union {
        struct tcphdr *tcp_header;
        struct udphdr *udp_header;
    } packet_protocol;
    unsigned int src_port;
    unsigned int dest_port;
};
#endif /* JASUSI_H */