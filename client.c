#include "lib.h"

int sock_fd = 0;
client_t client_info;
char send_buffer[BUFFER_SIZE] = {0};
char receve_buffer[BUFFER_SIZE] = {0};

void * read_thread_fnc(void * arg) {
    while (1) {
        if (read(sock_fd, receve_buffer, BUFFER_SIZE) <= 0) {
            printf("Server terminated.\n");
            kill(0, SIGKILL);
        };
        
        printf("\r");
        printf("\033[32mServer:\033[0m %s\n", receve_buffer);
        printf(">>> ");
        fflush(stdout);
        memset(receve_buffer, 0, BUFFER_SIZE);
    }
    return NULL;
} 

void * write_thread_fnc(void * arg) {
    while (1) {
        printf(">>> ");
        fgets(send_buffer, BUFFER_SIZE - 2, stdin);
        send_buffer[strlen(send_buffer) - 1] = '\0';
        write(sock_fd, send_buffer, strlen(send_buffer));
        if (!strcmp(send_buffer, "exit")) {
            sleep(2);
            printf("Exiting...\n");
            exit(0);
        } 
        memset(send_buffer, 0, BUFFER_SIZE);
    }
    return NULL;
}

int main(int argc, char * argv[]) {
    struct sockaddr_in serv_addr;
    pthread_t write_thread;
    pthread_t read_thread; 
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock_fd >= 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    assert(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) > 0);
    assert(connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) >= 0);
    printf("Enter your name: ");
    scanf("%s", client_info.client_name);
    printf("Enter your last name: ");
    scanf("%s", client_info.client_last_name);
    printf("Enter your age: ");
    scanf("%d", &client_info.age);
    printf("Enter your passport number: ");
    scanf("%s", client_info.pasport_number);
    write(sock_fd, &client_info, sizeof(client_info));
    getchar();
    pthread_create(&write_thread, NULL, write_thread_fnc, NULL);
    pthread_create(&read_thread, NULL, read_thread_fnc, NULL);
    pthread_join(write_thread, NULL);
    pthread_join(read_thread, NULL);
    close(sock_fd);
    return 0;
}