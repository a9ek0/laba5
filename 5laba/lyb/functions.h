#ifndef INC_5LABA_FUNCTIONS_H
#define INC_5LABA_FUNCTIONS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define CACHE_SIZE 10
#define MAX_DOMAIN_LENGTH 256
#define MAX_LINE_LENGTH 1024

typedef struct node {
    char *key;
    char *value;
    struct node *next;
    struct node *prev;
}NODE;

typedef struct cache {
    int size;
    NODE *head;
    NODE *tail;
    NODE **hash;
}CACHE;

void dell_oldest_from_cache(CACHE **cache);
void add_to_cache(CACHE *cache, const char *key, const char *value);
int calculate_hash(const char *key);
char *find_in_cache(const char *key, const CACHE *cache);
void print_cache(const CACHE *cache);
void print_hash(const CACHE *cache);
void update_position(CACHE **cache, const char *value);

char* get(CACHE *cache, const char *key);

void put(CACHE *cache, const char *key, const char *value);

void dell_tail(CACHE *cache);

int validate_ip_address(const char *ip_address);
void get_valid_ip_address(char **ip);
void check_in_range_int(int *value, int left_boarder, int right_boarder);
void dell_from_hash(CACHE *cache, const char *key);


void free_cache(CACHE *cache);
void free_node(NODE *node);
NODE* create_node(char *key, char *value);
CACHE *create_cache();

void menu(CACHE *cache, const char *file_name);
void domain_to_ip(CACHE *cache, const char *file_name);
void ip_to_domains(FILE *file, const char *ip);
void print_ip(const CACHE *cache);
int find_ip(CACHE *cache, const char *value);
void find_ip_domains(const CACHE *cache, const char *file_name);
void find_ip_domains1(const char *file_name, char *ip);
void domain_to_ips(const char *filename, const char *domain);

void find_original(FILE *file, const char *domain, const char *ip, const char *print);
int is_in_file(const char *file_name, const char *ip);

int check_input(const char *input, const char *str1, const char *str2);
void get_input(char *input, const char *str1, const char *str2);
int contains_indents(const char *str);
int contains_multiple_dots(const char *str);
void get_line(char **line);
char *add_extension(char *word, const char *extension);
void get_ip_and_domain(const CACHE *cache, const char *file_name, char **domain, char **ip);
char *get_ip_from_file(FILE *file, const char *key);

















void get_domains_associated_with_ip(const CACHE *cache, const char *value);
#endif //INC_5LABA_FUNCTIONS_H
