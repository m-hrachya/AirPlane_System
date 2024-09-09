#ifndef LIB
#define LIB

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>

#define MAX_CLIENT_COUNT 10
#define NAME_SIZE 128
#define COMMAND_SIZE 128
#define FLIGHTS_COUNT 3
#define LAST_NAME_SIZE 128
#define BOOK_DEFAULT 0
#define BOOK_CHEAP 1
#define BOOK_MIDDLE 2
#define BOOK_PREMIUM 3
#define RESPONSE_SIZE 1024
#define BUFFER_SIZE 1024
#define PASSPORT_NUMBER_SIZE 100

typedef struct flights_t {
    char available_flights[FLIGHTS_COUNT][NAME_SIZE];
    int premium_places[FLIGHTS_COUNT];
    int middle_places[FLIGHTS_COUNT];
     int cheap_places[FLIGHTS_COUNT];
} flights_t;

typedef struct client_t {
    char client_name[NAME_SIZE];
    char client_last_name[LAST_NAME_SIZE];
    int age;
    char pasport_number[PASSPORT_NUMBER_SIZE];
} client_t;

void initializeClientSockFds();
char* itoa(int value);
void initializeFlightInfos();
int getEmptyClientSockFd();
void setClientSockFd(int index, int value);
void * thread_fnc(void * arg);

#endif