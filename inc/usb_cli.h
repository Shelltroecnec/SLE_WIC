#ifndef USB_CLI_H
#define USB_CLI_H

#define USB_MIN_ARG_REQ                 1
#define USB_MAX_ARG_REQ                 1


void usb_help(void);
void usb_cmd(int argc, char **argv, void *additional_arg);

#endif