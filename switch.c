 /*
  * switch.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>

#include "main.h" //Main simulation
#include "net.h" //Network Function
#include "man.h" // Manager for Network
#include "host.h" //Host Definitions
#include "switch.h" // Switch Definitions
#include "packet.h" // Packet Structure

//Constants for Buffers and Size
#define MAX_FILE_BUFFER 1000
#define MAX_MSG_LENGTH 100
#define MAX_DIR_NAME 100
#define MAX_FILE_NAME 100
#define PKT_PAYLOAD_MAX 100
#define TENMILLISEC 10000   /* 10 millisecond sleep */
#define MAX_FWD_LENGTH 100 //Max length of FWD TBL

/*
 *  Main
 */

void switch_main(int switch_id)
{

/* State */
char dir[MAX_DIR_NAME]; //Directory for Storing Files
int dir_valid = 0; //Flag to Check Directory Valid

char man_msg[MAN_MSG_LENGTH]; //Buffer for Manager Messages
char man_reply_msg[MAN_MSG_LENGTH]; // Buffer for Replies
char man_cmd; // Command Recieved from manager

struct net_port *node_port_list; // List of Network Ports
struct net_port **node_port;  // Array of pointers to node ports
int node_port_num;            // Number of node ports

int ping_reply_received;

int i, k, n; //Loop Counters
int dst;  //Destination ID
char name[MAX_FILE_NAME]; // Name Buffer
char string[PKT_PAYLOAD_MAX+1]; //String Buffer

FILE *fp;  //File Pointer

struct packet *in_packet; /* Incoming packet */
struct packet *new_packet; //New Packet for forwarding

struct net_port *p; //Pointer for iterating Through Ports
struct host_job *new_job; //Job for Incoming Packets
struct host_job *new_job2; //Additional Job Variable

struct job_queue job_q; //Queue of Jobs

struct switch_fwd fwd_table[MAX_FWD_LENGTH]; //Forwarding Table

for (int i=0; i<MAX_FWD_LENGTH; i++)
	fwd_table[i].valid = 0;




/*
 * Create an array node_port[ ] to store the network link ports
 * at the switch.  The number of ports is node_port_num
 */
node_port_list = net_get_port_list(switch_id);

	/*  Count the number of network link ports */
node_port_num = 0;
for (p=node_port_list; p!=NULL; p=p->next) {
	node_port_num++;
}
	/* Allocate memory space for the array */
node_port = (struct net_port **)
	malloc(node_port_num*sizeof(struct net_port *));

	/* Load ports into the array */
p = node_port_list;
for (k = 0; k < node_port_num; k++) {
	node_port[k] = p;
	p = p->next;
}

/* Initialize the job queue */
job_q_init(&job_q);

int flag;

//Main Loop for switch op
while(1) {

	/*
	 * Get packets from incoming links and send them back out
  	 * Put jobs in job queue
 	 */

	flag = 0;

	//process incoming packets
	for (k = 0; k < node_port_num; k++) { /* Scan all ports */

		in_packet = (struct packet *) malloc(sizeof(struct packet));
		n = packet_recv(node_port[k], in_packet);

		if (n > 0) {
			//packets received, add to job queue
			new_job = (struct host_job *)
				malloc(sizeof(struct host_job));
			new_job->in_port_index = k;
			new_job->packet = in_packet;

			/*printf("\ts%d: get on p%d: h%d ~ h%d\n",
				switch_id, k, new_job->packet->src, new_job->packet->dst);*/

			job_q_add(&job_q, new_job);

			flag = 1;
		}
		else {
			//no packets recieved
			free(in_packet);
		}
	}

	
	/*
 	 * Execute one job in the job queue
 	 */

	flag = 0;

	if (job_q_num(&job_q) > 0) {

		
		/* Get a new job from the job queue */
		new_job = job_q_remove(&job_q);

		

		int vport = -1;
		for (i=0; i<MAX_FWD_LENGTH; i++)
		{
			//scan for valid match
			if (fwd_table[i].valid && fwd_table[i].dst == new_job->packet->src)
			{
				vport = fwd_table[i].port;
				
				break;
			}
		}
		if (vport == -1) //need to add an entry
		{
			for (i=0; i<MAX_FWD_LENGTH; i++)
			{
				if (!fwd_table[i].valid)
				{
					fwd_table[i].valid = 1;
					fwd_table[i].dst = new_job->packet->src;
					fwd_table[i].port = new_job->in_port_index;
					vport = fwd_table[i].port;
					
					break;
				}
			}
		}

		vport = -1;
		for (i=0; i<MAX_FWD_LENGTH; i++)
		{
			//scan for valid match
			if (fwd_table[i].valid && fwd_table[i].dst == new_job->packet->dst)
			{
				vport = fwd_table[i].port;
				
				break;
			}
		}

		if (vport > -1)
		{
			packet_send(node_port[vport], new_job->packet);
			
		}
		else
		{
			for (k=0; k<node_port_num; k++)
			{
				if (k != new_job->in_port_index) {
					packet_send(node_port[k], new_job->packet);
					
				}
			}
		}

		free(new_job->packet);
		free(new_job);
	}

	/* The switch goes to sleep for 10 ms */
	usleep(TENMILLISEC);

} /* End of while loop */

}