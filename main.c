/* 
 * File:   main.c
 * Author: mkardasi
 *
 * Created on September 2, 2015, 12:08 PM
 */

#include <stdlib.h>
#include "cloud.h"

/*
 * 
 */

void OnExit(void)
{
	printf("Global Cleanup\n");
	CleanUp_cURL();
}

int main(void)
{
	char *host = "https://api-iot.analoggarage.com/api/nodes";
	char *Key = "apiKey:witchcraft";
	
	Init_cURL();
	
	char size = 0;
	size = sizeof("[{\"mac\": \"MK-Node0\",\"sensors\": [{\"name\": \"coffee\",\"type\": \"temperature\"}]}]");
	printf("Size is: %u\n", size);
	
	RegisterNode(host, Key);
	
	atexit(OnExit);
	return (EXIT_SUCCESS);
}


	

