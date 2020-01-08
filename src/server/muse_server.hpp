#ifndef MUSE_SERVER_HPP
#define MUSE_SERVER_HPP
#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sqlite3.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "../shared.hpp"

#define DEFAULT_PORT "2442"
#define BUFF_SIZE 256

int serve(char* port);
int handleRequest(int new_sockfd);
int sendSongCallback(void* unused, int colNum, char** column, char** result);
void stop(int sig);

#endif
