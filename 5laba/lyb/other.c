#include "other.h"
int check_input(const char *input, const char *str1, const char *str2) {
    int match1 = strcmp(input, str1);
    int match2 = strcmp(input, str2);
    if (match1 == 0 || match2 == 0) {
        return 1;
    } else {
        return 0;
    }
}

int contains_indents(const char *str) {
    size_t len = strlen(str);
    for (int i = 0; i < len; i++) {
        if (str[i] == ' ') {
            return 1;
        }
    }
    return 0;
}

void get_input(char *input, const char *str1, const char *str2) {
    while (!check_input(input, str1, str2)) {
        printf("Wrong input!\n");
        fflush(stdin);
        fgets(input, MAX_LINE_LENGTH, stdin);
    }
}

int contains_multiple_dots(const char *str) {
    size_t len = strlen(str);
    int dot_count = 0;
    for (int i = 0; i < len; i++) {
        if (str[i] == '.') {
            dot_count++;
            if (dot_count >= 2) {
                return 1;
            }
        } else {
            dot_count = 0;
        }
    }
    return 0;
}

void get_line(char **line) {
    *line = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    fflush(stdin);
    fgets(*line, MAX_LINE_LENGTH, stdin);
}

char *add_extension(char *word, const char *extension)
{
    word[strcspn(word, "\n")] = '\0';
    strcat_s(word, MAX_LINE_LENGTH, extension);
    return word;
}

void get_ip_and_domain(const CACHE *cache, const char *file_name, char **domain, char **ip){

    FILE *file;
    fopen_s(&file, file_name, "rt");

    if(file == NULL)
        return;

    char *buff_domain = (char*) malloc (MAX_LINE_LENGTH * sizeof (char));
    char *buff_ip = (char*) malloc (MAX_LINE_LENGTH * sizeof (char));
    char *choose = (char*) malloc (MAX_LINE_LENGTH * sizeof (char));

    printf("Enter domain name of the site to get IP.\n");
    get_line(&buff_domain);
    buff_domain[strcspn(buff_domain, "\n")] = '\0';

    while(find_in_cache(buff_domain, cache) == NULL && get_ip_from_file(file, buff_domain) == NULL)
    {
        printf("This domain is not in the database try another one or add this buff_domain.(Try/Add)\n");
        get_line(&choose);
        get_input(choose, "Try\n", "Add\n");

        if (strcmp("Try\n", choose) == 0) {
            printf("Enter domain name of the site to get IP.\n");
            get_line(&buff_domain);
            buff_domain[strcspn(buff_domain, "\n")] = '\0';
        } else if (strcmp("Add\n", choose) == 0){
            fclose(file);

            fopen_s(&file, file_name, "at");
            if(file == NULL)
            {
                free(buff_ip);
                free(buff_domain);
                free(choose);
                return;
            }


            printf("It's original name or canonical name?(Original/Canonical)\n");
            get_line(&choose);
            get_input(choose, "Original\n", "Canonical\n");

            if(strcmp(choose, "Original\n") == 0){
                printf("Enter IP associated with that domain.\n");
                get_valid_ip_address(&buff_ip);

                fprintf(file, "%s IN A %s", buff_domain, buff_ip);
                strcpy_s(*ip, MAX_LINE_LENGTH, buff_ip);
                break;
            } else if(strcmp(choose, "Canonical\n") == 0){
                printf("Enter original domain.\n");
                char *orig_domain = malloc(MAX_LINE_LENGTH * sizeof (char));
                get_line(&orig_domain);


                add_extension(orig_domain, "\n");

                fclose(file);
                fopen_s(&file, file_name, "at");
                if (file == NULL)
                {
                    free(buff_ip);
                    free(orig_domain);
                    free(choose);
                    free(buff_domain);
                    return;
                }
                fprintf(file, "%s IN CNAME %s", buff_domain, orig_domain);
                free(buff_ip);
                free(orig_domain);
                free(choose);
                strcpy_s(*domain, MAX_LINE_LENGTH, buff_domain);
                free(buff_domain);
                fclose(file);
                return;
            }
        }
    }
    free(buff_ip);
    fclose(file);
    free(choose);
    strcpy_s(*domain, MAX_LINE_LENGTH, buff_domain);
    free(buff_domain);

}

char *get_ip_from_file(FILE *file, const char *key){
    char *line;
    char *buff_line = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    char *ip = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    line = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    rewind(file);
    while(fgets(line, MAX_LINE_LENGTH, file) != NULL){
        strcpy_s(buff_line, MAX_LINE_LENGTH, line);
        replace_char(buff_line, ' ', '\0');

        if(strcmp(buff_line, key) == 0)
        {
            if(strlen(buff_line) + 4 < strlen(line) && line[strlen(buff_line) + 4] == 'A')
            {
                strcpy_s(ip, MAX_LINE_LENGTH, (line + strlen(buff_line) + 6));
                free(line);
                free(buff_line);
                return ip;
            }
            else if(strlen(buff_line) + 4 < strlen(line) && line[strlen(buff_line) + 4] == 'C')
            {
                strcpy_s(ip, MAX_LINE_LENGTH, (line + strlen(buff_line) + 10));
                ip[strcspn(ip, "\n")] = '\0';
                char *result = get_ip_from_file(file, ip);
                free(line);
                free(buff_line);
                free(ip);
                return result;
            }

        }
    }
    free(ip);
    free(line);
    free(buff_line);
    return NULL;
}
