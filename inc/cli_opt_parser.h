#ifndef CLI_OPT_PARSER_H
#define CLI_OPT_PARSER_H

/**
 * @file opt_parser.h
 * @author fakhruddin Khan (f.khan@acevin.com)
 * @brief 
 * @version 0.1
 * @date 2021-03-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#define MAX_COMMAND_SIZE 25

typedef struct {
    unsigned char isRegistered : 1;
    char argNumber;                             //expected argument list for this taks, in case less arguments provided this task will not be executed
    char *cmdName;                                  //pointer to command name
    char *description;                              //pointer to description of the command
    void (*print_help)(void);                       //function pointer to printing helper text for the specific command
    void (*task)(int argc, char **argv, void *arg); //Actual task that will be executed from cli
    void *arg;                                      //additional arguments required to call from the cli interface/function
} stCommandLine_t;

extern stCommandLine_t stCommandLine[MAX_COMMAND_SIZE];

extern int cli_register_cmd(char *cmdName, char *desc, char numberArg, void *helper, void *task, void *arg);
extern int cli_deregister_cmd(char *cmdName);
extern void print_usage_help(void);
extern int parse_cli(int argc, char **argv, int idx);
extern int opts_parser(int argc, char **argv);

#endif