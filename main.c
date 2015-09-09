/* 
 * File:   main.c
 * Author: mkardasi
 *
 * Created on September 2, 2015, 12:08 PM
 */

#include <stdlib.h>
#include "cloud.h"
#include "datastructs.h"

int main(void)
{
	int funcRTN = 0;
	char* host = "https://api-iot.analoggarage.com/api/nodes";
	char* Key = "apiKey:witchcraft";
	
	//Initialize libcurl
	curl_global_init(CURL_GLOBAL_SSL);
	
	//Registers Node/Sensors with Cloud server.
	funcRTN = RegisterNode(host, Key);
	DebugF("RegisterNode:", &funcRTN);
	
	//Free resources acq'd by libcurl.
	curl_global_cleanup();
	
	
	return (EXIT_SUCCESS);
}