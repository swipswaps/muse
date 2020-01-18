#include "muse_client.h"

#ifdef TEST
int main(int argc, char** argv){
	connectToServ("2442", "127.0.0.1");
	struct songinfolst* song_info = (struct songinfolst*)calloc(1, sizeof(struct songinfolst));
	struct albuminfolst* album_info = (struct albuminfolst*)calloc(1, sizeof(struct albuminfolst));
	struct artistinfolst* artist_info = (struct artistinfolst*)calloc(1, sizeof(struct artistinfolst));
	queryAlbumSongs(25, &song_info);
	queryArtistAlbums(25, &album_info);
	queryGenreSongs("Rock", &song_info);
	queryGenre(argv);
	querySongs(&song_info);
	queryAlbums(&album_info);
	queryArtists(&artist_info);
	getSong(4062);
	struct albuminfolst* album_cursor = album_info;
	while(album_cursor != NULL){
		printf("%lu\n", album_cursor->id);
		printf("%s\n", album_cursor->title);
		printf("%lu\n\n", album_cursor->year);
		album_cursor = album_cursor->next;
	}
	free_songinfolst(song_info);
	free_albuminfolst(album_info);
	free_artistinfolst(artist_info);
	return 0;
}
#endif

int sockfd;

int connectToServ(char* port, char* server_ip){
	signal(SIGTERM, stop);
	signal(SIGHUP, stop);

	struct addrinfo seed,* server;

	memset(&seed, 0, sizeof(struct addrinfo));
	seed.ai_family = AF_UNSPEC;
	seed.ai_socktype = SOCK_STREAM;
	
	getaddrinfo(server_ip, port, &seed, &server);

	//Open the socket
	if((sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol)) == -1){
		fprintf(stderr, "Error opening a socket!\n");
		return 1;
	}

	if(connect(sockfd, server->ai_addr, server->ai_addrlen) == -1){
		printf("Could not connect to the server!\n");
		return 1;
	}

	freeaddrinfo(server);

	/*char* ch = "A message has been sent! Praise the sun!";
	if(send(sockfd, ch, 25, 0) == -1){
		printf("Could not send!\n");
	}*/

	return 0;
}

int getSong(unsigned long song_id){
	FILE* file = fopen("./song.mp3", "w");

	int request_size = sizeof(char) + sizeof(unsigned long);
	char* request = (char*)malloc(request_size);
	request[0] = REQSNG;
	*((unsigned long *)(request+1)) = song_id;
	
	if(send(sockfd, request, request_size, 0) == -1){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp_size_str = (char*)malloc(sizeof(unsigned long));
	if(recv(sockfd, resp_size_str, sizeof(unsigned long), 0) == -1){
		return 1;
	}
	unsigned long resp_size = (*((unsigned long*)(resp_size_str))) - sizeof(unsigned long);
	printf("Resp size: %lu\n", resp_size);
	char* resp = (char*)malloc(resp_size);

	if(recv(sockfd, resp, resp_size, 0) == -1){
		return 1;
	}
	free(resp_size_str);

	fwrite(resp, sizeof(char), resp_size, file);

	fclose(file);
	free(resp);

	return 0;
}

int querySongs(struct songinfolst** song_info){
	char request = QWRYSNG | ASC | ORDSNG;

	if(send(sockfd, &request, 1, 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		printf("Error recieving response!\n");
		return 1;
	}

	printf("Response:\n%s\n", resp);

	free(resp);

	return 0;
}

int queryAlbums(struct albuminfolst** album_info){
	char request = QWRYALBM | ASC | ORDSNG;

	if(send(sockfd, &request, 1, 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		printf("Error recieving response!\n");
		return 1;
	}

	printf("Response:\n%s\n", resp);

	free(resp);

	return 0;
}

int queryAlbumSongs(unsigned long album_id, struct songinfolst** song_info){
	if(queryEntity(album_id, QWRYALBMSNG | ASC)){
		printf("Error querying entity!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		printf("Error recieving response!\n");
		return 1;
	}

	printf("Response:\n%s\n", resp);

	free(resp);

	return 0;
}

int queryArtists(struct artistinfolst** artist_info){
	char request = QWRYART | ASC | ORDSNG;

	if(send(sockfd, &request, 1, 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		printf("Error recieving response!\n");
		return 1;
	}

	printf("Response:\n%s\n", resp);

	free(resp);

	return 0;
}

int queryArtistAlbums(unsigned long artist_id, struct albuminfolst** album_info){
	if(queryEntity(artist_id, QWRYARTALBM | ORDYR | ASC)){
		printf("Error querying entity!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		printf("Error recieving response!\n");
		free(album_info);
		return 1;
	}

	parseAlbums(resp, album_info);

	return 0;
}

int parseAlbums(char* resp, struct albuminfolst** album_info){
	*album_info = (struct albuminfolst*)calloc(1, sizeof(struct albuminfolst));

	struct albuminfolst* album_cursor = *album_info;
	char* resp_cursor = resp;
	resp_cursor = parseFieldLong(&(album_cursor->id), resp_cursor, '\t');
	resp_cursor = parseFieldStr(&(album_cursor->title), resp_cursor, '\t');
	resp_cursor = parseFieldLong(&(album_cursor->year), resp_cursor, '\n');

	while(*(resp_cursor) != '\0'){
		struct albuminfolst* new_album = (struct albuminfolst*)calloc(1, sizeof(struct albuminfolst));
		resp_cursor = parseFieldLong(&(new_album->id), resp_cursor, '\t');
		resp_cursor = parseFieldStr(&(new_album->title), resp_cursor, '\t');
		resp_cursor = parseFieldLong(&(new_album->year), resp_cursor, '\n');
		album_cursor->next = new_album;
		album_cursor = album_cursor->next;
	}
	return 0;
}

char* parseFieldStr(char** dest, char* base, char endchar){
	int str_size = substrsize(base, endchar) + 1;
	char* new_str = (char*)malloc(str_size + 2);
	*dest = new_str;
	snprintf(new_str, str_size, "%s", base);
	new_str[str_size] = '\0';
	return base + str_size + 1;
}

char* parseFieldLong(unsigned long* dest, char* base, char endchar){
	int str_size = substrsize(base, endchar);
	char* num_str = (char*)malloc(str_size + 1);
	substr(base, '\t', num_str, str_size);
	num_str[str_size] = '\0';
	*dest = strtoul(num_str, NULL, 10);
	free(num_str);
	return base + str_size + 1;
}

int queryGenre(char** genres){
	char request = QWRYGNR | ASC | ORDSNG;

	if(send(sockfd, &request, 1, 0) == 0){
		printf("Could not send request!\n");
		return 1;
	}

	char* resp = NULL;
	if(receiveResponse(&resp)){
		printf("Error recieving response!\n");
		return 1;
	}

	printf("Response:\n%s\n", resp);

	free(resp);

	return 0;
}

int queryGenreSongs(char* genre, struct songinfolst** song_info){
	if(genre == NULL){
		return 1;
	}
	int genre_size = strlen(genre);
	int request_size = genre_size + sizeof(char);
	char* request = (char*)calloc(request_size, 1);
	request[0] = QWRYGNRSNG | ASC | ORDSNG;
	strncpy(request+1, genre, genre_size);

	if(send(sockfd, request, request_size, 0) == -1){
		printf("Could not send request!\n");
		return 1;
	}

	free(request);

	char* resp = NULL;
	if(receiveResponse(&resp)){
		printf("Error recieving response!\n");
		return 1;
	}

	printf("Response:\n%s\n", resp);

	free(resp);

	return 0;
}

int queryEntity(unsigned long entity_id, char flags){
	if(entity_id == 0){
		return 1;
	}
	int request_size = sizeof(unsigned long) + sizeof(char);
	char* request = (char*)calloc(request_size, 1);
	request[0] = flags;
	*((unsigned long*)(request+1)) = entity_id;

	if(send(sockfd, request, request_size, 0) == -1){
		printf("Could not send request!\n");
		return 1;
	}

	free(request);

	return 0;
}

int receiveResponse(char** resp){
	char* resp_size_str = (char*)malloc(sizeof(unsigned long));
	if(recv(sockfd, resp_size_str, sizeof(unsigned long), 0) == -1){
		return 1;
	}
	unsigned long resp_size = (*((unsigned long*)(resp_size_str))) - sizeof(unsigned long);
	*resp = (char*)malloc(resp_size+1);
	//TODO: Make the server send the response size
	if(recv(sockfd, *resp, resp_size, 0) == -1){
		return 1;
	}
	(*resp)[resp_size] = '\0';
	free(resp_size_str);

	return 0;
}

void free_songinfolst(struct songinfolst* song_info){
	struct songinfolst* cursor = song_info;
	struct songinfolst* tmp;
	while(cursor != NULL){
        free(cursor->title);
        free(cursor->artist);
        free(cursor->album);
        free(cursor->genre);

        tmp = cursor;
        cursor = cursor->next;
        free(tmp);
	}
}

void free_albuminfolst(struct albuminfolst* album_info){
	struct albuminfolst* cursor = album_info;
	struct albuminfolst* tmp;
	while(cursor != NULL){
        free(cursor->title);

		tmp = cursor;
		cursor = cursor->next;
		free(tmp);
	}
}

void free_artistinfolst(struct artistinfolst* artist_info){
	struct artistinfolst* cursor = artist_info;
	struct artistinfolst* tmp;
	while(cursor != NULL){
        free(cursor->name);

		tmp = cursor;
		cursor = cursor->next;
		free(tmp);
	}
}

/** Iterates through a string and does a deep copy until a null byte or specified stopping point is hit
 * @param base The base string
 * @param until The ending character
 * @param cpy The char* to copy file contents into
 * @param cpySize The size of cpy
 * @return 1 on success, 0 otherwise
 */
int substr(char* base, char until, char* cpy, int cpySize){
    //Iterate through until we hit a null byte or the delim
    int i;
    for(i = 0; *base != until && i < cpySize; i++){
        *(cpy++) = *(base++);
    }
    if(i == cpySize){
        return 0;
    }
    *cpy = '\0';
    return 1;
}

int substrsize(char* str, char until){
	int i;
	for(i = 0; str[i] != until && str[i] != '\0'; i++){
		continue;
	}
	return i;
}
void free_genreinfolst(struct genreinfolst* genre_info){
    struct genreinfolst* cursor = genre_info;
    struct genreinfolst* tmp;
    while(cursor != NULL){
        free(cursor->genre);

        tmp = cursor;
        cursor = cursor->next;
        free(tmp);
    }
}

//TODO: Remove circularly linked list in server, change out w/ normal list. I dunno why I thought circularly linked was the right implementation for that :P

void stop(int sig){
	//Terminate the connection
	send(sockfd, "\0", 1, 0);
	close(sockfd);
	exit(0);
}