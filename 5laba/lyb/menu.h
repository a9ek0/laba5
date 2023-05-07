#ifndef INC_5LABA_MENU_H
#define INC_5LABA_MENU_H
#include "cache.h"
#include "structures.h"
#include <string.h>
#include <stdarg.h>
void menu(CACHE *cache, const char *file_name);
void domain_to_ip(CACHE *cache, const char *file_name);
void ip_to_domains(FILE *file, const char *ip);
void print_ip(const CACHE *cache);
int find_ip(const CACHE *cache, const char *value);
void find_ip_domains(const CACHE *cache, const char *file_name);
void domain_to_ips(const char *filename, const char *domain);

void find_original(FILE *file, const char *domain, const char *ip, const char *print);
int is_in_file(const char *file_name, const char *ip);

void replace_char(char *line, char old_char, char new_char);

void free1(char *str1, char *str2, FILE *file);
#endif //INC_5LABA_MENU_H


