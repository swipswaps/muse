#ifndef MUSE_SERVER_TUI_HPP
#define MUSE_SERVER_TUI_HPP

#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <vector>
#include <linux/limits.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sys/wait.h>

#include "../shared.h"
#include "muse_server.h"

#define STATE_FILE_SIZE 4096

enum pages {
	MAIN_PAGE,
	NETWORK_PAGE,
	LIBRARY_PAGE,
	SERVER_PAGE
};

struct MenuItem {
	MenuItem(char* l, char* o, void* f, int p) :
			label(l), option(o), funct(f), chng_pg_to(p) { }
	char* label;
	char* option;
	void* funct;
	int chng_pg_to;
};

char port[5];
std::vector<char*> lib_paths;
int curr_page = MAIN_PAGE;
int muse_pid = -1;
int ascii_height, ascii_length;

void changePage(MENU* &menu, int page_num);
void handleMenuCallback(WINDOW* &win, MENU* &menu, void* callback, int index);
void writeInfoWindow(WINDOW* &win, int y, int x);
int confirmSelection(WINDOW* &win);
void exitMuse(WINDOW* &win, MENU* &menu);
void cleanup(MENU* &menu);
void cleanupServ(MENU* &menu);
void backgroundProc(MENU* &menu);
void updatePort(WINDOW* &win);
void refreshDatabase();
void addLibPath(WINDOW* win);
void removeLibPath(WINDOW* win);
void writeStateToFile();
void readStateFromFile();
void startServer(MENU* &menu);

void printASCII(FILE* file, int y, int x);
void wprintASCII(WINDOW* win, FILE* file, int y, int x);
int getASCIILength(FILE* file);
int getASCIIHeight(FILE* file);

const struct MenuItem main_page[] = {
	MenuItem("1.", "Network Options", (void*)changePage, NETWORK_PAGE),
	MenuItem("2.", "Library Locations", (void*)changePage, LIBRARY_PAGE),
	MenuItem("3.", "Start Server", (void*)startServer, -1),
	MenuItem("4.", "Exit Muse", (void*)exitMuse, -1)
};

const struct MenuItem port_page[] = {
	MenuItem("1.", "Specify Port", (void*)updatePort, -1),
	MenuItem("2.", "Back", (void*)changePage, MAIN_PAGE)
};

const struct MenuItem library_page[] = {
	MenuItem("1.", "Add Path", (void*)addLibPath, -1),
	MenuItem("2.", "Remove Path", (void*)removeLibPath, -1),
	MenuItem("3.", "Back", (void*)changePage, MAIN_PAGE)
};

const struct MenuItem server_page[] = {
	MenuItem("1.", "Kill Server", (void*)cleanupServ, MAIN_PAGE),
	MenuItem("2.", "Run In Background", (void*)backgroundProc, -1)
};

const struct MenuItem* page_select[] = { main_page, port_page, library_page, server_page };
const int page_length[] = { ARR_SIZE(main_page), ARR_SIZE(port_page), ARR_SIZE(library_page), ARR_SIZE(server_page) };

#endif
