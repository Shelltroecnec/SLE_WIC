/**
 * @file version.h
 * @author Fakhruddin Khan (f.khan@acevin.com)
 * @brief 
 * @version 0.1
 * @date 2021-08-11
 * 
 * @copyright Copyright (c) 2021
 *  
 */


#ifndef _VERSION_H
#define _VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "git_version.h"


#ifndef VERSION_MAJOR
#define VERSION_MAJOR           1
#endif

#ifndef VERSION_MINOR
#define VERSION_MINOR           0   
#endif

#ifndef VERSION_PATCH
#define VERSION_PATCH           0
#endif

#ifndef VERSION_COMMIT
#define VERSION_COMMIT "unknown"
#endif

typedef struct periphery_version {
    unsigned int major;
    unsigned int minor;
    unsigned int patch;
    const char *commit_id;
} periphery_version_t;

const char *periphery_version(void);
void print_version(void);
void print_version_logs(void);


const periphery_version_t *periphery_version_info(void);

#ifdef __cplusplus
}
#endif

#endif

