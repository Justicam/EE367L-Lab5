// switch.h - Header file for the switch module

// Structure to represent the forwarding table entries
struct switch_fwd {
    int valid; // Valid bit to check if the entry is active
    char dst;   // Destination host ID
    int port;  // Port number to forward packets to for the given destination
};

// Function prototypes
void switch_main(int switch_id);

