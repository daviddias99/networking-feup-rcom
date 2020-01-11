#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

#include <sys/socket.h>

#include <ctype.h>
#include <unistd.h>

#include <sys/stat.h>


#define SERVER_PORT 21

/*
speedtest.tele2.net
www.wftpserver.com/onlinedemo.htm
*/


struct ftp_st {
    // ftp://[<user>:<password>@]<host>/<url-path>
    char* user;
    char* password;
    char* host;
    char* url_path;
};

char response[255];
size_t response_index = 0;

struct ftp_st ftp;


int parse_arguments(char* arg, struct ftp_st* ftp);
char* get_file_name(char* file_path);
int get_file_size();
void read_response(int socket_fd, char* response_code);
int download_file(int socket_fd, char* file_path);
int send_command(int socket_fd, char* command, char* command_args, int socket_fd_client);
int get_server_port(int socket_fd);
void free_ftp();

int main(int argc, char** argv) {

    if (argc < 2) {
        perror(" > Please provide a URL\n");
        return 1;
    } else if (argc > 2) {
        perror(" > Too many arguments\n");
        return 1;
    }

    atexit(free_ftp);
    int ret;
    if ((ret = parse_arguments(argv[1], &ftp)) < 0) {
        if (ret == -1)
            perror(" > Invalid URL format\n");
        else if (ret == -2)
            perror(" > Unable to allocate memory\n");

        exit(1);
    }

    printf(" - Username : %s\n", ftp.user);
    printf(" - Password : %s\n", ftp.password);
    printf(" - Host : %s\n", ftp.host);
    printf(" - URL Path : %s\n", ftp.url_path);

    struct hostent *h;
    if ((h = gethostbyname(ftp.host)) == NULL) {  
        perror(" > gethostbyname");
        exit(1);
    }

    printf(" - IP Address : %s\n\n",inet_ntoa(*((struct in_addr *)h->h_addr)));

    int socket_fd, socket_fd_client = -1;
	struct sockaddr_in server_addr, server_addr_client;

    bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)h->h_addr))); /*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(SERVER_PORT);											/*server TCP port must be network byte ordered */

    // open TCP socket
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror(" > Error opening socket");
		exit(0);
	}

    // connect to the server
	if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror(" > Error connecting to server");
		exit(0);
	}

    char response_code[3];
    read_response(socket_fd, response_code);

    if (response_code[0] == '2') {
        printf(" > Connection established\n");
    }

    printf("\n > Sending Username\n");
    ret = send_command(socket_fd, "user ", ftp.user, socket_fd_client);
    if (ret) {
        printf(" > Sending Password\n\n");
        ret = send_command(socket_fd, "pass ", ftp.password, socket_fd_client);
    }
    
    // TODO: handle errors
    if (ret < 0) {
        exit(1);
    }

    int server_port = get_server_port(socket_fd);
    
    /*server address handling*/
	bzero((char *)&server_addr_client, sizeof(server_addr_client));
	server_addr_client.sin_family = AF_INET;
	server_addr_client.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)h->h_addr))); /*32 bit Internet address network byte ordered*/
	server_addr_client.sin_port = htons(server_port);										   /*server TCP port must be network byte ordered */

	/*open an TCP socket*/
	if ((socket_fd_client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror(" > socket()");
		exit(0);
	}

	/*connect to the server*/
	if (connect(socket_fd_client, (struct sockaddr *)&server_addr_client, sizeof(server_addr_client)) < 0) {
		perror(" > connect()");
		exit(0);
	}

    // printf("\n > Sending RETR\n");
    ret = send_command(socket_fd, "retr ", ftp.url_path, socket_fd_client);
    if (ret < 0) {
        printf(" > Error in RETR response\n");
        close(socket_fd_client);
        close(socket_fd);
        exit(1);
    }

    ret = send_command(socket_fd, "quit", "", socket_fd_client);
    if (ret < 0) {
        printf(" > Error in QUIT response\n");
        close(socket_fd_client);
        close(socket_fd);
        exit(1);
    }

    close(socket_fd_client);
    close(socket_fd);
    exit(0);
}

void free_ftp() {
    free(ftp.host);
    free(ftp.password);
    free(ftp.url_path);
    free(ftp.user);
}



/**
 * @brief 
 * 
 * @param arg 
 * @param ftp 
 * @return  Returns 0 when successful, returns -1 in case of invalid format,
 *          returns -2 when unable to allocate enough memory
 */
int parse_arguments(char* arg, struct ftp_st* ftp) {
    // ftp://[<user>:<password>@]<host>/<url-path>
    if (strncmp(arg, "ftp://", 6) != 0) {
        perror(" > Invalid URL header\n");
        return 1;
    }
    
    // remove the first 6 chars
    arg = arg + 6;

    char* char_ptr = strchr(arg, ':');
    int str_len;
    if (char_ptr != NULL) {

        str_len = char_ptr - arg;
        ftp->user = malloc(str_len);
        if (ftp->user == NULL) 
            return -2;
        strncpy(ftp->user, arg, str_len);

        arg = arg + str_len + 1;

        char_ptr = strchr(arg, '@');
        if (char_ptr == NULL)
            return -1;
    
        str_len = char_ptr - arg;
        ftp->password = malloc(str_len);
        if (ftp->password == NULL)
            return -2;
        strncpy(ftp->password, arg, str_len);

        arg = arg + str_len + 1;
    }
    else {
        ftp->user = strdup("anonymous");
        ftp->password = strdup("anonymous");
    }

    char_ptr = strchr(arg, '/');
    if (char_ptr == NULL)
        return -1;
    
    str_len = char_ptr - arg;
    ftp->host = malloc(str_len);
    if (ftp->host == NULL)
        return -2;
    strncpy(ftp->host, arg, str_len);
    arg += str_len + 1;

    
    ftp->url_path = malloc(strlen(arg));
    if (ftp->url_path == NULL)
        return -2;
    strcpy(ftp->url_path, arg);

    return 0;
}

void read_response(int socket_fd, char* response_code) {
    int state = 0;
    size_t index = 0;
    char read_char;

    while(state != 3) {
        // TODO: deal with read errors
        read(socket_fd, &read_char, 1);
        printf("%c", read_char);
        response[response_index++] = read_char;

        switch (state) {
        // reading the response code
        case 0:
            if (read_char == ' ') {
                if (index != 3) {
                    printf(" > Error : response code incomplete\n");
                    return;
                }
                index = 0;
                state = 1;
            }
            else if (read_char == '-') {
                index = 0;
                state = 2;
            }
            else if (isdigit(read_char) && index < 3) {
                response_code[index] = read_char;
                index++;
            }
            break;

        // read until the end of the line
        case 1:
            if (read_char == '\n') {
                state = 3;
            }
            break;

        // wait for response code in multiple line responses
        case 2:
            if (read_char == response_code[index]) {
                index++;
            }
            else if (index == 3 && read_char == ' ') {
                state = 1;
            }
            else if (index == 3 && read_char == '-') {
                index = 0;
            }
            break;
        
        default:
            break;
        }
    }

    response[response_index] = '\0';
    response_index = 0;
}

char* get_file_name(char* file_path) {
    char* file_name = file_path;

    for (int i = strlen(file_path) - 1; i >= 0; i--) {
        if (file_path[i] == '/') {
            file_name = file_path + i + 1;
            break;
        }
    }

    return file_name;
}

int get_file_size() {
    char* start = strchr(response, '(');
    char* end = strchr(start, ' ');
    int size = end - start;
    char* str = strndup(start + 1, size);
    size = atoi(str);
    free(str);
    return size;
}

void progress_bar(const char* prefix, size_t count, size_t max_count) {

    int progress = count * 100 / max_count;
    
    fflush(stdout);
    printf("\r%s : %3d%% [", prefix, progress);

    for (uint8_t i = 0; i < progress; i++)
        printf("#");
    for (uint8_t i = progress; i < 100; i++)
        printf(" ");
    printf("]"); 

	if (progress == 100)
		printf("\n");
}

int download_file(int socket_fd, char* file_path) {
    char* file_name = get_file_name(file_path);
    int file_size = get_file_size();

    FILE *file = fopen(file_name, "wb+");
    char buffer[1000];
    int bytes, progress = 0;
    while ((bytes = read(socket_fd, buffer, 1000)) > 0) {
        fwrite(buffer, bytes, 1, file);
        progress += bytes;
        progress_bar(file_name, progress, file_size);
    }

    fclose(file);
}

int send_command(int socket_fd, char* command, char* command_args, int socket_fd_client) {
    char response_code[3];
    int action = 0;

    // send the command
    write(socket_fd, command, strlen(command));
    write(socket_fd, command_args, strlen(command_args));
    write(socket_fd, "\n", 1);

    while (1) { 
        read_response(socket_fd, response_code);
        action = response_code[0] - '0';

        switch (action) {
            // positive preliminary reply
            case 1:
                if (strcmp(command, "retr ") == 0) {
                    download_file(socket_fd_client, command_args);
                    break;
                }
                read_response(socket_fd, response_code);
                break;

            // positive completion reply
            case 2:
                return 0;

            // positive intermidiate reply
            // the command needs additional information
            case 3:
                return 1;

            // transient negative completion reply
            // send the command again
            case 4:
                write(socket_fd, command, strlen(command));
			    write(socket_fd, command_args, strlen(command_args));
			    write(socket_fd, "\r\n", 2);
                break;

            // permanent negative completion reply
            case 5:
                return -1;

            // protected reply
            case 6:
                return -1;

            default:
                return -1;
        }
    }
}

int get_server_port(int socket_fd) {

    write(socket_fd, "pasv\n", 5);

    int state = 0;
    int index = 0;

    char first_byte[4], second_byte[4];
    char read_char;

    while (state != 7) {
        read(socket_fd, &read_char, 1);
        printf("%c", read_char);

        switch (state) {

            case 0:
                if (read_char == ' ') {
                    if (index != 3) {
                        printf(" > Error receiving response code");
                        return -1;
                    }
                    index = 0;
                    state = 1;
                }
                else {
                    index++;
                }
                break;

            case 5:
                if (read_char == ',') {
                    index = 0;
                    state = 6;
                }
                else {
                    first_byte[index] = read_char;
                    index++;
                }
                break;
            
            case 6:
                if (read_char == ')') {
                    state++;
                }
                else {
                    second_byte[index] = read_char;
                    index++;
                }
                break;
            
            default:
                if (read_char == ',') {
                    state++;
                }
                break;
        }
    }

    return atoi(first_byte) * 256 + atoi(second_byte);
}
