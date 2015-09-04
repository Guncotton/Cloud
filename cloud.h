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

/*
For testing only. Registers a Node with the following
 Mac: MK-Node0
		Name: coffee
		Type: temperature
 */
const char data[] = "[{\"mac\": \"MK-Node0\",\"sensors\": [{\"name\": \"coffee\",\"type\": \"temperature\"}]}]";

//Holds the data to be sent
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
    Payload->ReadPtr++;                 /* advance pointer */
    Payload->Size--;                /* less data left */
    return 1;                        /* we return 1 byte at a time! */
  }
  return 0;                          /* no more data left to deliver */
}

int RegisterNode(char *Url, char *apiKey)
{
	CURL *handle;
	CURLcode CurlRtn = 0;
	struct data config;
	struct curl_slist *HeaderList;
	struct SendThis DataSet;
	
	//For debugging.
	config.trace_ascii = 1;
	
	DataSet.ReadPtr = data;
	DataSet.Size = (long)strlen(data);
	
	//printf("Sizeof DataSet: %u\n", Sizeof(
	//Clear console.
	//printf("\033[2J");
	
	//Returns cURL's handle.
	handle = curl_easy_init();
	
	HeaderList = NULL;
	HeaderList = curl_slist_append(HeaderList, apiKey);
	HeaderList = curl_slist_append(HeaderList, "Content-Type:application/json");
	
	if(HeaderList == NULL){
		fprintf(stderr, "Error: Header List Null\n");
		return(FAILURE);
	}
	
	if (handle){	
		// For Debug only.
		curl_easy_setopt(handle, CURLOPT_DEBUGFUNCTION, debug_data);
		curl_easy_setopt(handle, CURLOPT_DEBUGDATA, &config);
		curl_easy_setopt(handle, CURLOPT_VERBOSE, TRUE);
		
		//Pass host address.
		curl_easy_setopt(handle, CURLOPT_URL, Url);
		
		//
		curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PUT");
		
		//Display header is stdout. Not needed in production or when debug.
		//curl_easy_setopt(handle, CURLOPT_HEADER, TRUE);
		
		//Config to send HTTP POST
		curl_easy_setopt(handle, CURLOPT_POST, TRUE);
		
		//Bytes for server to expect. 
		curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, DataSet.Size);
		
		//Pass custom headers to include.
		curl_easy_setopt(handle, CURLOPT_HTTPHEADER, HeaderList);
		
		//Function pointer, is called when lib needs data to send.
		curl_easy_setopt(handle, CURLOPT_READFUNCTION, Send_CallBack);
		
		//Config pointer to our data.
		curl_easy_setopt(handle, CURLOPT_READDATA, &DataSet);
		
		
		curl_easy_perform(handle); //Start xfer.
		
		if(CurlRtn != CURLE_OK)
			fprintf(stderr, "Xfer Failed: %s\n", curl_easy_strerror(CurlRtn));
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
