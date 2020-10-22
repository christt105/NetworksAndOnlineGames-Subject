#include "server.h"

#define UDP 0
#define TCP 1

int main(int argc, char** argv) {
#if UDP
	return UDPServer(argc, argv);
#elif TCP
	return TCPServer(argc, argv);
#else
	return 0;
#endif
}