#include <linux/init.h>
#include <linux/ip.h>             
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/netdevice.h>      
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h> 
#include <linux/skbuff.h>         
#include <linux/udp.h>
#include "jasusi.h"

#define AUTHOR "Wambui Karuga <wambui@brck.com"
#define DESCRIPTION "Network Traffic Logger"
#define LICENSE "GPL v2"

static struct nf_hook_ops jasusi_ops;
/* Interfaces */
static const unsigned char *localhost = "lo";
static const char *ethernet = "eth0";
struct sk_buff *socket_buffer;                              
struct udphdr *udp_header;
struct iphdr *header;                                               

static unsigned int return_source_ip(struct iphdr *pkt) {
    return (unsigned int) pkt->saddr;
}
static unsigned int return_destination_ip(struct iphdr *pkt) {
    return (unsigned int) pkt->daddr;
}
static unsigned int return_protocol(struct iphdr *pkt) {
    return (unsigned int ) pkt->protocol;
}
unsigned int jasusi_main_hook(unsigned int hooknum, struct sk_buff *skb,
                            const struct net_device *in, const struct net_device *out,
                               int (*okfn)(struct sk_buff *)) {
    socket_buffer = skb;
    header = (struct iphdr *) skb_network_header(socket_buffer);
    struct device_packets *test;
    test = kmalloc(sizeof(*test), GFP_KERNEL);
    test->src_addr = return_source_ip(header);
    test->dest_addr = return_destination_ip(header);
    test->protocol = return_protocol(header);
    if (test->protocol == 17) {
        test->udp_header = (struct udphdr *) skb_transport_header(socket_buffer);
        
    }
    /* if (strcmp(in->name, localhost) == 0 || strcmp(in->name, ethernet) == 0){
        printk(KERN_INFO "Packets received through interface %s. Dropping...", in->name);
        return NF_DROP;
    } */
    printk(KERN_INFO "Received packet from %d using protocol %d", test->src_addr, header->protocol);
    return NF_ACCEPT;
}

static int jasusi_init(void) {
    jasusi_ops.hook = jasusi_main_hook;
    jasusi_ops.pf = PF_INET;
    jasusi_ops.hooknum = NF_INET_PRE_ROUTING;
    jasusi_ops.priority = NF_IP_PRI_FIRST;

    nf_register_hook(&jasusi_ops);
    return 0;
}

static void jasusi_cleanup(void){
    printk(KERN_INFO "Unloading jasusi!");
    nf_unregister_hook(&jasusi_ops);
    return;
}

module_init(jasusi_init);
module_exit(jasusi_cleanup);

MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_LICENSE(LICENSE);