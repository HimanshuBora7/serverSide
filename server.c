#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
struct sockaddr_in *createIPv4Address(char *ip, int port);
int createTCPIpv4Socket();

struct sockaddr_in *createIPv4Address(char *ip, int port)
{

    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);

    if (strlen(ip) == 0)
    {
        address->sin_addr.s_addr = INADDR_ANY;
    }
    else
    {

        inet_pton(AF_INET, ip, &address->sin_addr.s_addr);
    }
    return address;
}

int main()
{

    int serverSocketFD = createTCPIpv4Socket();

    struct sockaddr_in *serverAddress = createIPv4Address("", 2000);

    int result = bind(serverSocketFD, (struct sockaddr *)serverAddress, sizeof(*serverAddress));

    if (result == 0)
    {
        printf("server is bounded successfully ");
    }

    int listenerResult = listen(serverSocketFD, 10);
    if (listenerResult == 0)
    {
        printf("listening\n");
    }
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);
    printf("before accept\n");
    int clientFd = accept(serverSocketFD, (struct sockaddr *)&clientAddress, (socklen_t *)&clientAddressSize);
    printf("client connected\n");
    char buffer[1024];
    while (1)
    {
        int bytes = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

        if (bytes <= 0)
            break;

        buffer[bytes] = '\0';
        printf("client: %s", buffer);
    }
    close(clientFd);
    return 0;
}

int createTCPIpv4Socket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
};