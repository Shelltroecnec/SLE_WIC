

#include "headers.h"

const char *periphery_version(void) {
    #define _STRINGIFY(s) #s
    #define STRINGIFY(s) _STRINGIFY(s)
    return "v" STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." STRINGIFY(VERSION_PATCH);
}

const periphery_version_t *periphery_version_info(void) {
    static const periphery_version_t version = {
        .major = VERSION_MAJOR,
        .minor = VERSION_MINOR,
        .patch = VERSION_PATCH,
        .commit_id = VERSION_COMMIT,
    };
    return &version;
}

void print_version(void){
    const periphery_version_t *version_t = periphery_version_info();
    CLOG("%s version %d.%d.%d, build %s\r\n", exec_filename, version_t->major, version_t->minor, version_t->patch, version_t->commit_id);
}

void print_version_logs(void){
    const periphery_version_t *version_t = periphery_version_info();
    CLOG("Changes made in version %d.%d.%d: Updated cli parser and added config option\n",version_t->major, version_t->minor, version_t->patch);
}