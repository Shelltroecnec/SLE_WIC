#include "headers.h"
#include "pwm.h"

/**
 * @brief PWM helper function
 * 
 */
void pwm_help(void) {
    CLOG("PWM command Usage:\n");
    CLOG("   %s -c PWM [CHIP] [CH] \n", exec_filename);
    CLOG("      CHIP - Select PWM Chip i.e between 0 to 3\n");
    CLOG("      CH - Select PWM channels, i.e between 0 to 6\n");
}

/**
 * @brief 				Updare pwm value
 * 
 * @param pwm 			structure to pwm handler
 * @param frequency 	PWM frequency
 * @param duty_cycle 	Set duty cycle
 * @return int 			0 - success, -1 - error
 */
int update_pwm_value(pwm_t *pwm, double frequency, double duty_cycle ){

    // pwm_t *pwm;

    // pwm = pwm_new();
	/* Open PWM chip 0, channel 10 */
    if (pwm_open(pwm, pwm->chip, pwm->channel) < 0) {
        fprintf(stderr, "pwm_open(): %s\n", pwm_errmsg(pwm));
        return -1; //exit(1);
    }

    /* Set frequency to 1 kHz */
    if (pwm_set_frequency(pwm, 1e3) < 0) {
        fprintf(stderr, "pwm_set_frequency(): %s\n", pwm_errmsg(pwm));
        return -1; // exit(1);
    }

    /* Set duty cycle to 75% */
    if (pwm_set_duty_cycle(pwm, duty_cycle) < 0) {
        fprintf(stderr, "pwm_set_duty_cycle(): %s\n", pwm_errmsg(pwm));
        return -1;//exit(1);
    }
 
    /* Enable PWM */
    if (pwm_enable(pwm) < 0) {
        fprintf(stderr, "pwm_enable(): %s\n", pwm_errmsg(pwm));
        return -1;//exit(1);
    }

    // pwm_close(pwm);

    // pwm_free(pwm);

    return 0;

}

/**
 * @brief Helper text for different options
 * 
 */
void show_command_option(void){
	ILOG("List of commands:\n");
	ILOG("   s - Start/ON Motor command\n");
	ILOG("   p - Stop/Off Motor command\n");
	ILOG("   r - Change Motor direction\n");
	ILOG("   d - Disconnect command\n");
	ILOG("   b - Break command\n");
	ILOG("   q - Emergency Suspend command\n");
	ILOG("   e - Edit parameter command\n");
	ILOG("Select a command: ");
	fflush(stdout);
}

/**
 * @brief 		callback to PWM function
 * 
 * @param ptr 	character pointer NULL exit, else parse character accordingly
 * @param args 	pointer to a function
 * @return int 	0 - success, non 0 on error
 */
int pwm_periodic_cb(char *ptr, void *args) {

    if (ptr == NULL) {
		//Take the action here for the last command

		return 0; //do nothing when no command entered
    }
	/**
	 * s - Start/ON Motor command
	 * p - Stop/Off Motor command
	 * r - Change Motor direction
	 * d - Disconnect command
	 * b - Break command
	 * q - Emergency Suspend command
	 */ 
	if(ptr[0] == 's' || ptr[0] == 'S'){
		//stop motor and exit from the motor process
		CLOG("Start/ON motor command\n");
	} else if(ptr[0] == 'p' || ptr[0] == 'P'){
		//stop motor and exit from the motor process
		CLOG("Stop/Off motor command\n");
	} else if(ptr[0] == 'r' || ptr[0] == 'R'){
		//stop motor and exit from the motor process
		CLOG("Change motor direction command\n");
	} else if(ptr[0] == 'b' || ptr[0] == 'B'){
		//stop motor and exit from the motor process
		CLOG("Break command\n");
	} else if(ptr[0] == 'q' || ptr[0] == 'Q'){
		//stop motor and exit from the motor process
		CLOG("Emergency Stopping motor.....\n");
	} else {
		CLOG("Invalid command: %s\n",ptr);
	}

	show_command_option();
	
    return 0;

}


void time_handler1(size_t timer_id, void * user_data)
{
    ILOG("Single shot timer expired.(%lu)\n", timer_id);
}

/**
 * @brief 					PWM cli command
 * 
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program. 
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name) 
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void pwm_cmd(int argc, char **argv, void *additional_arg) {
	
    char user_input[128];
	unsigned int chip = 0, channel = 0;
	char ch = 0, ch1 = 0;

    struct timeval tv;
	pwm_t *pwm = NULL;
    
	pwm = pwm_new();

    if (argc < PWM_MIN_ARG_REQ) {
        // CLOG("[AI] Less number of argument/s provided for AI Cmd!!\n");
        pwm_help();
        return;
    }

    //parsing the inputs
    chip = atoi(argv[1]);
    channel = atoi(argv[2]);

    CLOG("Selected PWM chip : %d, channel : %d\n", chip, channel);

    if ((chip > 4) || (chip < 0)) {
        CLOG("Invalid chip number!!!\n");
        pwm_help();
        return;
    }

	if ((channel > 6) || (channel < 0)) {
        CLOG("Invalid pwm channels!!!\n");
        pwm_help();
        return;
    }

	CLOG("Default frequency is %d Hz. Would you like to modify it? [Y/n] ", config.pwm_frequency);
	ch = getchar();
	while ((ch != '\n') && ((ch1 = getchar()) != '\n') && ch1 != (char)EOF);
	int frequency = config.pwm_frequency;   
	if ((ch == 'Y') || (ch == 'y')) {
		CLOG("Enter frequency(in Hz):");
		
		scanf("%d", &frequency);
		while ((ch1 = getchar()) != '\n' && ch1 != (char)EOF);

	}
	config.pwm_frequency = frequency;
	CLOG("PWM frequency: %d Hz\n", config.pwm_frequency);
	//-------------------

	CLOG("Default Ramp period is %d seconds. Would you like to modify it? [Y/n] ", config.ramp_period_sec);
	ch = getchar();
	while ((ch != '\n') && ((ch1 = getchar()) != '\n') && ch1 != (char)EOF);
	int ramp_period_sec = config.ramp_period_sec;   
	if ((ch == 'Y') || (ch == 'y')) {
		CLOG("Enter the Ramp Period(in seconds):");
		
		scanf("%d", &ramp_period_sec);
		while ((ch1 = getchar()) != '\n' && ch1 != (char)EOF);

	}
	config.ramp_period_sec = ramp_period_sec;
	CLOG("Ramp period: %d\n", config.ramp_period_sec);
	//------------

	CLOG("Default Duty cycle step is %d %%. Would you like to modify it? [Y/n] ", config.pwm_duty_cycle_step);
	ch = getchar();
	while ((ch != '\n') && ((ch1 = getchar()) != '\n') && ch1 != (char)EOF);
	int pwm_duty_cycle_step = config.pwm_duty_cycle_step;   
	if ((ch == 'Y') || (ch == 'y')) {
		CLOG("Enter the Duty cycle step[Between 1-100 %%]:");
		
		scanf("%d", &pwm_duty_cycle_step);
		while ((ch1 = getchar()) != '\n' && ch1 != (char)EOF);

	}

	if(pwm_duty_cycle_step > 100 || pwm_duty_cycle_step < 0){
		pwm_duty_cycle_step = config.pwm_duty_cycle_step;   
	}

	config.pwm_duty_cycle_step = pwm_duty_cycle_step;
	CLOG("Duty cycle: %d %%\n", config.pwm_duty_cycle_step);
	//------------

	show_command_option();
	size_t timer1;
	timer1 = start_timer(500, time_handler1, TIMER_PERIODIC, NULL);

	tv.tv_sec = 2;
    tv.tv_usec = 0;
    check_stdin_cb(&tv, &pwm_periodic_cb, NULL);
   
   	stop_timer(timer1);

}
