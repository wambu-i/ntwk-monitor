#ifndef JASUSI_H
#define JASUSI_H
#include <linux/udp.h>
#include <linux/tcp.h>

struct device_packets {
    unsigned int src_addr;
    unsigned int dest_addr;
    unsigned int mac_addr;
    char *interface;
    int protocol;
    union packet_protocol {
        struct tcphdr *tcp_header;
        struct udphdr *udp_header;
    }
    unsigned int src_port;
    unsigned int dest_port;
};
#endif /* JASUSI_H */