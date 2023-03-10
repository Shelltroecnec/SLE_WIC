#ifndef CONFIG_H
#define CONFIG_H


struct Config {

#undef INT
#undef STRING
#undef DOUBLE
#define INT(x, z) int x
#define STRING(x, y, z) char x[z]
#define DOUBLE(x, z) double x
#define STRARRAY(x, y, z, ...) char x[y][z]

#include "config_types.h"
};

typedef struct {
    char type, save, set;
    char key[64];
    void *ptr;
} assoc;

extern struct Config config;

void config_init(struct Config *config);
void config_load(char *filename, char setSave, char override);
void printConfig(void);
char *get_key_string(void *ptr);
void config_startup(void);
#endif