#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>

#define PORT 44488
#define DEST_IP "127.0.0.1"

struct data{
    double value1;
    double value2;
};

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    struct data data;
    
    data.value1 = -245.575131325451; // 100; // The value to be sent
    data.value2 = 1.41843971631206; // 200; 

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(DEST_IP);
    

    ssize_t n = sendto(sockfd, &data, sizeof(data), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    if (n < 0) {
        perror("sendto failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    } else {
        printf("Sent uint64_t values %f - %f  to %s:%d\n", data.value1, data.value2, DEST_IP, PORT);
    }

    close(sockfd);
    return 0;
}

