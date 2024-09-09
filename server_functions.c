#include "lib.h"

extern int sock_fd;

extern int client_sock_fds[MAX_CLIENT_COUNT];
extern int client_count;

extern client_t client_infos[MAX_CLIENT_COUNT];
extern flights_t flight_infos;

void initializeClientSockFds() {
    for (int i = 0; i < MAX_CLIENT_COUNT; ++i) {
        client_sock_fds[i] = -1;
    }
}

char* itoa(int value) {
    if (value == 0) {
        char* str = (char*)malloc(2 * sizeof(char)); 
        if (str == NULL) {
            return NULL;
        }
        str[0] = '0';
        str[1] = '\0';
        return str;
    }
    int temp = value;
    int length = 0;
    int is_negative = 0;
    
    if (value < 0) {
        is_negative = 1;
        temp = -temp;
    }

    do {
        length++;
        temp /= 10;
    } while (temp != 0);

    if (is_negative) {
        length++;
    }

    char* str = (char*)malloc((length + 1) * sizeof(char));
    if (str == NULL) {
        return NULL;
    }

    str[length] = '\0';

    temp = value;
    if (value < 0) {
        temp = -temp;
    }

    for (int i = length - 1; i >= 0; i--) {
        if (temp > 0) {
            str[i] = (temp % 10) + '0';
            temp /= 10;
        } else {
            if (is_negative) {
                str[i] = '-';
                is_negative = 0; 
            }
        }
    }
    return str;
}

void initializeFlightInfos() {
    for (int i = 0; i < FLIGHTS_COUNT; ++i) {
        printf("Flight N%d: ", i + 1);
        fgets(flight_infos.available_flights[i], NAME_SIZE - 2, stdin);
        flight_infos.available_flights[i][strlen(flight_infos.available_flights[i]) - 1] = '\0';
        printf("Premium places: ");
        scanf("%d", &flight_infos.premium_places[i]);
        printf("Middle places: ");
        scanf("%d", &flight_infos.middle_places[i]);
        printf("Cheap places: ");
        scanf("%d", &flight_infos.cheap_places[i]);
        printf("\n");
        fflush(stdin);
        fflush(stdout);
        getchar();
    }
}

int getEmptyClientSockFd() {
    for (int i = 0; i < MAX_CLIENT_COUNT; ++i) {
        if (client_sock_fds[i] == -1) {
            return i;
        }
    }
    return -1;
}

void setClientSockFd(int index, int value) {
    client_sock_fds[index] = value;
    ++client_count;
}

void * thread_fnc(void * arg) {
    int * _index = (int *) arg;
    char client_command[COMMAND_SIZE] = {0};
    int already_booked = 0;
    int booked_place = BOOK_DEFAULT;
    int selected_flight = -1;
    while (1) {
        memset(client_command, 0, COMMAND_SIZE);
        read(client_sock_fds[*_index], client_command, COMMAND_SIZE);
        assert(read>=0);
        char response[RESPONSE_SIZE] = {0};
        if (!strcmp(client_command, "exit")) {
            strcpy(response, "No changes will be saved");
            write(client_sock_fds[*_index], response, RESPONSE_SIZE);
            break;
        } else if (!strcmp(client_command, "available flights")) {
            for (int i = 0; i < FLIGHTS_COUNT; ++i) {
                memset(response, 0, RESPONSE_SIZE);
                strcpy(response, "Flight N");
                char * _ind = itoa(i + 1);
                strcat(response, _ind);
                free(_ind);
                strcat(response, ": ");
                strcat(response, flight_infos.available_flights[i]);
                write(client_sock_fds[*_index], response, RESPONSE_SIZE);
                memset(response, 0, RESPONSE_SIZE);
                char * cheaps = itoa(flight_infos.cheap_places[i]);
                char * middles = itoa(flight_infos.middle_places[i]);
                char * prems = itoa(flight_infos.premium_places[i]);
                strcpy(response, "Cheap places: ");
                strcat(response, cheaps);
                strcat(response, ", Middle places: ");
                strcat(response, middles);
                strcat(response, ", Premium places: ");
                strcat(response, prems);
                free(prems);
                free(cheaps);
                free(middles);
                write(client_sock_fds[*_index], response, RESPONSE_SIZE);
            }
        } else if (!strcmp(client_command, "select 1")) {
            selected_flight = 0;
            write(client_sock_fds[*_index], "Selected 1st flight", strlen("Selected 1st flight"));
        } else if (!strcmp(client_command, "select 2")) {
            selected_flight = 1;
            write(client_sock_fds[*_index], "Selected 2nd flight", strlen("Selected 2nd flight"));
        } else if (!strcmp(client_command, "select 3")) {
            selected_flight = 2;
            write(client_sock_fds[*_index], "Selected 3rd flight", strlen("Selected 3rd flight"));
        } else if (!strcmp(client_command, "book cheap")) {
            if (already_booked) {
                write(client_sock_fds[*_index], "You have already booked flight.\n Cancel to book anouther place.", strlen("You have already booked flight.\n Cancel to book anouther place."));
                continue;
            } else if (selected_flight == -1) {
                write(client_sock_fds[*_index], "Select flight first", strlen("Select flight first"));
                continue;
            }
            write(client_sock_fds[*_index], "Cheap place booked", strlen("Cheap place booked"));
            booked_place = BOOK_CHEAP;
            already_booked = 1;
        } else if (!strcmp(client_command, "book middle")) {
            if (already_booked) {
                write(client_sock_fds[*_index], "You have already booked flight.\n Cancel to book anouther place.", strlen("You have already booked flight.\n Cancel to book anouther place."));
                continue;
            } else if (selected_flight == -1) {
                write(client_sock_fds[*_index], "Select flight first", strlen("Select flight first"));
                continue;
            }
            write(client_sock_fds[*_index], "Middle place booked", strlen("Middle place booked"));
            booked_place = BOOK_MIDDLE;
            already_booked = 1;
        } else if (!strcmp(client_command, "book premium")) {
            if (already_booked) {
                write(client_sock_fds[*_index], "You have already booked flight. Cancel to book anouther place.", strlen("You have already booked flight. Cancel to book anouther place."));
                continue;
            } else if (selected_flight == -1) {
                write(client_sock_fds[*_index], "Select flight first", strlen("Select flight first"));
                continue;
            }
            write(client_sock_fds[*_index], "Premium place booked", strlen("Premium place booked"));
            booked_place = BOOK_PREMIUM;
            already_booked = 1;
        } else if (!strcmp(client_command, "cancel")) {
            if (already_booked) {
                already_booked = 0;
                booked_place = BOOK_DEFAULT;
                write(client_sock_fds[*_index], "Canceled.", strlen("Canceled."));
            } else {
                write(client_sock_fds[*_index], "Nothing to cancel.", strlen("Nothing to cancel."));
            }
        } 
        else if (!strcmp(client_command, "confirm")) {
            memset(response, 0, RESPONSE_SIZE);
            strcpy(response, "Booked: ");
            if (booked_place == BOOK_CHEAP) {
                flight_infos.cheap_places[selected_flight]--;
                strcat(response, "Cheap place, ");
            } else if (booked_place == BOOK_MIDDLE) {
                flight_infos.middle_places[selected_flight]--;
                strcat(response, "Middle place, ");
            } else if (booked_place == BOOK_PREMIUM) {
                flight_infos.premium_places[selected_flight]--;
                strcat(response, "Premium place, ");
            }
            char * flight_index = itoa(selected_flight + 1);
            strcat(response, flight_index);
            free(flight_index);
            strcat(response, "th flight. Lucky Flight!");
            write(client_sock_fds[*_index], response, RESPONSE_SIZE);
            already_booked = 0;
            booked_place = BOOK_DEFAULT;
            selected_flight = -1;
        }
    }
    close(client_sock_fds[*_index]);
    client_sock_fds[*_index] = -1;
    --client_count;
    free(_index);
    return NULL;
}