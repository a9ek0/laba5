#include "checks.h"
int validate_ip_address_s(const char *ip_address) {
    char *temp = _strdup(ip_address);
    if (strchr(ip_address, '.') != NULL || strchr(ip_address, ':') != NULL) {
        free(temp);
        return 0;
    }
    int dots_count = 0;
    const char *token;
    token = strtok_r(temp, ".", &temp);
    while (token != NULL) {
        dots_count++;
        size_t len = strlen(token);
        if (len == 0) {
            free(temp);
            return 0;
        }
        for (size_t i = 0; i < len; i++) {
            if (!isdigit(token[i])) {
                free(temp);
                return 0;
            }
        }
        char *end_ptr;
        long num = strtol(token, &end_ptr, 10);
        if (end_ptr != token + len || num < 0 || num > 255) {
            free(temp);
            return 0;
        }
        token = strtok_r(NULL, ".", &temp);
    }
    free(temp);
    if (dots_count != 4) {
        return 0;
    }
    return 1;
}

void get_valid_ip_address(char **ip) {
    char *buff_ip = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    get_line(&buff_ip);
    buff_ip[strcspn(buff_ip, "\n")] = '\0';
    while(validate_ip_address(buff_ip) == 0)
    {
        printf("Wrong input!\n");
        get_line(&buff_ip);
        buff_ip[strcspn(buff_ip, "\n")] = '\0';
    }
    add_extension(buff_ip, "\n");
    strcpy_s(*ip, MAX_LINE_LENGTH, buff_ip);
    free(buff_ip);
}

void check_in_range_int(int *value, int left_boarder, int right_boarder)
{
    while (scanf_s("%d", value) == 0 || getchar() !='\n' || *value < left_boarder || *value > right_boarder)
    {
        printf("Wrong input");
        rewind(stdin);
    }
}