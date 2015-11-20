#define MAX_PENDING_CONNECTIONS 10
#define MAX_BUFF_LEN 1024

int   sendMessage(char *buff, int descriptor);
int   receiveMessage(char *buff, int descriptor);
void  setupListenSocket(int port, int *listen_socket);
void  acceptIncomingConnection(int *listen_socket, int *accept_socket);
void  connectToServer(char *ip_address, int port, int *descriptor);
void  receiveFile(char *buff, int descriptor, char *filename);
void  sendFile(int descriptor, char *filename);
