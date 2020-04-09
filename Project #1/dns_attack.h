#include <netinet/ip.h>
#include <netinet/udp.h>
#include <unistd.h>

//Load IP and UDP Header From Libraries
typedef struct iphdr ip_header;
typedef struct udphdr udp_header;

//Define the Pseudoheader
typedef struct{
    u_int32_t saddr;
    u_int32_t daddr;
    u_int8_t filler;
    u_int8_t protocol;
    u_int16_t len;
}pseudo;

//Define the DNS Header
typedef struct{
    unsigned short q_ID;
    unsigned short flag;
    unsigned short qc;
    unsigned short ans_c;
    unsigned short ac;
    unsigned short add;

}dns_header;

//DNS Question Field
typedef struct{
    unsigned short qtype;
    unsigned short qclass;
}question;

//DNS OPT Field
typedef struct __attribute__((__packed__)){
    uint8_t name;
    uint16_t type;
    uint16_t class;
    uint8_t extended;
    uint8_t version;
    uint16_t do_and_z;
    uint16_t len;
}optrr;
