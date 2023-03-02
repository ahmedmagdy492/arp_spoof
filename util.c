#include "util.h"


void extract_ip(char* ip, int* formatted_ip_out) {
	int len = strlen(ip);
	char tempBuffer[4];
	tempBuffer[3] = '\0';
	int glob_counter = 0, tempCounter = 0;

	for(int i = 0;i < len; i++) {
		if(ip[i] == '.') {
			formatted_ip_out[glob_counter] = atoi(tempBuffer);
			glob_counter++;
			tempCounter = 0;
			memset(tempBuffer, 0, 4);
		}
		else {
			tempBuffer[tempCounter] = ip[i];
			tempCounter++;
		}
	}

	formatted_ip_out[glob_counter] = atoi(tempBuffer);
}


void extract_mac(char* mac, int* formatted_mac_out) {
	int len = strlen(mac);
	char tempBuffer[3];
	tempBuffer[2] = '\0';
	int glob_counter = 0, tempCounter = 0;

	for(int i = 0;i < len; i++) {
		if(mac[i] == ':') {
			formatted_mac_out[glob_counter] = (int)strtol(tempBuffer, NULL, 16);
			glob_counter++;
			tempCounter = 0;
			memset(tempBuffer, 0, 3);
		}
		else {
			tempBuffer[tempCounter] = mac[i];
			tempCounter++;
		}
	}

	formatted_mac_out[glob_counter] = (int)strtol(tempBuffer, NULL, 16);
}