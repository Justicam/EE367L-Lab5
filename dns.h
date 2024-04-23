/*
    dns.h
*/

//Inintialize the naming table
void dns_init(struct naming_table table);

//Function to register hosts to naming table
int dns_add_entry(char* domain, int ip);