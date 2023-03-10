#ifndef SD_CARD_CLI_H
#define SD_CARD_CLI_H

#define SD_MIN_ARG_REQ 3


void sd_card_help(void);

void sd_card_cmd(int argc, char **argv, void *additional_arg);

#endif