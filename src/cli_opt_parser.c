/**
 * @file cli_opt_parser.c
 * @author Fakhruddin Khan (f.khan@acevin.com)
 * @brief 
 * @version 0.1
 * @date 2021-08-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#include "headers.h"

char *exec_filename = NULL;
stCommandLine_t stCommandLine[MAX_COMMAND_SIZE];
bool loop = false;
int iTotalRegisterCmd = 0; //currently registered commands count

/**
 * @brief            Register a command line task that will be executed by cli
 * 
 * @param cmdName    Name of command that will be used to run a specific task from command line      
 * @param desc       Command description which will be used to print in helper
 * @param numberArg  Number of arguments required for processing of this task. 
 *                   This is useful in case we would like to run multiple commands in single execution
 * @param helper     Pointer to helper function for specific task help print, it will be called in case
 *                   of invalid arg or for help  
 * @param task       Task or thread to be run when this command is executed from command line
 * @param arg        Optional parameter to be passed from the calling of the task
 * 
 * @return int       registered status; 0 - Failed, 1 - Success
 */
int cli_register_cmd(char *cmdName, char *desc, char numberArg, void *helper, void *task, void *arg) {
    int iLoop = 0;
    int isRegister = 0;
    for (iLoop = 0; iLoop < MAX_COMMAND_SIZE; iLoop++) {
        if (stCommandLine[iLoop].isRegistered == 0) { //check if any structure is available
            stCommandLine[iLoop].isRegistered = 1;
            stCommandLine[iLoop].print_help = helper;
            stCommandLine[iLoop].task = task;
            stCommandLine[iLoop].argNumber = numberArg; //4 bit only
            stCommandLine[iLoop].cmdName = cmdName;
            stCommandLine[iLoop].description = desc;
            isRegister = 1; //registration successfull
            iTotalRegisterCmd++;
            // CLOG("Register cmd: %s at stCommandLine[%d] args: %d\n", stCommandLine[iLoop].cmdName, iLoop, stCommandLine[iLoop].argNumber);
            break;
        }
    }
    if (isRegister == 0) {
        CLOG("Registration failed for %s command.\n", cmdName);
        if (iTotalRegisterCmd >= MAX_COMMAND_SIZE) {
            CLOG("Max command %d limit is reached!!! Number of task registered is %d\n", MAX_COMMAND_SIZE, iTotalRegisterCmd);
        }
    }
    // CLOG("Register commands[%d/%d]\n",iTotalRegisterCmd,MAX_COMMAND_SIZE);

    return isRegister;
}

int cli_deregister_cmd(char *cmdName) {
    int iLoop = 0;
    for (iLoop = 0; iLoop < MAX_COMMAND_SIZE; iLoop++) {
        if (stCommandLine[iLoop].cmdName && strcmp(cmdName, stCommandLine[iLoop].cmdName) == 0) {
            stCommandLine[iLoop].isRegistered = 0;
            iTotalRegisterCmd--;
            return 0; 
        }
    }
    //CLOG("Deregister: Max command limit: %d  current registered count: %d\n",MAX_COMMAND_SIZE,iTotalRegisterCmd);

    return -1;
}

void loop_func(void){
    DLOG("Inside Loop func\n");
    loop = true;
}
void print_usage_help(void) {
    int iLoop = 0;
    CLOG("Usage:  %s [OPTIONS] COMMAND\n", exec_filename);
    CLOG("Options:\n");
    // CLOG("  %*.*s%*.*s\n",-10,10,-10,10,"--cmd string", "Name of the command to be executed");
    CLOG("   %*.*s %s\n", -20, 20, "-c COMMAND", "CLI commands to be executed");
    CLOG("   %*.*s %s\n", -20, 20, "-l, --loop", "Run command in continous loop until break");
    CLOG("   %*.*s %s\n", -20, 20, "-h, --help", "To print the help");
    CLOG("   %*.*s %s\n", -20, 20, "-d Delay", "To add delay, it is useful to execute mulitple commands ");
    CLOG("   %*.*s %s\n", -20, 20, "-v, --version", "Print version information and quit");
    CLOG("   %*.*s %s\n", -20, 20, "-i, --info", "Print version information logs and quit");


    CLOG("Commands:\n");
    for (iLoop = 0; iLoop < MAX_COMMAND_SIZE; iLoop++) {
        // CLOG("isRegistered[%d]: %d\n",iLoop,stCommandLine[iLoop].isRegistered);
        if (stCommandLine[iLoop].isRegistered == 1) {
            CLOG("   %*.*s %s\n", -15, 15, stCommandLine[iLoop].cmdName, stCommandLine[iLoop].description);
        }
    }

    CLOG("Run '%s -c COMMAND' OR '%s --COMMAND' for more information on a command.\n", exec_filename, exec_filename);
}
int parse_cli(int argc, char **argv, int idx) {
    int iLoop = 0;
    int ret = 0;
    DLOG ("argc: %d idx: %d\n",argc,idx);
    DLOG ("argc: %s\n",argv[idx]);
    for (iLoop = 0; iLoop < iTotalRegisterCmd; iLoop++) {
        if (strcmp(argv[idx], stCommandLine[iLoop].cmdName) == 0) {
            DLOG("%s Command found\n", argv[idx]);

            if (argc - idx < stCommandLine[iLoop].argNumber) {
                CLOG("[CLI] parsing args, less number of args for %s command, required: %d, given: %d\n", stCommandLine[iLoop].cmdName, stCommandLine[iLoop].argNumber, argc - idx);
                (*stCommandLine[iLoop].print_help)();
                return 0;
            } else {
                //Got valid arguments for this command, execute the function
                ret = stCommandLine[iLoop].argNumber;
                (*stCommandLine[iLoop].task)(argc - idx, &argv[idx], &ret);
                // CLOG("Got valid arguments\n");
            }
            break;
        }
    }
    if(iLoop == iTotalRegisterCmd){
        CLOG("Invalid COMMAND: %s\n", argv[idx]);
        print_usage_help();
        return EXIT_SUCCESS;
    }
    return ret;
}

/**
 * @brief       This function parse the command line argument and call the respective CLI or help functions
 * 
 * @param argc  argument count
 * @param argv  double pointer to an array of options/string
 * @return int  return status, 0 - on success, -1 on failure
 */
int opts_parser(int argc, char **argv) {
    int retValue = 0;
    int i = 0, iLoop = 0;
    struct option long_opt[] = {
    
      {"loop",          no_argument,       NULL, 'l'},
      {"help",          no_argument,       NULL, 'h'},
      {"version",       no_argument,       NULL, 'v'},
      {"info",          no_argument,       NULL, 'i'},
      {"config",        required_argument, NULL, 's'},
      {NULL,            0,                 NULL, 0  }
    };
    /* Parse command line options */
    while ((i = getopt_long(argc, argv, "hc:C:-vVd:liIs:", long_opt, NULL)) != -1) {
        DLOG("%c: %d/%d - %s\n", i, optind, argc, argv[optind]);
        switch (i) {
        case 'l':
            CLOG("l option selected\n");
            DLOG("-l is detected: argv[%d]: %s\r\n", optind, argv[optind]);
            // loop = true;
            loop_func();
            break;
        case 'h':
            print_usage_help();
            //CLOG("-h is detected: argv[%d]: %s\r\n", optind, argv[optind]);
            return EXIT_SUCCESS;
            break;
        case 'd':
            DLOG("%ld\r\n", time(NULL));
            ILOG("delay is added [%d]: %s, %s\r\n", optind, argv[optind], optarg);
            usleep(atoi(optarg) * 1000);
            DLOG("%ld\r\n", time(NULL));

            break;
        case 'v':
        case 'V':
            // ILOG("print version: argv[%d]\r\n", optind);

            //print the version string for the cli tool
            print_version();
            return EXIT_SUCCESS;
            break;
        case 'i':
        case 'I':
        // Print info for changes made in this version.
        // To update add comments in version.c file function print_version_info
            print_version_logs();
            return EXIT_SUCCESS;
            break;
        case 's':
            CLOG("--config option selected\n");
            DLOG("Optiond[%d]: %s\n",optind - 1, argv[optind - 1]);
            config_load(argv[optind - 1], 0, 0);
            // optind++;
            DLOG("optindval = %d\n",optind);
            break;
        // case '-':
        //     //parse arguments for single command execution or for help
        //     if ((strcmp(argv[optind], "--help") == 0) && (argc >= 2)) { //print help for this command
        //         print_usage_help();
        //         DLOG("--help is detected: argv[%d]: %s\r\n", optind, argv[optind]);
        //         return EXIT_SUCCESS;
        //     } else if((strcmp(argv[optind], "--version") == 0) && (argc >= 2)){
        //         print_version();
        //         return EXIT_SUCCESS;
        //     } else if((strcmp(argv[optind], "--info") == 0) && (argc >= 2)){
        //         print_version_logs();
        //         return EXIT_SUCCESS;
        //     } else if ((strcmp(argv[optind], "--l") == 0) && (argc >=2)){
        //         CLOG("--loop is detected: argv[%d]: %s\r\n", optind, argv[optind]);
        //         optind++;
        //         // loop = true;
        //         loop_func();
        //         break;
        //     } else if ((strcmp(argv[optind], "--config") == 0) /*&& (optind+1 < argc)*/){
        //         // config_startup;
        //         CLOG("Optiond[%d]: %s\n",optind + 1, argv[optind + 1]);
        //         config_load(argv[optind+1], 0, 0);
        //         optind++;
        //         printf("optindval = %d\n",optind);
        //     } else {
        //         for (iLoop = 0; iLoop < iTotalRegisterCmd; iLoop++) {
        //             if (strcmp(stCommandLine[iLoop].cmdName,(char *)&argv[optind][2]) == 0) { //if the command help is provided then we can just print the help
        //                 ILOG("Help for the command: %s\n", stCommandLine[iLoop].cmdName);
        //                 (*stCommandLine[iLoop].print_help)();
        //                 return EXIT_SUCCESS;
        //                 break;
        //             }
        //         }
        //         if(iLoop == iTotalRegisterCmd){
        //            CLOG("Invalid COMMAND: %s\n", (char *)&argv[optind][2]);
        //            print_usage_help(); 
        //         }
        //     }
        //     // return EXIT_SUCCESS;
        //     break;
        
        case 'c': //commands for the cli interface
        case 'C':
            retValue = parse_cli(argc, argv, optind - 1); //here we get optind as 3 which will point to next ARG
            DLOG("argc: %d, ret: %d\r\n", argc, retValue);
            DLOG("-------%c: %d/%d - %s\n", i, optind, argc, argv[optind]);
            
            //number of arguments that are parsed by above commands
            if (retValue > 0) {
                optind += retValue - 1;
            }
            break;
        default:
            CLOG("ERROR: argument parsing options, use -h option for help, current argv[%d]: %s\n", optind, optarg);
            print_usage_help();
            return EXIT_FAILURE;
        }
    }

    if ((i == -1) && (argc == 2)) {
        for (iLoop = 0; iLoop < iTotalRegisterCmd; iLoop++) {
            if (strcmp(argv[1], stCommandLine[iLoop].cmdName) == 0) {
                CLOG("Usage for %s command:\n", stCommandLine[iLoop].cmdName);
                (*stCommandLine[iLoop].print_help)();
                return EXIT_FAILURE;
            }
        }
    } 
    
    if ((i == -1 && argc < 3)) {
        CLOG("No argument provided!!!!\r\n");
        //CLOG("No Agument provided current[%d]: %s\n", optind, argv[optind]);
        print_usage_help();
        return EXIT_FAILURE;
    }
    return 0;
}
