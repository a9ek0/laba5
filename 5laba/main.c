#include "lyb/functions.h"

int main() {

    CACHE *cache;
    char file_name[16] = "domain_list.txt";

    cache = create_cache();

    menu(cache, file_name);

    free_cache(cache);
    return 0;
}
