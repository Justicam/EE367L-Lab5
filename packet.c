
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#include "main.h"
#include "packet.h"
#include "net.h"
#include "host.h"
#include "switch.h"

#define TREE_PACKET_TYPE 0x01

void packet_send(struct net_port *port, struct packet *p)
{
    char msg[PAYLOAD_MAX + 12];
    int i, index;

    
        msg[0] = (char)p->src;
        msg[1] = (char)p->dst;
        msg[2] = (char)p->type;
        msg[3] = (char)p->length;
        for (i = 0; i < p->length; i++) {
            msg[i + 4] = p->payload[i];
        }
        index = p->length + 4;
        
	if (p->type == TREE_PACKET_TYPE) {
        msg[p->length + 4] = (char)(p->packetRootID >> 24);
        msg[p->length + 5] = (char)(p->packetRootID >> 16);
        msg[p->length + 6] = (char)(p->packetRootID >> 8);
        msg[p->length + 7] = (char)p->packetRootID;
        msg[p->length + 8] = (char)p->packetRootDist;
        msg[p->length + 9] = p->packetSenderType;
        msg[p->length + 10] = p->packetSenderChild;
    }

     write(port->pipe_send_fd, msg, p->length + 11);
}

int packet_recv(struct net_port *port, struct packet *p)
{
    char msg[PAYLOAD_MAX + 12];
    int n;
    int i;
    int index;

    if (port->type == PIPE) {
        n = read(port->pipe_recv_fd, msg, PAYLOAD_MAX + 12);
        if (n > 0) {
            p->src = (char)msg[0];
            p->dst = (char)msg[1];
            p->type = (char)msg[2];
            p->length = (int)msg[3];
            for (i = 0; i < p->length; i++) {
                p->payload[i] = msg[i + 4];
            }
            index = p->length + 4;
            
            if (p->type == TREE_PACKET_TYPE && n > index + 6) {
            p->packetRootID = (msg[p->length + 4] << 24) |
                              (msg[p->length + 5] << 16) |
                              (msg[p->length + 6] << 8) |
                              msg[p->length + 7];
            p->packetRootDist = (char)msg[p->length + 8];
            p->packetSenderType = msg[p->length + 9];
            p->packetSenderChild = msg[p->length + 10];
        }
    }
  }

    return (n);
} 


