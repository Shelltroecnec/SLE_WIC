#ifndef TOOLS_H
#define TOOLS_H



int htoi(char *s);
// void check_stdin_cb(struct timeval *tv, int (*callback)(char *ptr));
void check_stdin_cb(struct timeval *ptv, int (*callback)(char *, void *), void *args);
float map(float value, float in_min, float in_max, float out_min, float out_max);
void byte_to_bits(char *data, char len, char *rgucbits);

bool file_exists (char *filename);
int exec_sys(const char *command, char *result);
int getRandom(int lower, int upper);

extern float avg_samples_float(float *temp_sample,uint8_t max_sample, float *, float *);
extern uint16_t avg_samples_integer(uint16_t *samples, uint16_t sample_cnt, uint16_t *, uint16_t *);


void print_byte_array(unsigned char *byte_array, int byte_array_size);
int convert_byte_array(const char *hex_str, unsigned char *byte_array, int byte_array_max);
char *open_shared_mem_obj(key_t key, char *data, size_t len, int mode);

#endif

