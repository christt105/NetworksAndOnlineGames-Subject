#include "client.h"

#define UDP 0
#define TCP 1

int main(int argc, char** argv) {
#if UDP
	return UDPClient(argc, argv);
#elif TCP
	return TCPClient(argc, argv);
#else
	return 0;
#endif
}