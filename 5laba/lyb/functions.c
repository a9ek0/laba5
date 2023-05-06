#include "functions.h"
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
    strcat(word, extension);
    return word;
}

void get_ip_and_domain(const CACHE *cache, const char *file_name, char **domain, char **ip){

    FILE *file;
    file = fopen(file_name, "rt");

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

            file = fopen(file_name, "at");
            if(file == NULL)
            {
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
                strcpy(*ip, buff_ip);

                free(buff_ip);
                break;
            } else if(strcmp(choose, "Canonical\n") == 0){
                printf("Enter original domain.\n");
                char *orig_domain = malloc(MAX_LINE_LENGTH * sizeof (char));
                get_line(&orig_domain);


                add_extension(orig_domain, "\n");

                file = fopen(file_name, "at");

                fprintf(file, "%s IN CNAME %s", buff_domain, orig_domain);

                free(orig_domain);
                fclose(file);
                free(choose);
                strcpy(*domain, buff_domain);
                free(buff_domain);
                return;
            }
        }
    }
    fclose(file);
    free(choose);
    strcpy(*domain, buff_domain);
    free(buff_domain);

}

char *get_ip_from_file(FILE *file, const char *key){
    char *line;
    char *buff_line = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    char *ip = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    line = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    rewind(file);
    while(fgets(line, MAX_LINE_LENGTH, file) != NULL){
        strcpy(buff_line, line);
        buff_line[strcspn(buff_line, " ")] = '\0';

        if(strcmp(buff_line, key) == 0)
        {
            if(line[strlen(buff_line) + 4] == 'A')
            {
                strcpy(ip, (line + strlen(buff_line) + 6));
                free(line);
                free(buff_line);
                return ip;
            }
            else if(line[strlen(buff_line) + 4] == 'C')
            {
                strcpy(ip, (line + strlen(buff_line) + 10));
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

    strcpy(expression, "Yes\n");

    while(strcmp(expression, "Yes\n") == 0 && strcmp(expression, "No\n") != 0){

        get_ip_and_domain(cache, file_name, &domain, &ip);

        const char *value = find_in_cache(domain, cache);

        if (value == NULL) {

            file = fopen(file_name, "rt");
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

        // printf("Do you want to continue?\n");
        // get_line(&expression);
        // get_input(expression, "Yes\n", "No\n");
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



int find_ip(CACHE *cache, const char *value) {
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
    file = fopen(file_name, "rt");
    if(file == NULL)
        return;
    ip_to_domains(file, ip);
    fclose(file);
}


void find_ip_domains1(const char *file_name, char *ip){
    FILE *file;
    char *line = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    char *buff_line = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    char *buff_ip = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    file = fopen(file_name, "rt");
    while(fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        strcpy(buff_line, line);
        buff_line[strcspn(buff_line, " ")] = '\0';

        if (line[strlen(buff_line) + 4] == 'A') {
            strcpy(buff_ip, (line + strlen(buff_line) + 6));
            if(strcmp(buff_ip, ip) == 0)
                printf("%s", buff_line);
        } else if (line[strlen(buff_line) + 4] == 'C') {
            strcpy(buff_ip, (line + strlen(buff_line) + 10));
            //ip[strcspn(buff_ip, "\n")] = '\0';
            if(strcmp(buff_ip, ip) == 0)
                printf("%s", buff_line);
            find_ip_domains1(file_name, buff_ip);
        }
    }
    free(line);
    fclose(file);
}

void domain_to_ips(const char *filename, const char *domain) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char *ptr = strtok(line, " \t\n");
        if (ptr == NULL) {
            continue;
        }

        if (strcmp(ptr, domain) != 0) {
            continue;
        }

        ptr = strtok(NULL, " \t\n");
        if (ptr == NULL) {
            continue;
        }

        if (strcmp(ptr, "IN") != 0) {
            continue;
        }

        ptr = strtok(NULL, " \t\n");
        if (ptr == NULL) {
            continue;
        }

        if (strcmp(ptr, "A") == 0) {
            ptr = strtok(NULL, " \t\n");
            if (ptr != NULL) {
                printf("%s\n", ptr);
            }
        } else if (strcmp(ptr, "CNAME") == 0) {
            ptr = strtok(NULL, " \t\n");
            if (ptr != NULL) {
                domain_to_ips(filename, ptr);
            }
        }
    }

    fclose(fp);
}

void ip_to_domains(FILE *file, const char *ip) {
    char *line = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    char *buff_ip = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    const char *pos;
    long double position;

    while(fgets(line, MAX_LINE_LENGTH, file) != NULL){
        pos = strstr(line, "CNAME");
        if(pos != NULL){
            position = line - pos;
            strcpy(buff_ip, (line - (int)position) + 6);
            buff_ip[strcspn(buff_ip, "\n")] = '\0';
            line[strcspn(line, " ")] = '\0';
            find_original(file, buff_ip, ip, line);

        } else{
            pos = strstr(line, " ");
            position = line - pos;
            strcpy(buff_ip, (line - (int)position) + 6);
            if(strcmp(buff_ip, ip) == 0){
                line[strcspn(line, " ")] = '\0';
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
    long double position;
    size_t pos1 = ftell(file);
    rewind(file);
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        pos = strstr(line, "CNAME");
        if (pos == NULL) {
            pos = strstr(line, " ");
            position = line - pos;
            strcpy(buff_ip, (line - (int) position) + 6);
            line[strcspn(line, " ")] = '\0';
            if (strcmp(line, domain) == 0 && strcmp(buff_ip, ip) == 0) {
                printf("%s\n",  print);
            }
        }
    }
    free(line);
    free(buff_ip);
    free(buff_line);
    fseek(file, pos1, SEEK_SET);
}
void print_ip_from_file(const char *file_name) {
    char *line = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    char *pos;
    long double position;
    FILE *file;
    file = fopen(file_name, "rt");
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
            strcpy(buff_ip, (line - (int)position) + 6);
            printf("%s",  buff_ip);
        } else{
            pos = strstr(line, " ");
            position = line - pos;
            strcpy(buff_ip, (line - (int)position) + 6);
            printf("%s",  buff_ip);
        }
    }
    free(buff_ip);
    free(line);
    fclose(file);
}

int is_in_file(const char *file_name, const char *ip) {
    char *line = (char*) malloc(MAX_LINE_LENGTH * sizeof (char));
    char *pos;
    long double position;
    FILE *file;
    file = fopen(file_name, "rt");
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
            strcpy(buff_ip, (line - (int)position) + 6);
            buff_ip[strcspn(buff_ip, "\n")] = '\0';
            if(strcmp(buff_ip, ip) == 0)
            {
                return 1;
            }

        } else{
            pos = strstr(line, " ");
            position = line - pos;
            strcpy(buff_ip, (line - (int)position) + 6);
            if(strcmp(buff_ip, ip) == 0){
                return 1;
            }
        }
    }
    free(buff_ip);
    free(line);
    fclose(file);
    return 0;
}

void free_cache(CACHE *cache) {
    NODE *current = cache->head;
    while (current != NULL) {
        NODE *next = current->next;
        free(current->key);
        free(current);
        current = next;
    }

    for (int i = 0; i < CACHE_SIZE; i++) {
        NODE *bucket = cache->hash[i];
        while (bucket != NULL) {
            NODE *next = bucket->next;
            free(bucket->value);
            free(bucket);
            bucket = next;
        }
    }
    free(cache->hash);

    free(cache);
}

void free_node(NODE *node) {
    free(node->key);
    free(node->value);
    free(node);
}

CACHE* create_cache() {
    CACHE *cache = (CACHE*) malloc(sizeof(CACHE));
    cache->size = 0;
    cache->head = NULL;
    cache->tail = NULL;
    cache->hash = (NODE**) malloc(CACHE_SIZE * sizeof(NODE*));
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache->hash[i] = NULL;
    }
    return cache;
}

NODE* create_node(char *key, char *value) {
    NODE *node = (NODE*) malloc(sizeof(NODE));
    node->key = key;
    node->value = value;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

int validate_ip_address(const char *ip_address) {
    char *temp = strdup(ip_address);
    if (contains_multiple_dots(ip_address) || contains_indents(ip_address)) {
        free(temp);
        return 0;
    }
    int dots_count = 0;
    const char *token = strtok(temp, ".");
    while (token != NULL) {
        dots_count++;
        int len = strlen(token);
        if (len == 0) {
            free(temp);
            return 0;
        }
        for (int i = 0; i < len; i++) {
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
        token = strtok(NULL, ".");
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
    strcpy(*ip, buff_ip);
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

void dell_oldest_from_cache(CACHE **cache){
    if ((*cache)->head == NULL) {
        return;
    }

    NODE *tail_node = (*cache)->tail;
    if (tail_node == (*cache)->head) {
        free_node(tail_node);
        free(*cache);
        *cache = NULL;
        return;
    }

    tail_node->prev->next = NULL;
    (*cache)->tail = tail_node->prev;

}















void dell_from_hash(CACHE *cache, const char *key) {
    int bucket = calculate_hash(key);
    NODE *curr_node = cache->hash[bucket];

    if (curr_node != NULL && strcmp(curr_node->key, key) == 0) {
        cache->hash[bucket] = curr_node->next;
        free_node(curr_node);
        return;
    }

    while (curr_node != NULL) {
        if (strcmp(curr_node->key, key) == 0) {
            curr_node->prev->next = curr_node->next;
            if (curr_node->next != NULL) {
                curr_node->next->prev = curr_node->prev;
            }
            free_node(curr_node);
            return;
        }
        curr_node = curr_node->next;
    }
}



void put(CACHE *cache, const char *key, const char *value) {
    char *existing_value = get(cache, key);
    if (existing_value != NULL) {
        strncpy(existing_value, value, strlen(value) + 1);
        return;
    }
    NODE *node = (NODE*) malloc(sizeof(NODE));
    node->key = (char*) malloc(strlen(key) + 1);
    node->value = (char*) malloc(strlen(value) + 1);
    strncpy(node->key, key, strlen(key) + 1);
    strncpy(node->value, value, strlen(value) + 1);
    node->next = NULL;
    node->prev = NULL;
    if(cache->head == NULL) {
        cache->head = node;
        cache->tail = node;
    } else {
        node->next = cache->head;
        cache->head->prev = node;
        cache->head = node;
        cache->tail->next = NULL;
    }
    //cache->head = node;
    int hash_val = calculate_hash(key);
    //node->next = cache->hash[hash_val];
    //node->prev = NULL;
    if (cache->hash[hash_val] == NULL) {
        cache->hash[hash_val] = node;
    }else{
        NODE *tmp = cache->hash[hash_val];
        while(tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = node;
        node->prev = tmp;
    }
    if (cache->size == CACHE_SIZE) {
        dell_tail(cache);
    } else {
        cache->size++;
    }
    cache->tail->next = NULL;
}

void dell_tail(CACHE *cache) {
    NODE *node_to_remove = cache->tail;
    cache->tail = node_to_remove->prev;
    if (cache->tail != NULL) {
        cache->tail->next = NULL;
    }
    int hash_val = calculate_hash(node_to_remove->key);
    NODE *node = cache->hash[hash_val];
    while (node != NULL) {
        if (node == node_to_remove) {
            cache->hash[hash_val] = node_to_remove->next;
        } else if (node->next == node_to_remove) {
            node->next = node_to_remove->next;
        }
        if (node == node_to_remove || node->next == node_to_remove)
            break;
        node = node->next;
    }
    free(node_to_remove->key);
    free(node_to_remove->value);
    free(node_to_remove);
}



void add_to_cache(CACHE *cache, const char *key, const char *value) {

    int hash = calculate_hash(key);
    NODE *hash_node = (NODE*) malloc(sizeof(NODE));
    NODE *cache_node = (NODE*) malloc(sizeof(NODE));
    cache_node->key = strdup(key);
    hash_node->value = strdup(value);
    hash_node->next = cache_node->next = NULL;
    hash_node->prev = cache_node->prev = NULL;

    if(cache->hash[hash] == NULL) {
        cache->hash[hash] = hash_node;
    } else {
        NODE *tmp = cache->hash[hash];
        while(tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = hash_node;
        hash_node->prev = tmp;
    }

    if(cache->head == NULL) {
        cache->head = cache_node;
        cache->tail = cache_node;
    } else {
        cache_node->next = cache->head;
        cache->head->prev = cache_node;
        cache->head = cache_node;
        cache->tail->next = NULL;
    }
    cache->size++;

    if(cache->size > CACHE_SIZE) {
        cache->size--;
        dell_oldest_from_cache(&cache);
    }

}

int calculate_hash(const char *key){
    int hash = 0;
    for (int i = 0; i < strlen(key); ++i) {
        hash += key[i];
    }
    return hash % CACHE_SIZE;
}

char *find_in_cache(const char *key, const CACHE *cache) {
    int hash_val = calculate_hash(key);
    NODE *node = cache->hash[hash_val];
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }
    return NULL;
}

/*NODE *find_in_cache(const char *key, const CACHE *cache) {
    NODE *tmp = cache->head;
    int hash = calculate_hash(key);
    if(tmp == NULL)
        return NULL;
    for (int i = 0; i < cache->size; ++i) {
        if(strcmp(tmp->key, key) == 0) {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}*/

void print_cache(const CACHE *cache) {
    NODE *node = cache->head;
    while (node != NULL) {
        printf("%s: %s", node->key, node->value);
        node = node->next;
    }
}

void print_hash(const CACHE *cache){
    NODE *tmp;
    for (int i = 0; i < CACHE_SIZE; ++i) {
        tmp = cache->hash[i];
        while (tmp != NULL) {
            printf("%d - %s ", i, cache->hash[i]->key);
            printf("%s", cache->hash[i]->value);
            tmp = tmp->next;
        }
    }
}

void update_position(CACHE **cache, const char *value){
    int hash_val = calculate_hash(value);
    NODE *node = (*cache)->hash[hash_val];
    while(node != NULL) {
        if(strcmp(node->key, value) == 0) {
            if (node == (*cache)->tail && node == (*cache)->head) {
                return;
            } else if (node != (*cache)->head && node != (*cache)->tail) {
                node->prev->next = node->next;
                node->next->prev = node->prev;
                (*cache)->head->prev = node;
                node->next = (*cache)->head;
                node->prev = NULL;
                (*cache)->head = node;
                return;
            } else if (node == (*cache)->tail) {
                node->prev->next = NULL;
                (*cache)->tail = node->prev;
                (*cache)->head->prev = node;
                node->next = (*cache)->head;
                node->prev = NULL;
                (*cache)->head = node;
                return;
            }
        }
        node = node->next;
    }
}

char* get(CACHE *cache, const char *key) {
    int hash_val = calculate_hash(key);
    NODE *node = cache->hash[hash_val];
    //printf("\n== %s\n", node->value);
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            if (node != cache->head) {
                cache->tail = (node == cache->tail) ? node->prev : cache->tail;
                node->prev->next = node->next;
                node->next ? node->next->prev = node->prev : 0;
                node->next = cache->head;
                node->prev = NULL;
                cache->head->prev = node;
                cache->head = node;
            }
            return node->value;
        }
        node = node->next;
    }
    return NULL;
}
