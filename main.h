
#define BCAST_ADDR 100
#define PAYLOAD_MAX 100
#define STRING_MAX 100
#define NAME_LENGTH 100
#define PKT_FILE_DOWNLOAD_START   5
#define PKT_FILE_DOWNLOAD_MID     6
#define PKT_FILE_DOWNLOAD_END     7


enum NetNodeType { /* Types of network nodes */
	HOST,
	SWITCH
};

enum NetLinkType { /* Types of linkls */
	PIPE,
	SOCKET
};

struct net_node { /* Network node, e.g., host or switch */
	enum NetNodeType type;
	int id;
	struct net_node *next;
};

struct net_port { /* port to communicate with another node */
	enum NetLinkType type;
	int pipe_host_id;
	int pipe_send_fd;
	int pipe_recv_fd;
	struct net_port *next;
};

/* Packet sent between nodes  */

struct packet {
	
    int packetRootID;
    int packetRootDist;
    char packetSenderType;
    char packetSenderChild;
    char src;        // Source of the packet
    char dst;        // Destination of the packet
    char type;       // Type of the packet
    int length;      // Length of the packet's payload
    char payload[PAYLOAD_MAX];
};

/* Types of packets */

#define PKT_PING_REQ		0
#define PKT_PING_REPLY		1
#define PKT_FILE_UPLOAD_START	2
#define PKT_FILE_UPLOAD_END	3


/***** EDITED BY JUSTIN CAMPOS *****/
#define PKT_FILE_UPLOAD_MID 4

