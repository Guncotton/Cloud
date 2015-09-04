#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <curl/curl.h>
#include "debug.h"

#define TRUE	1
#define FALSE	0

const char data[] = "[{\"mac\": \"MK-Node0\",\"sensors\": [{\"name\": \"coffee\",\"type\": \"temperature\"}]}]";

struct WriteThis {
  const char* readptr;
  int sizeleft;
};

int RegNode(char *Url, char *apiKey);

static size_t read_callback(void* buffer, size_t size, size_t nmemb, void* srcstream)
{
  struct WriteThis* payload = (struct WriteThis*)srcstream;

  if(size*nmemb < 1)
    return 0;

  if(payload->sizeleft) {
    *(char*)buffer = payload->readptr[0];
    payload->readptr++;                 /* advance pointer */
    payload->sizeleft--;                /* less data left */
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
	struct WriteThis DataSet;
	
	//For debugging.
	config.trace_ascii = 1;
	
	DataSet.readptr = data;
	DataSet.sizeleft = (long)strlen(data);
	
	//Clear console.
	//printf("\033[2J");
	
	//Returns cURL's handle.
	handle = curl_easy_init();
	
	HeaderList = NULL;
	HeaderList = curl_slist_append(HeaderList, apiKey);
	HeaderList = curl_slist_append(HeaderList, "Content-Type:application/json");
	
	if(HeaderList == NULL){
		fprintf(stderr, "Error: Header List Null\n");
		return -1;
	}
	
	if (handle){	
		// For Debug.
		curl_easy_setopt(handle, CURLOPT_DEBUGFUNCTION, debug_trace);
		curl_easy_setopt(handle, CURLOPT_DEBUGDATA, &config);
		curl_easy_setopt(handle, CURLOPT_VERBOSE, 1);
				
		curl_easy_setopt(handle, CURLOPT_URL, Url);
		curl_easy_setopt(handle, CURLOPT_HEADER, 1);
		curl_easy_setopt(handle, CURLOPT_POST, 1);
		//curl_easy_setopt(handle, CURLOPT_POSTFIELDS, "[{\"mac\": \"MK-Node0\",\"sensors\": [{\"name\": \"coffee\",\"type\": \"temperature\"}]}]");
		curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, 75);
		curl_easy_setopt(handle, CURLOPT_HTTPHEADER, HeaderList);
		curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(handle, CURLOPT_READFUNCTION, read_callback);
		curl_easy_setopt(handle, CURLOPT_READDATA, &DataSet);
		
		curl_easy_perform(handle); //Start xfer.
		
		if(CurlRtn != CURLE_OK)
			fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(CurlRtn));
	}
	else {
		fprintf(stderr, "Error: Handle Null\n");
		return -1;
	}
	
	//Resource cleanup.
	curl_slist_free_all(HeaderList);
	curl_easy_cleanup(handle);
	
	return 0;
};