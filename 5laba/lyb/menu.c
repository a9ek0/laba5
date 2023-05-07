#include "menu.h"


void menu(CACHE *cache, const char *file_name){
    int choose = 0;
    while(1) {
        printf("Choose option:\n"
               "1.Find IP for domain.\n"
               "2.Find domains for IP\n"
               "3.Exit\n");
        check_in_range_int(&choose, 1, 3);
        switch (choose) {
            case 1:
                domain_to_ip(cache, file_name);
                break;
            case 2:

                find_ip_domains(cache, file_name);
                break;
            case 3:
                return;
            default:
                break;
        }
    }
}

void domain_to_ip(CACHE *cache, const char *file_name)
{
    FILE *file;

    char *domain = (char*)malloc(MAX_LINE_LENGTH * sizeof (char));
    char *ip = (char*)malloc(MAX_LINE_LENGTH * sizeof (char));
    char *expression = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));

    strcpy_s(expression, MAX_LINE_LENGTH, "Yes\n");

    while(strcmp(expression, "Yes\n") == 0 && strcmp(expression, "No\n") != 0){

        get_ip_and_domain(cache, file_name, &domain, &ip);

        const char *value = find_in_cache(domain, cache);

        if (value == NULL) {

            fopen_s(&file, file_name, "rt");
            if(file == NULL)
                exit(EXIT_FAILURE);

            ip = get_ip_from_file(file, domain);
            put(cache, domain, ip);
            printf("MISS - %s", ip);
        } else {
            update_position(&cache, domain);
            printf("HIT - %s", value);
        }
        print_cache(cache);

       printf("Do you want to continue?\n");
       get_line(&expression);
       get_input(expression, "Yes\n", "No\n");
    }
    free(expression);
    free(ip);
    free(domain);
}

void print_ip(const CACHE *cache) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        NODE *node = cache->hash[i];
        while (node != NULL) {
            printf("%s", node->value);
            node = node->next;
        }
    }
}



int find_ip(const CACHE *cache, const char *value) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        NODE *node = cache->hash[i];
        while (node != NULL) {
            if (strcmp(node->value, value) == 0) {
                return 1;
            }
            node = node->next;
        }
    }
    return 0;
}

void find_ip_domains(const CACHE *cache, const char *file_name){
    char *ip = (char*)malloc(MAX_LINE_LENGTH * sizeof (char));
    printf("Select the ip whose domains you want to see.\n");
    print_ip(cache);
    get_valid_ip_address(&ip);

    while(is_in_file(file_name, ip) == 0)
    {
        printf("Wrong input!\n");
        get_valid_ip_address(&ip);

    }
    FILE *file;
    fopen_s(&file, file_name, "rt");
    if(file == NULL)
        return;
    ip_to_domains(file, ip);
    fclose(file);
}
void domain_to_ips(const char *filename, const char *domain) {
    FILE *file;
    fopen_s(&file, filename, "rt");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *ptr;
        ptr = strtok(line, " \t\n");
        if (ptr == NULL) {
            continue;
        }

        if (strcmp(ptr, domain) != 0) {
            continue;
        }

        ptr = strtok(line, " \t\n");
        if (ptr == NULL) {
            continue;
        }

        if (strcmp(ptr, "IN") != 0) {
            continue;
        }

        ptr = strtok(line, " \t\n");
        if (ptr == NULL) {
            continue;
        }

        if (strcmp(ptr, "A") == 0) {
            ptr = strtok(line, " \t\n");
            if (ptr != NULL) {
                printf("%s\n", ptr);
            }
        } else if (strcmp(ptr, "CNAME") == 0) {
            ptr = strtok(line, " \t\n");
            if (ptr != NULL) {
                domain_to_ips(filename, ptr);
            }
        }
    }

    fclose(file);
}

void ip_to_domains(FILE *file, const char *ip) {
    char *line = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    char *buff_ip = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    const char *pos;
    long long position;

    while(fgets(line, MAX_LINE_LENGTH, file) != NULL){
        pos = strstr(line, "CNAME");
        if(pos != NULL){
            position = line - pos;
            strcpy_s(buff_ip, MAX_LINE_LENGTH, (line - (int)position) + 6);
            buff_ip[strcspn(buff_ip, "\n")] = '\0';
            replace_char(line, ' ', '\0');
            find_original(file, buff_ip, ip, line);

        } else{
            pos = strstr(line, " ");
            position = line - pos;
            strcpy_s(buff_ip, MAX_LINE_LENGTH, (line - (int)position) + 6);
            if(strcmp(buff_ip, ip) == 0){
                replace_char(line, ' ', '\0');
                printf("%s\n",  line);
            }
        }
    }
    free(buff_ip);
    free(line);
}

void find_original(FILE *file, const char *domain, const char *ip, const char *print) {
    char *line = (char *) malloc(MAX_LINE_LENGTH * sizeof(char));
    char *buff_ip = (char *) malloc(MAX_LINE_LENGTH * sizeof(char));
    char *buff_line = (char *) malloc(MAX_LINE_LENGTH * sizeof(char));
    const char *pos;
    long long position;
    size_t pos1 = ftell(file);
    rewind(file);
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        pos = strstr(line, "CNAME");
        if (pos == NULL) {
            pos = strstr(line, " ");
            position = line - pos;
            strcpy_s(buff_ip, MAX_LINE_LENGTH, (line - (int) position) + 6);
            replace_char(line, ' ', '\0');
            if (strcmp(line, domain) == 0 && strcmp(buff_ip, ip) == 0) {
                printf("%s\n",  print);
            }
        }
    }
    free(line);
    free(buff_ip);
    free(buff_line);
    fseek(file, (long)pos1, SEEK_SET);
}
void print_ip_from_file(const char *file_name) {
    char *line = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    const char *pos;
    long long position;
    FILE *file;
    fopen_s(&file, file_name, "rt");
    if(file == NULL)
    {
        free(line);
        return;
    }
    char *buff_ip = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    while(fgets(line, MAX_LINE_LENGTH, file) != NULL){
        pos = strstr(line, "CNAME");
        if(pos != NULL){
            position = line - pos;
            strcpy_s(buff_ip, MAX_LINE_LENGTH, (line - (int)position) + 6);
            printf("%s",  buff_ip);
        } else{
            pos = strstr(line, " ");
            position = line - pos;
            strcpy_s(buff_ip, MAX_LINE_LENGTH, (line - (int)position) + 6);
            printf("%s",  buff_ip);
        }
    }
    free(buff_ip);
    free(line);
    fclose(file);
}

int is_in_file(const char *file_name, const char *ip) {
    char *line = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    const char *pos;
    long long position;
    FILE *file;
    fopen_s(&file, file_name, "rt");
    if(file == NULL)
    {
        free(line);
        return 0;
    }
    char *buff_ip = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    while(fgets(line, MAX_LINE_LENGTH, file) != NULL){
        pos = strstr(line, "CNAME");
        if(pos != NULL){
            position = line - pos;
            strcpy_s(buff_ip, MAX_LINE_LENGTH, (line - (int)position) + 6);
            buff_ip[strcspn(buff_ip, "\n")] = '\0';
            if(strcmp(buff_ip, ip) == 0)
            {
                free1(buff_ip, line, file);
                return 1;
            }
        } else{
            pos = strstr(line, " ");
            position = line - pos;
            strcpy_s(buff_ip, MAX_LINE_LENGTH, (line - (int)position) + 6);
            if(strcmp(buff_ip, ip) == 0){
                {
                    free1(buff_ip, line, file);
                    return 1;
                }
            }
        }
    }
    free1(buff_ip, line, file);
    return 0;
}

void replace_char(char *line, char old_char, char new_char)
{
    size_t length = strlen(line);
    for (int i = 0; i < length; i++) {
        if (line[i] == old_char) {
            line[i] = new_char;
        }
    }
}

void free1(char *str1, char *str2, FILE *file)
{
    free(str1);
    free(str2);
    fclose(file);
}