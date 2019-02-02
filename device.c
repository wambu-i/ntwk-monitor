#include <linux/init.h>
#include <linux/ip.h> 
#include <linux/inet.h>            
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/netdevice.h>      
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h> 
#include <linux/skbuff.h>         
#include "jasusi.h"

static struct nf_hook_ops jasusi_ops;
/* Interface to listen to */
static const char *wifi = "wlan0";
struct sk_buff *socket_buffer;                              
struct udphdr *udp_header;
struct iphdr *ip_header;                                            

static u32 return_source_ip(struct iphdr *pkt) {
    return ntohl(pkt->saddr);
}
static u32 return_destination_ip(struct iphdr *pkt) {
    return ntohl(pkt->daddr);
}
static int return_protocol(struct iphdr *pkt) {
    int proto = pkt->protocol;
    if (!proto)
        return -1;
    return pkt->protocol;
}

static char *return_interface(const struct sk_buff *buffer) {
    return buffer->dev->name;
}
static unsigned char *return_device_mac(const struct sk_buff *buffer) {
    struct ethhdr *header;
    header = eth_hdr(buffer);
    return header->h_source;
}

static u_int32_t check_local_ip(void) {
    static char *local_network = "192.168.180.1";
    struct sockaddr_in *address;
    inet_pton(AF_INET, local_network, address->sin_addr);
    return address->sin_addr;
}

static unsigned int jasusi_all_incoming_hook(unsigned int hooknum, struct sk_buff *skb,
                            const struct net_device *in, const struct net_device *out,
                               int (*okfn)(struct sk_buff *)) {
    struct device_packets *packet;
    struct iphdr *ip_header;
    u32 destaddr;

    socket_buffer = skb;
    if (!socket_buffer)
        return NF_ACCEPT;
    packet = kmalloc(sizeof(*packet), GFP_KERNEL);
    packet->interface = return_interface(socket_buffer);
    /* Packets are not coming through the Wi-Fi chip
    *  so we don't care about them.
    */
    if (strcmp(packet->interface, wifi) != 0)
        return NF_ACCEPT;
    
    ip_header = (struct iphdr *) skb_network_header(socket_buffer);
    destaddr = check_local_ip();
    pr_debug("The local network address is %pI4", &destaddr);
    packet->protocol = return_protocol(ip_header);

    if (packet->protocol < 0)
        return NF_ACCEPT;

   /*  printk(KERN_INFO "Received packet with protocol %d on interface %s", packet->protocol, packet->interface); */

    if (packet->protocol != IPPROTO_TCP) {
        return NF_ACCEPT;
    }
    packet->src_addr = return_source_ip(ip_header);
    packet->dest_addr = return_destination_ip(ip_header);
    packet->mac_addr = return_device_mac(socket_buffer);
    /* trace_printk("Received packet from device %pM using protocol %d\n", packet->mac_addr, packet->protocol); */
    printk(KERN_INFO "Received packet from device %pM using protocol %d from source %pI4 to destination %pI4\n", packet->mac_addr, packet->protocol, &packet->src_addr, &packet->dest_addr);
    /* trace_printk("Received packet from device %pM using protocol %d\n to destination %pI4", packet->mac_addr, packet->protocol, packet->dest_addr); */
    
    return NF_ACCEPT;
}

static struct nf_hook_ops jasusi_hooks[] __read_mostly = {
    {
        .hook       = jasusi_all_incoming_hook,
        .pf         = NFPROTO_IPV4,
        .hooknum    = NF_INET_PRE_ROUTING,
        .priority   = NF_IP_PRI_FIRST,
    },

};

static int jasusi_init(void) {
    int result;
    result = nf_register_hooks(jasusi_hooks, ARRAY_SIZE(jasusi_hooks));
    if (result > 0) {
        ;
    }
    pr_debug("Loading jasusi into kernel!\n");
    return 0;
}

static void jasusi_cleanup(void){
    printk(KERN_INFO "Unloading jasusi!\n");
    nf_unregister_hooks(jasusi_hooks, ARRAY_SIZE(jasusi_hooks));
    return;
}

module_init(jasusi_init);
module_exit(jasusi_cleanup);

MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_LICENSE(LICENSE);