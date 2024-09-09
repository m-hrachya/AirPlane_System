#include "lib.h"

int sock_fd;

int client_sock_fds[MAX_CLIENT_COUNT];
int client_count;

client_t client_infos[MAX_CLIENT_COUNT];
flights_t flight_infos;

int main(int argc, char * argv[]) {
    initializeClientSockFds();
    initializeFlightInfos();
    // Preparing To Socket Creation
    struct sockaddr_in address;
    socklen_t size = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));
    address.sin_addr.s_addr = INADDR_ANY;

    // Socket Creation
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock_fd >= 0);
    assert(bind(sock_fd, (struct sockaddr *) &address, size) >= 0);
    assert(listen(sock_fd, MAX_CLIENT_COUNT) == 0);

    // Starting Server
    while(1) {
        printf("\033[36mWaiting for acceptations...\033[0m\n");
        pthread_t thread;
        int tmp = accept(sock_fd, (struct sockaddr *)&address, &size);
        assert(tmp >= 0);
        int * _index = (int *) malloc(sizeof(int));
        assert(_index != NULL);
        *_index = getEmptyClientSockFd();
        setClientSockFd(*_index, tmp);
        recv(tmp, &client_infos[*_index], sizeof(client_infos), 0);
        printf("New user connected!\n");
        printf("\033[33mName: \033[0m%s\n", client_infos[*_index].client_name);
        printf("\033[33mLast Name: \033[0m%s\n", client_infos[*_index].client_last_name);
        printf("\033[33mAge:\033[0m %d\n", client_infos[*_index].age);
        printf("\033[33mPassport Number:\033[0m %s\n", client_infos[*_index].pasport_number);
        pthread_create(&thread, NULL, thread_fnc, (void *)_index);
        pthread_detach(thread);
    }
}