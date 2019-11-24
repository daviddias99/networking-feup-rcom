#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct ftp_url_st {
    char* user;
    char* password;
    char* host;
    char* url_path;
} ftp_url;

/**
 * @brief 
 * 
 * @param arg 
 * @param ftp 
 * @return  Returns 0 when successful, returns 1 in case of invalid format,
 *          returns 2 when unable to allocate enough memory
 */
int parse_arguments(char* arg, ftp_url* ftp) {
    // ftp://[<user>:<password>@]<host>/<url-path>
    if (strncmp(arg, "ftp://", 6) != 0) {
        perror("Invalid url header\n");
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
            return 2;
        strncpy(ftp->user, arg, str_len);

        arg = arg + str_len + 1;

        char_ptr = strchr(arg, '@');
        if (char_ptr == NULL)
            return 1;
    
        str_len = char_ptr - arg;
        ftp->password = malloc(str_len);
        if (ftp->password == NULL)
            return 2;
        strncpy(ftp->password, arg, str_len);

        arg = arg + str_len + 1;
    }
    else {
        ftp->user = NULL;
        ftp->password = NULL;
    }

    char_ptr = strchr(arg, '/');
    if (char_ptr == NULL)
        return 1;
    
    str_len = char_ptr - arg;
    ftp->host = malloc(str_len);
    if (ftp->host == NULL)
        return 2;
    strncpy(ftp->host, arg, str_len);
    arg += str_len + 1;

    
    ftp->url_path = malloc(strlen(arg));
    if (ftp->url_path == NULL)
        return 2;
    strcpy(ftp->url_path, arg);

    return 0;
}

int main(int argc, char** argv) {

    if (argc < 2) {
        perror("Please provide a URL\n");
        return 1;
    } else if (argc > 2) {
        perror("Too many arguments\n");
        return 1;
    }

    ftp_url ftp;
    int ret;
    if ((ret = parse_arguments(argv[1], &ftp)) != 0) {
        if (ret == 1)
            perror("Invalid URL format\n");
        if (ret == 2)
            perror("Unable to allocate memory\n");

        // TODO: free ftp
        return 1;
    }

    printf("Username : %s\n", ftp.user);
    printf("Password : %s\n", ftp.password);
    printf("Host : %s\n", ftp.host);
    printf("URL Path : %s\n", ftp.url_path);
    

    return 0;
}

