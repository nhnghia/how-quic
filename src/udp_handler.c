#include "udp_handler.h"
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <time.h>
#include "log.h"
#include "quic.h"

void udp_handler(
    u_char *args,
    const struct pcap_pkthdr *header,
    const u_char *packet
)
{
    time_t local_tv_sec;
    struct tm ltime;
    char timestr[16];

    struct ether_header *eth_hdr;
    ip_header *ip_hdr;
    eth_hdr = (struct ether_header *)packet;
    if (ntohs(eth_hdr->ether_type) != ETHERTYPE_IP) {
        log_debug("not an ip packet, skipping");
        return;
    }

    // header lengths in bytes
    int ethernet_header_length = 14;
    int ip_header_length;

    // find the start of IP header
    ip_hdr = (ip_header *) (packet + ethernet_header_length);

    ip_header_length = (ip_hdr->ver_ihl & 0x0F) * 4;

    //u_char protocol = (ip_hdr + 9);
    if (ip_hdr->proto != IPPROTO_UDP) {
        log_debug("not a udp packet, return");
        return;
    }

    udp_header *udp_hdr = (udp_header *)
        ((u_char *)ip_hdr + ip_header_length);
    
    u_short src_port = ntohs(udp_hdr->src_port);
    u_short dst_port = ntohs(udp_hdr->dst_port);
    u_short datagram_length = ntohs(udp_hdr->len);

    log_debug("%d.%d.%d.%d:%d -> %d.%d.%d.%d:%d",
        ip_hdr->saddr.byte1,
        ip_hdr->saddr.byte2,
        ip_hdr->saddr.byte3,
        ip_hdr->saddr.byte4,
        src_port,
        ip_hdr->daddr.byte1,
        ip_hdr->daddr.byte2,
        ip_hdr->daddr.byte3,
        ip_hdr->daddr.byte4,
        dst_port);
    
    if (dst_port == 4433)
    {
        local_tv_sec = header->ts.tv_sec;
        log_debug("%lld.%.9ld", (long long)header->ts.tv_sec, 
            header->ts.tv_usec);

        /* print timestamp and length of the packet */
        log_debug("total packet available: %d bytes", header->caplen);
        log_debug("expected packet size: %d bytes", header->len);
        
        log_debug("real_length: %d bytes", datagram_length);
        log_debug("udp payload_length: %d bytes\n", datagram_length - 8);
        quic_parse_header(packet + ethernet_header_length 
            + ip_header_length + 8);
    }
}