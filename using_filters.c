#include <stdio.h>
#include <pcap.h>
#include <time.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

void print_packet_info(const u_char *packet, struct pcap_pkthdr packet_header);
void my_packet_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

int nDelay = 100000;
char command1[50];
char command2[50];
char command3[50];
char command4[50];
char command5[50];

int main(int argc, char **argv) {
strcpy(command1, "echo -n \"40\" > /sys/class/gpio/unexport");
strcpy(command2, "echo -n \"40\" > /sys/class/gpio/export");
strcpy(command3, "echo -n \"out\" > /sys/class/gpio/gpio40/direction");
strcpy(command4, "echo -n \"1\" > /sys/class/gpio/gpio40/value");
strcpy(command5, "echo -n \"0\" > /sys/class/gpio/gpio40/value");
system(command1);
system(command2);
system(command3);

system(command4);
usleep(nDelay);
usleep(nDelay);
usleep(nDelay);
usleep(nDelay);
usleep(nDelay);
usleep(nDelay);
system(command5);

	char dev[] = "mon0";
	pcap_t *handle;
	 const u_char *packet;
 	struct pcap_pkthdr packet_header;
	char error_buffer[PCAP_ERRBUF_SIZE];
	struct bpf_program filter;
	char filter_exp[] = "udp && src 169.254.1.1 && dst 255.255.255.255 && src port 50505 && dst port 50505";
	bpf_u_int32 subnet_mask, ip;

	if (pcap_lookupnet(dev, &ip, &subnet_mask, error_buffer) == -1) {
		printf("Could not get information for device: %s\n", dev);
		ip = 0;
		subnet_mask = 0;
	}
	handle = pcap_open_live(dev, BUFSIZ, 1, 0, error_buffer);
	if (handle == NULL) {
		printf("Could not open %s - %s\n", dev, error_buffer);
		return 2;
	}
	if (pcap_compile(handle, &filter, filter_exp, 0, ip) == -1) {
		printf("Bad filter - %s\n", pcap_geterr(handle));
		return 2;
	}
	if (pcap_setfilter(handle, &filter) == -1) {
		printf("Error setting filter - %s\n", pcap_geterr(handle));
		return 2;
	}


while (1)
{

	// pcap_next() or pcap_loop() to get packets from device now 
	// Only packets over port 80 will be returned.
	packet = pcap_next(handle, &packet_header);
 	if (packet == NULL) {
        	printf("No packet found.\n");
	}
	else {
		system(command4);
		system(command5);
		//print_packet_info(packet, packet_header);
	}
}



/*
        int snapshot_length = 1024;
        int total_packet_count = 200;
        u_char *my_arguments = NULL;
	pcap_loop(handle, total_packet_count, my_packet_handler, my_arguments);
*/
	
	return 0;
}


void print_packet_info(const u_char *packet, struct pcap_pkthdr packet_header) {
    u_char *pointer;
    int address_length;

    printf(
        "Time recieved: %s\n",
        ctime((const time_t*)&packet_header.ts.tv_sec)
    );
    printf("Packet length %d\n", packet_header.len);
    printf("Packet capture length: %d\n", packet_header.caplen);

    /* What type of ethernet packet */
    struct ether_header *ethernet_header;
    ethernet_header = (struct ether_header *) packet;
    if (ntohs(ethernet_header->ether_type) == ETHERTYPE_IP) {
        printf(
            "Ethernet type hex:%x dec:%d is an IP packet.\n",
            ntohs(ethernet_header->ether_type),
            ntohs(ethernet_header->ether_type)
        );
    } else  if (ntohs (ethernet_header->ether_type) == ETHERTYPE_ARP) {
        printf(
            "Ethernet type hex:%x dec:%d is an ARP packet.\n",
            ntohs(ethernet_header->ether_type),
            ntohs(ethernet_header->ether_type)
        );
    } else {
        printf("Other ethernet type: %x", ntohs(ethernet_header->ether_type));
        return;
    }

    /* Source address */
    address_length = ETHER_ADDR_LEN;
    pointer = ethernet_header->ether_shost;
    printf("Source Address:  ");
    do {
        printf(
            "%s%x",
            (address_length == ETHER_ADDR_LEN) ? " " : ":", *pointer++
        );
    } while(--address_length > 0);
    printf("\n");

    /* Destination address */
    pointer = ethernet_header->ether_dhost;
    address_length = ETHER_ADDR_LEN;
    printf("Destination Address:  ");
    do {
        printf(
            "%s%x",
            (address_length == ETHER_ADDR_LEN) ? " " : ":", *pointer++
        );
    } while(--address_length > 0);
    printf("\n");
}

void my_packet_handler(
    u_char *args,
    const struct pcap_pkthdr *header,
    const u_char *packet
)
{

int nDelay = 10;
char command4[50];
char command5[50];

strcpy(command4, "echo -n \"1\" > /sys/class/gpio/gpio40/value");
strcpy(command5, "echo -n \"0\" > /sys/class/gpio/gpio40/value");

system(command4);
//usleep(nDelay);
system(command5);



}


/* Finds the payload of a TCP/IP packet */
/*
void my_packet_handler(
    u_char *args,
    const struct pcap_pkthdr *header,
    const u_char *packet
)
{
    // First, lets make sure we have an IP packet 
    struct ether_header *eth_header;
    eth_header = (struct ether_header *) packet;
    if (ntohs(eth_header->ether_type) != ETHERTYPE_IP) {
        printf("Not an IP packet. Skipping...\n\n");
        return;
    }

    // The total packet length, including all headers
    //   and the data payload is stored in
    //   header->len and header->caplen. Caplen is
    //   the amount actually available, and len is the
    //   total packet length even if it is larger
    //   than what we currently have captured. If the snapshot
     //  length set with pcap_open_live() is too small, you may
     //  not have the whole packet. 
    printf("Total packet available: %d bytes\n", header->caplen);
    printf("Expected packet size: %d bytes\n", header->len);

    // Pointers to start point of various headers 
    const u_char *ip_header;
    const u_char *tcp_header;
    const u_char *payload;

    // Header lengths in bytes
    int ethernet_header_length = 14; // Doesn't change
    int ip_header_length;
    int tcp_header_length;
    int payload_length;

    // Find start of IP header 
    ip_header = packet + ethernet_header_length;
    // The second-half of the first byte in ip_header
    //   contains the IP header length (IHL). 
    ip_header_length = ((*ip_header) & 0x0F);
    // The IHL is number of 32-bit segments. Multiply
    //   by four to get a byte count for pointer arithmetic 
    ip_header_length = ip_header_length * 4;
    printf("IP header length (IHL) in bytes: %d\n", ip_header_length);

    // Now that we know where the IP header is, we can 
    //   inspect the IP header for a protocol number to 
     //  make sure it is TCP before going any further. 
    //   Protocol is always the 10th byte of the IP header 
    u_char protocol = *(ip_header + 9);
    if (protocol != IPPROTO_TCP) {
        printf("Not a TCP packet. Skipping...\n\n");
        return;
    }

    // Add the ethernet and ip header length to the start of the packet
    //   to find the beginning of the TCP header
    tcp_header = packet + ethernet_header_length + ip_header_length;
    // TCP header length is stored in the first half 
    //   of the 12th byte in the TCP header. Because we only want
    //   the value of the top half of the byte, we have to shift it
    //   down to the bottom half otherwise it is using the most 
    //   significant bits instead of the least significant bits 
    tcp_header_length = ((*(tcp_header + 12)) & 0xF0) >> 4;
    // The TCP header length stored in those 4 bits represents
    //   how many 32-bit words there are in the header, just like
    //   the IP header length. We multiply by four again to get a
    //   byte count. 
    tcp_header_length = tcp_header_length * 4;
    printf("TCP header length in bytes: %d\n", tcp_header_length);

    // Add up all the header sizes to find the payload offset
    int total_headers_size = ethernet_header_length+ip_header_length+tcp_header_length;
    printf("Size of all headers combined: %d bytes\n", total_headers_size);
    payload_length = header->caplen -
        (ethernet_header_length + ip_header_length + tcp_header_length);
    printf("Payload size: %d bytes\n", payload_length);
    payload = packet + total_headers_size;
    printf("Memory address where payload begins: %p\n\n", payload);

    // Print payload in ASCII 
    if (payload_length > 0) {
        const u_char *temp_pointer = payload;
        int byte_count = 0;
        while (byte_count++ < payload_length) {
            printf("%c", *temp_pointer);
            temp_pointer++;
        }
        printf("\n");
    }

    return;
}
*/