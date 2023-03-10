

#ifndef RLY_CLI_H
#define RLY_CLI_H

#define RLY_MIN_ARG_REQ                 3
#define RLY_MAX_ARG_REQ                 3

#define RLY_SINGLE_OP_MODE              0
#define RLY_INDVIDUAL_BULK_OP_MODE      0xFE
#define RLY_BULK_OP_MODE                0xFF


#define RELAY_OFF_STATE(x)                ((x == 0)? 1: 0)
#define RELAY_ON_STATE(x)                 ((x == 0)? 0: 1)
#define RELAY_STATE_STR(x, s)             (((x == 0)? ((s == 1)? "OFF": "ON") : ((s == 1)? "ON": "OFF")))


#define NUM_MAX_RLY                     7


#define RELAY_STRING(x)                 ((x == 1)? "SSR 1   ": \
                                         (x == 2)? "SSR 2   ": \
                                         (x == 3)? "SSR 3   ": \
                                         (x == 4)? "SSR 4   ": \
                                         (x == 5)? "SSR 5   ": \
                                         (x == 6)? "SSR 6   ": \
                                         (x == 0)? "24V DCR ": "24V DCR "\
                                        )

#define SELECT_RELAY_IO(x)              ((x == 1)? config.ssr_cathode_1: \
                                         (x == 2)? config.ssr_cathode_2: \
                                         (x == 3)? config.ssr_cathode_3: \
                                         (x == 4)? config.ssr_cathode_4: \
                                         (x == 5)? config.ssr_cathode_5: \
                                         (x == 6)? config.ssr_cathode_6: \
                                         (x == 0)? config.ssr_24V_dry_contact_relay: config.ssr_24V_dry_contact_relay\
                                        )
void relay_cmd(int argc, char **argv, void *additional_arg);
void relay_help(void);
int32_t relay_operation(int32_t , int32_t , int32_t);
void test_Function(void);

#endif
