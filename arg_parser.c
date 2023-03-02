#include "arg_parser.h"

static params p_interface;

static params p_target_ip;

static params p_target_mac;

static params p_help;

static params* m_params[] =  {
    &p_interface,
    &p_target_ip,
    &p_target_mac
};

static void init() {
    p_interface.param_full_name = "--interface";
    p_interface.param_short_name = "-i";
    p_interface.param_desc = "the interface in which to use to send the packets";

    p_target_ip.param_full_name = "--target_ip";
    p_target_ip.param_short_name = "";
    p_target_ip.param_desc = "the ip in which the send the packets to";

    p_target_mac.param_full_name = "--target_mac";
    p_target_mac.param_short_name = "";
    p_target_mac.param_desc = "the mac address in which the send the query about";

    p_help.param_full_name = "--help";
    p_help.param_short_name = "-h";
    p_help.param_desc = "show the help";
}

static int are_params_valid(char* args[], int argc) {

    if(argc == 1) {
        printf("see %s -h or --help for more info about usage\n", args[0]);
        return 0;
    }    
    
    for(int i = 0; i < argc; i++) {
        if(strcmp(args[i], p_help.param_full_name) == 0) {
            printf("ARP Spoof Help Menu\n");
            printf("--------------------------------\n");

            printf("Usage: %s <param-name> [<argument-value>] \n\n", args[0]);

            for(int i = 0; i < PARAM_COUNT; i++) {
                printf("%s\t%s <argument>: %s\n", 
                m_params[i]->param_short_name,
                m_params[i]->param_full_name,
                m_params[i]->param_desc);
            }

            return 0;
        }

        if(strcmp(args[i], p_help.param_short_name) == 0) {
            printf("ARP Spoof Help Menu\n");
            printf("--------------------------------\n");

            printf("Usage: %s <param-name> [<argument-value>] \n\n", args[0]);

            for(int i = 0; i < PARAM_COUNT; i++) {
                printf("%s\t%s <argument>: %s\n", 
                m_params[i]->param_short_name,
                m_params[i]->param_full_name,
                m_params[i]->param_desc);
            }
            return 0;
        }

        for(int j = 0; j < PARAM_COUNT; j++) {
            if(strcmp(args[i], m_params[j]->param_full_name) == 0) {
                if(i < argc-1) {
                    if(args[i]) {
                        m_params[j]->value = args[i+1];
                    }
                    else {
                        printf("missing argument for parameter: %s\n", args[i]);
                        return 0;
                    }
                }
                else {
                    printf("missing argument for parameter: %s\n", args[i]);
                    return 0;
                }
            }

            if(strcmp(args[i], m_params[j]->param_short_name) == 0) {
                if(i < argc-1) {
                    if(args[i]) {
                        m_params[j]->value = args[i+1];
                    }
                    else {
                        printf("missing argument for parameter: %s\n", args[i]);
                        return 0;
                    }
                }
                else {
                    printf("missing argument for parameter: %s\n", args[i]);
                    return 0;
                }
            }
        }
    }

    return 1;
}

int parse_args(char* args[], int argc, char* interface, char* target_ip, char* target_mac) {
    init();
    if(are_params_valid(args, argc)) {
        printf("Welcome to ARP Spoofer v1.0\n");
        printf("------------------------------\n");

        strncpy(interface, p_interface.value, strlen(p_interface.value)+1);
        strncpy(target_ip, p_target_ip.value, strlen(p_target_ip.value)+1);
        strncpy(target_mac, p_target_mac.value, strlen(p_target_mac.value)+1);

        return 1;
    }
    else {
        return 0;
    }
}