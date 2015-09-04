/* 
 * File:   main.c
 * Author: mkardasi
 *
 * Created on September 2, 2015, 12:08 PM
 */

#include <stdlib.h>
#include "cloud.h"
#include "datastructs.h"

struct Node RasPi;

int main(void)
{
	char *host = "https://api-iot.analoggarage.com/api/nodes";
	char *Key = "apiKey:witchcraft";
	
	//Initialize libcurl
	curl_global_init(CURL_GLOBAL_SSL);
	
	//RegisterNode(host, Key);
	
	//Free resources acq'd by libcurl.
	
	curl_global_cleanup();
	
	RasPi.Mac = "Pi";
	
	return (EXIT_SUCCESS);
}