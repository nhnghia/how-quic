#include <stdio.h>
#include <pcap.h>
#include <unistd.h>

#include "udp_handler.h"
#include "log.h"

void help()
{
    printf("usage:\n");
    printf("./how-quic -i interface -s server_ip -p server_port\n\n");
    printf("example:\n");
    printf("./how-quic -i lo -s 192.168.1.101 -p 443\n");
}

int main(int argc, char *argv[])
{
    int c;
    char *device;
    char *server_ip;
    char *server_port;
    filter_server filter;
    
    opterr = 0;
    while ((c = getopt(argc, argv, "i:s:p:")) != -1)
        switch (c)
        {
        case 'i':
            device = optarg;
            break;
        case 's':
            server_ip = optarg;
            break;
        case 'p':
            server_port = optarg;
            break;
        case '?':
            if (optopt == 'c')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option '-%c'.\n", optopt);
            else
                fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            return 1;
        default:
            printf("hah\n");
            //return;
        }

    if (argc < 6) {
        help();
        return 1;
    }

    log_info("interface = %s, server ip = %s, server port = %s\n",
           device, server_ip, server_port);
    
    filter.server_ip = server_ip;
    filter.server_port = server_port;

    // log_set_level(4);
    // char *device = argc > 1 ? argv[1] : "lo";
    char error_buffer[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    int snapshot_length = 1024;

    // end the loop after this many packets are captured
    int total_packet_count = 13;

    handle = pcap_open_live(device, snapshot_length, 0, 10000, error_buffer);
    if (handle == NULL)
    {
        log_fatal("could not open device %s: %s", device, error_buffer);
        return 2;
    }

    // pcap_loop(handle, total_packet_count, udp_handler, NULL);
    pcap_loop(handle, total_packet_count, udp_handler, (u_char*)&filter);
    return 0;
}
