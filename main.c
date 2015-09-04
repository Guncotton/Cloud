/* 
 * File:   main.c
 * Author: mkardasi
 *
 * Created on September 2, 2015, 12:08 PM
 */

#include <stdlib.h>
#include "cloud.h"

int main(void)
{
	char *host = "https://api-iot.analoggarage.com/api/nodes";
	char *Key = "apiKey:witchcraft";
	
	curl_global_init(CURL_GLOBAL_SSL);
	RegisterNode(host, Key);
	curl_global_cleanup();
	
	return (EXIT_SUCCESS);
}