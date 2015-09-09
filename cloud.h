/* 
 * File:   cloud.h
 * Author: mkardasi
 *
 * Created on September 2, 2015, 12:08 PM
 *
 * Library for uploading data to ADI IoT servers.
 */

//For program clarity. DO NOT CHANGE
#define TRUE	1
#define FALSE	0
#define SUCCESS 0
#define FAILURE -1

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <curl/curl.h>
#include "debug.h"

#ifndef CLOUD_H
#define CLOUD_H
/*
For testing only. Registers a Node with the following
Mac: MK-Node0
		Name: coffee
		Type: temperature
*/
const char data[] = "[{\"mac\": \"MK-Node0\",\"sensors\": [{\"name\": \"coffee\",\"type\": \"temperature\"}]}]";

struct SendThis {
  const char* String;
  int BytesRemaining;
};

/*
 * This is a callback function which is invoked when libcurl is performing a send.
 * Source is a ptr to the data that needs to be sent. Payload is a ptr to a block
 * who's contents will be sent when the function exits. Size is the size in bytes,
 * of the data type that is being sent. Blocks is the numbers of elements to send
 * ie. Source = "ABC" then Size = 1 byte & Blocks = 3.
 */

static size_t Send_CallBack(void* Payload, size_t Size, size_t Blocks, void* Source)
{
  struct SendThis* Buffer = (struct SendThis*)Source;

  if(Size*Blocks < 1)
    return 0;

  if(Payload->BytesRemaining) {
    *(char*)Payload = Buffer->String;
    //Buffer->ptrData++; // Advance 1 byte.
    Buffer->BytesRemaining =- strlen(Buffer->String);
    return strlen(Buffer->String);
  }
  return 0; // 0 bytes left
}

int BuildRegString(Input, Output, size_t Len)
{
    	printf("[{\"mac\": \"MK-Node0\",\"sensors\": [{\"name\": \"coffee\",\"type\": \"temperature\"}]}]\n");

    	char* Node = "MK-Node0";
    	char* SensName = "coffee";
    	char* Type = "temperature";
    
    	char* a = "[{\"mac\": \"";
	char* b = Node;
	char* c = "\",\"sensors\": [{\"name\": \"";
	char* d = SensName;
	char* e = "\",\"type\": \"";
	char* f = Type;
	char* g = "\"}]}]";
    
    	printf("%s%s%s%s%s%s%s", a,b,c,d,e,f,g);
        printf("\n");
        return 0;
}

int RegisterNode(char *Url, char *apiKey)
{
	CURL *handle;
	CURLcode CurlStatus = 0;
	struct curl_slist *HeaderList;
	struct SendThis DataSet;

	DataSet.ReadPtr = data;
	DataSet.Size = (int)strlen(data);
	
	//Returns cURL's handle.
	handle = curl_easy_init();
	
	//Setup custom headers for req'd by the cloud server.
	HeaderList = NULL;
	HeaderList = curl_slist_append(HeaderList, apiKey);
	HeaderList = curl_slist_append(HeaderList, "Content-Type:application/json");
	
	if(HeaderList == NULL){
		fprintf(stderr, "Error: Header List Null\n");
		return(FAILURE);
	}
	
	FILE buffer;
	
	fputs(DataSet.ReadPtr, &buffer);
	
	printf("Payload Size %i bytes\n", DataSet.Size);
	
	if (handle){	
		// For Debug only.
		curl_easy_setopt(handle, CURLOPT_DEBUGFUNCTION, debug_data);
		curl_easy_setopt(handle, CURLOPT_DEBUGDATA, (void*)"1");
		curl_easy_setopt(handle, CURLOPT_VERBOSE, TRUE);
		
		//Pass host address.
		curl_easy_setopt(handle, CURLOPT_URL, Url);
		
		/*
		 * The POST request requires a preceeding PUT. Do this
		 * by setting the option a custom command.
		*/
		curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PUT");
		
		//HTTP POST request.
		curl_easy_setopt(handle, CURLOPT_POST, TRUE);
		
		//Bytes for server to expect. 
		curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, DataSet.Size);
		
		//Pass custom headers to include.
		curl_easy_setopt(handle, CURLOPT_HTTPHEADER, HeaderList);
		
		//Function invoked when libcurl wants to send data.
		//curl_easy_setopt(handle, CURLOPT_READFUNCTION, Send_CallBack);
		
		//Set pointer to our data.
		curl_easy_setopt(handle, CURLOPT_READDATA, &buffer);
		
		//Perform transfer.
		curl_easy_perform(handle);
		
		if(CurlStatus != CURLE_OK)
			fprintf(stderr, "Xfer Failed: %s\n", curl_easy_strerror(CurlStatus));
	}
	else {
		fprintf(stderr, "Handle Null\n");
		return(FAILURE);
	}
	
	//Resource cleanup.
	curl_slist_free_all(HeaderList);
	curl_easy_cleanup(handle);
	
	return(SUCCESS);
}
#endif /* CLOUD_H */
