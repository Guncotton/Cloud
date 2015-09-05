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
  const char* ReadPtr;
  int Size;
};

static size_t Send_CallBack(void* Buffer, size_t Size, size_t nmemb, void* source)
{
  struct SendThis* Payload = (struct SendThis*)source;

  if(Size*nmemb < 1)
    return 0;

  if(Payload->Size) {
    *(char*)Buffer = Payload->ReadPtr[0];
    Payload->ReadPtr++; // Advance 1 byte.
    Payload->Size--; // 1 less byte left.
    return 1; // 1 byte at a time.
  }
  return 0; // 0 bytes left
}

int RegisterNode(char *Url, char *apiKey)
{
	CURL *handle;
	CURLcode CurlStatus = 0;
	struct curl_slist *HeaderList;
	struct SendThis DataSet;

	DataSet.ReadPtr = data;
	DataSet.Size = (long)strlen(data);
	
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
		
		//Enables callback func for outgoing data & passes it's ptr.
		//curl_easy_setopt(handle, CURLOPT_READFUNCTION, Send_CallBack);
		
		//Config pointer to our data.
		curl_easy_setopt(handle, CURLOPT_READDATA, &buffer);
		
		//Perform transfer.
		//curl_easy_perform(handle);
		
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