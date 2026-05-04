#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/select.h>
struct sockaddr_in *createIPv4Address(char *ip, int port);
int createTCPIpv4Socket();

#define MAX_CLIENTS 10
int clientSockets[MAX_CLIENTS] = {0};

struct sockaddr_in *createIPv4Address(char *ip, int port)
{

    struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
    memset(address, 0, sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);

    if (strlen(ip) == 0)
    {
        address->sin_addr.s_addr = INADDR_ANY;
    }
    else
    {

        inet_pton(AF_INET, ip, &address->sin_addr);
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
    fd_set readfds;
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);

    while (1)
    {
        FD_ZERO(&readfds);

        FD_SET(serverSocketFD, &readfds);
        int max_fd = serverSocketFD;

        // add all clients
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = clientSockets[i];

            if (sd > 0)
                FD_SET(sd, &readfds);

            if (sd > max_fd)
                max_fd = sd;
        }

        select(max_fd + 1, &readfds, NULL, NULL, NULL);

        // new connection
        if (FD_ISSET(serverSocketFD, &readfds))
        {
            int new_socket = accept(serverSocketFD,
                                    (struct sockaddr *)&clientAddress,
                                    (socklen_t *)&clientAddressSize);

            printf("New client connected: %d\n", new_socket);

            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (clientSockets[i] == 0)
                {
                    clientSockets[i] = new_socket;
                    break;
                }
            }
        }

        // handle messages
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = clientSockets[i];

            if (FD_ISSET(sd, &readfds))
            {
                char buffer[1024];
                int bytes = recv(sd, buffer, sizeof(buffer) - 1, 0);

                if (bytes <= 0)
                {
                    printf("Client disconnected: %d\n", sd);
                    close(sd);
                    clientSockets[i] = 0;
                }
                else
                {
                    buffer[bytes] = '\0';
                    printf("Client %d: %s", sd, buffer);

                    for (int j = 0; j < MAX_CLIENTS; j++)
                    {
                        if (clientSockets[j] != 0 && clientSockets[j] != sd)
                        {
                            printf("Broadcasting to %d\n", clientSockets[j]);
                            send(clientSockets[j], buffer, bytes, 0);
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int createTCPIpv4Socket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
};