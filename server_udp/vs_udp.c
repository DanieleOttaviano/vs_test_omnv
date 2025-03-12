#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

static struct timespec r;
static int period;


#define SEND_IP "192.168.1.82"
#define SEND_PORT 44488
#define RECV_IP "192.168.1.1"
#define RECV_PORT 44489
#define MAX_ELEMENTS 20000
#define NSEC_PER_SEC 1000000000ULL
#define REPS 1

struct data {
    double ivs3;
    double zcdot;
};

static inline void timespec_add_us(struct timespec *t, uint64_t d)
{
    d *= 1000;
    t->tv_nsec += d;
    t->tv_sec += t->tv_nsec / NSEC_PER_SEC;
    t->tv_nsec %= NSEC_PER_SEC;
}

void wait_next_activation(void)
{
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &r, NULL);
    timespec_add_us(&r, period);
}

void start_periodic_timer(uint64_t offs, int t)
{
    clock_gettime(CLOCK_REALTIME, &r);
    timespec_add_us(&r, offs);
    period = t;
}


void udp_send_data(int sockfd, struct sockaddr_in *servaddr, struct data *data) {
    ssize_t n = sendto(sockfd, data, sizeof(*data), 0, (const struct sockaddr *) servaddr, sizeof(*servaddr));
    if (n < 0) {
        perror("sendto failed");
    } 
    // else {
    //     printf("Sent values %lf - %lf to %s:%d\n", data->ivs3, data->zcdot, SEND_IP, SEND_PORT);
    // }
}

void udp_recv_data(int sockfd, struct sockaddr_in *servaddr, double *recv_data) {
    socklen_t len = sizeof(*servaddr);
    ssize_t n = recvfrom(sockfd, recv_data, sizeof(double), 0, (struct sockaddr *) servaddr, &len);
    if (n < 0) {
        perror("recvfrom failed");
    }
    // else {
    //     printf("Received values %lf from %s:%d\n", *recv_data, RECV_IP, RECV_PORT);
    // }
}

int load_data_from_file(const char *filename, double *data, size_t max_elements) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    size_t count = 0;
    while (count < max_elements && fscanf(file, "%lf", &data[count]) == 1) {
        count++;
    }

    fclose(file);
    return count;
}

void save_data_to_file(const char *filename, double *data, size_t count) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    for (size_t i = 0; i < count; i++) {
        fprintf(file, "%lf\n", data[i]);
    }

    fclose(file);
}

int main() {
    int send_sockfd, recv_sockfd;
    struct sockaddr_in send_servaddr, recv_servaddr;
    struct data data;
    struct timespec start, end;
    double ivs3_data[MAX_ELEMENTS];
    double zcdot_data[MAX_ELEMENTS];
    double gt_data[MAX_ELEMENTS];
    double recv_data[MAX_ELEMENTS];
    double cycle_times[REPS * MAX_ELEMENTS];
    size_t ivs3_count, zcdot_count, gt_count, recv_count = 0;

    // Load data from files
    ivs3_count = load_data_from_file("VS_Classic_Track/IVS3.txt", ivs3_data, MAX_ELEMENTS);
    zcdot_count = load_data_from_file("VS_Classic_Track/ZcDot.txt", zcdot_data, MAX_ELEMENTS);
    gt_count = load_data_from_file("VS_Classic_Track/VS3_ref.txt", gt_data, MAX_ELEMENTS);

    if (ivs3_count != zcdot_count || ivs3_count != gt_count) {
        fprintf(stderr, "Data files have different number of elements\n");
        exit(EXIT_FAILURE);
    }

    // Creating socket file descriptor for sending
    if ((send_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("send socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&send_servaddr, 0, sizeof(send_servaddr));
    send_servaddr.sin_family = AF_INET;
    send_servaddr.sin_port = htons(SEND_PORT);
    send_servaddr.sin_addr.s_addr = inet_addr(SEND_IP);

    // Creating socket file descriptor for receiving
    if ((recv_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("recv socket creation failed");
        close(send_sockfd);
        exit(EXIT_FAILURE);
    }

    memset(&recv_servaddr, 0, sizeof(recv_servaddr));
    recv_servaddr.sin_family = AF_INET;
    recv_servaddr.sin_port = htons(RECV_PORT);
    recv_servaddr.sin_addr.s_addr = inet_addr(RECV_IP);

    // Bind the receiving socket
    if (bind(recv_sockfd, (const struct sockaddr *)&recv_servaddr, sizeof(recv_servaddr)) < 0) {
        perror("bind failed");
        close(send_sockfd);
        close(recv_sockfd);
        exit(EXIT_FAILURE);
    }

    start_periodic_timer(0, 1000);
    
    for (int rep = 0; rep < REPS; rep++) {
        for (size_t i = 0; i < ivs3_count; i++) {
            data.ivs3 = ivs3_data[i];
            data.zcdot = zcdot_data[i];

            udp_send_data(send_sockfd, &send_servaddr, &data);
            clock_gettime(CLOCK_REALTIME, &start);
            udp_recv_data(recv_sockfd, &recv_servaddr, &recv_data[recv_count]);
            clock_gettime(CLOCK_REALTIME, &end);
            recv_count++;

            cycle_times[i + (rep * MAX_ELEMENTS)] = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec); // time in nanoseconds
            wait_next_activation();
        }
        recv_count = 0;
    }

    for(int rep = 0; rep < REPS; rep++) {
        for(size_t i = 0; i < ivs3_count; i++) {
            printf("Received values %lf (%lf), Cycle Time %zu: %lf\n", recv_data[i], gt_data[i], i, cycle_times[i + (rep * MAX_ELEMENTS)]);
        }
    } 

    // Save received data to file
    save_data_to_file("VS_Classic_Track/recv_VS3_ref.txt", recv_data, MAX_ELEMENTS);
    save_data_to_file("VS_Classic_Track/cycle_times.txt", cycle_times, MAX_ELEMENTS * REPS);

    close(send_sockfd);
    close(recv_sockfd);
    return 0;
}
