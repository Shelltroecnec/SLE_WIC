// - vector implementation
// - config implementation based on vector of assocs
// - json parsing snd writer in c
#include "headers.h"

vector assocs;
struct Config config;

void AddAssoc(char type, char *key, void *pv) {
    assoc a = {type, 0, 0, {0}, pv};
    strcpy(a.key, key);
    assoc *ptr = (assoc *)malloc(sizeof(assoc));
    memcpy(ptr, &a, sizeof(assoc));
    // ILOG("%s: %lu %d\n",key,ptr, sizeof(assoc));
    vector_add(&assocs, ptr);
}

void config_init(struct Config *config) {
    vector_init(&assocs); //initialize the assocs vector to allocate the memory
#undef INT
#undef STRING
#undef DOUBLE
#undef STRARRAY

#define INT(x, z)                                                   \
    AddAssoc('i', #x, &config->x); /*ILOG("i: %s =  %d\n", #x, z)*/ \
    ;                                                               \
    config->x = z
#define STRING(x, y, z)                                            \
    AddAssoc('s', #x, config->x); /*ILOG("s: %s =  %s\n", #x, y)*/ \
    ;                                                              \
    strcpy(config->x, y)
#define DOUBLE(x, z)                                                 \
    AddAssoc('d', #x, &config->x); /*ILOG("d: %s =  %0.3lf\n", #x, z)*/ \
    ;                                                                \
    config->x = z

    // #define INT(x,z)       AddAssoc('i', #x, &x); x = z
    // #define STRING(x,z)    AddAssoc('s', #x, &x); x = z
    // #define DOUBLE(x,z)    AddAssoc('d', #x, &x); x = z
    // #define STRARRAY(x,...)  AddAssoc('a', #x, &x); { const char* vv[] = { __VA_ARGS__ }; for (uint32_t _i=0; _i < sizeof(vv) / sizeof(void*); _i++) x.push_back(vv[_i]); }

#include "config_types.h"
}

void printConfig() {

    int i = 0;
    for (i = 0; i < vector_total(&assocs); i++) {
        switch (((assoc *)assocs.items[i])->type) {
        case 'i':
            DLOG("Defult config: %s[i] = %d\n", (((assoc *)assocs.items[i])->key), *(int *)((assoc *)assocs.items[i])->ptr);
            break;
        case 's':
            DLOG("Defult config: %s[s] = %s\n", ((assoc *)assocs.items[i])->key, (char *)((assoc *)assocs.items[i])->ptr);
            break;
        case 'd':
            DLOG("Defult config: %s[f] = %0.3f\n", ((assoc *)assocs.items[i])->key, *(double *)((assoc *)assocs.items[i])->ptr);
            break;
        default:
            DLOG("Invalid type or structure\n");
        }
    }
}

char set_key_value(char *key, void *value, char save) {
    int i = 0;
    for (i = 0; i < vector_total(&assocs); i++) {
        if (!strcmp(((assoc *)assocs.items[i])->key, key)) { //in case both matches then only change the value of the stored value
            switch (((assoc *)assocs.items[i])->type) {
            case 'i':
                *(int *)((assoc *)assocs.items[i])->ptr = *(int *)value;
                ((assoc *)assocs.items[i])->save |= save;
                return 1;
            case 's':
                strcpy(((assoc *)assocs.items[i])->ptr, value);
                ((assoc *)assocs.items[i])->save |= save;
                return 1;
            case 'd':
                *(double *)((assoc *)assocs.items[i])->ptr = *(double *)value;
                ((assoc *)assocs.items[i])->save |= save;
                return 1;
            }
        }
    }
    return 0;
}

char *get_key_string(void *ptr) {
    int i = 0;
    for (i = 0; i < vector_total(&assocs); i++) {
        if (((assoc *)assocs.items[i])->ptr == ptr) {
            ILOG("found Key value pair of the given variable: %s\n", ((assoc *)assocs.items[i])->key);
            return ((assoc *)assocs.items[i])->key;
        }
    }
    return NULL;
}

void config_load(char *filename, char setSave, char override) {
    FILE *f = NULL;
    size_t rlen = 0, len = 0;
    char *data = NULL;
    int i = 0;
    if (!file_exists(filename)) {
        ELOG("[CFG] %s file does not exists\n", filename);
        return;
    }
    /* open in read binary mode */
    f = fopen(filename, "rb");
    /* get the length */
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);

    data = (char *)malloc(len + 1);

    rlen = fread(data, 1, len, f);
    data[rlen] = '\0';
    fclose(f);

    //doit(data);
    cJSON *tree = NULL;
    char *out = NULL;
    int idefaultVal = 0;
    char rgcDefaultVal[128] = {0};
    double fdefaultVal = 0.0;
    // char defval[100];
    tree = cJSON_Parse(data); //parsed the string parameter here so this can be used as the json object(structure)
    out = cJSON_Print((const cJSON *)tree);
    //ILOG("%s:\n%s\n",filename, out);
    free(out);

    for (i = 0; i < vector_total(&assocs); i++) {
        if ((override == 1 || ((assoc *)assocs.items[i])->save == 0) &&
            cJSON_HasObjectItem(tree, ((assoc *)assocs.items[i])->key)) { //in case both matches then only change the value of the stored value
            switch (((assoc *)assocs.items[i])->type) {
            case 'i':
                idefaultVal = *(int *)((assoc *)assocs.items[i])->ptr;
                *(int *)((assoc *)assocs.items[i])->ptr = (int)cJSON_GetNumberValue(cJSON_GetObjectItem(tree, ((assoc *)assocs.items[i])->key));
                // ILOG(" To: %d\n", *(int*)((assoc *)assocs.items[i])->ptr);
                DLOG("Defualt Config para: %s, Updated From: %d,  To: %d\n", ((assoc *)assocs.items[i])->key, idefaultVal, *(int *)((assoc *)assocs.items[i])->ptr);

                break;
            case 's':
                memset(rgcDefaultVal, 0, sizeof(rgcDefaultVal));
                strcpy(rgcDefaultVal, ((assoc *)assocs.items[i])->ptr);

                strcpy(((assoc *)assocs.items[i])->ptr, cJSON_GetStringValue(cJSON_GetObjectItem(tree, ((assoc *)assocs.items[i])->key)));
                // DLOG("To: %s\n",  ((assoc *)assocs.items[i])->ptr);
                DLOG("Defualt Config para: %s, Updated From: %s, To: %s\n", ((assoc *)assocs.items[i])->key, rgcDefaultVal, (char *)((assoc *)assocs.items[i])->ptr);

                break;
            case 'd':
                fdefaultVal = *(double *)((assoc *)assocs.items[i])->ptr;
                // DLOG("Defualt Config parameter %s, Updated From: %0.3lf,", ((assoc *)assocs.items[i])->key,  *(double*)((assoc *)assocs.items[i])->ptr);

                *(double *)((assoc *)assocs.items[i])->ptr = (double)cJSON_GetNumberValue(cJSON_GetObjectItem(tree, ((assoc *)assocs.items[i])->key));
                // DLOG("To: %0.3lf\n", *(double*)((assoc *)assocs.items[i])->ptr);
                DLOG("Defualt Config para: %s, Updated From: %0.3lf,  To: %0.3lf\n", ((assoc *)assocs.items[i])->key, fdefaultVal, *(double *)((assoc *)assocs.items[i])->ptr);

                break;
            default:
                DLOG("Invalid type in assocs\n");
            }
            ((assoc *)assocs.items[i])->save = setSave;
            ((assoc *)assocs.items[i])->set = 1;
        }
    }
    cJSON_Delete(tree);
}

/**
 * @brief this is the main function to test the config feature file only
 * 
 */
// int main(void) {
//     int i;
//
//     double d = 90.9;
//     config_init(&config);

//     // ILOG("om_server_port value: %d\n",config.om_server_port);
//     // ILOG("mac_addr value: %s\n",config.mac_addr);
//     // ILOG("gateway_id value: %s\n",config.gateway_id);
//     // ILOG("om_server_authorization value: %s\n",config.om_server_authorization);
//     // ILOG("hw_mon_supply_voltage_threshold_low_shutdown value: %.03f\n",config.hw_mon_supply_voltage_threshold_low_shutdown);
//     ILOG("Number of assoc: %d\n",assocs.total);
//     // for(i = 0; i < assocCnt; i++){
//     //     ILOG("assocs[%d]: %c %d %d %s ",i, assocs[i].type, assocs[i].save, assocs[i].set, assocs[i].key);
//     //     if(assocs[i].type == 'i'){
//     //         ILOG("%d\n",*(int *)assocs[i].ptr);

//     //     } else if(assocs[i].type == 'd'){
//     //         ILOG(" %0.3lf\n",*(double *)assocs[i].ptr);
//     //         *(double *)assocs[i].ptr = 8.0;
//     //     } else if(assocs[i].type == 's'){
//     //         ILOG("%s\n",assocs[i].ptr);
//     //         *(double *)assocs[i].ptr = 8.0;
//     //     }

//     // }

//     // for(i = 0; i < assocCnt; i++){

//     // }
//     // ILOG("config value: %.03f\n",config.hw_mon_supply_voltage_threshold_low_shutdown);

//     // set_key_value("mac_addr", "70:76:7F:00:12:12",1);
//     // ILOG("mac_addr: %s\n",config.mac_addr);
//     // ILOG("total size: %ld\n",sizeof(assocs.items[0]));
//     // ILOG("total: %d\n",assocs.total);
//     // ILOG("capacity: %d\n",assocs.capacity);
//     printConfig();
//     // int port = 50000;
//     // vector_add(&assocs,&port);

//     // set_key_value("om_server_port", &port,0);
//     // vector_delete(&assocs,6,FREE_DYNAMIC_MEMORY);
//     // ILOG("AFter setting\n");

//     // printConfig();
//     // ILOG("int in assoc: %d\n",*(int*)assocs.items[5]);

//     // vector_delete(&assocs,5,FREE_STATIC_MEMORY);

//     // ILOG("AFter setting\n");
//     config_load("./test.json",0,0);
//     printConfig();
//     // ILOG("int in assoc: %d\n",*(int*)assocs.items[5]);

//     return 0;
// }

void config_startup(){
    config_init(&config); //initialize the configuration first
    printConfig();
    DLOG("Config init done.....\r\n");

    config_load("./config.json", 0, 0);
}
