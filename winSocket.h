#include <winsock2.h>

SOCKET initServer(WSADATA wsa, SOCKET listen_sock, SOCKADDR_IN serveraddr, SOCKET client_sock, SOCKADDR_IN clientaddr, int addrlen);