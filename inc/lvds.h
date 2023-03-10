#ifndef LVDS_H
#define LVDS_H


/* I2C Device*/

#define LVDS_MIN_ARG_REQ                 1
#define LVDS_MAX_ARG_REQ                 1

void LVDS_show_img(void);
void lvds_help(void);
void lvds_cmd(int argc, char **argv, void *additional_arg);
#endif