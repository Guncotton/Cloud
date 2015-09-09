/* 
 * File:   cloud.c
 * Author: mkardasi
 *
 * Created on September 2, 2015, 12:08 PM
 *
 * Functions for uploading data to ADI IoT servers.
 */

#include "cloudAPI.h"

/*
 * Functions registers a Node with the server. A Node must be registered before
 * any data is uploaded.
 */
int RegisterNode(char *Url, char *apiKey)
{
	CURL *handle;
	CURLcode CurlRtn = 0;
	struct curl_slist* HeaderList;
	char* Buf;
	size_t BufSize;
	FILE* stream;
		
	//Return libcurl handle.
	handle = curl_easy_init();
	
	//Setup custom headers req'd by the cloud server.
	HeaderList = NULL;
	HeaderList = curl_slist_append(HeaderList, apiKey);
	HeaderList = curl_slist_append(HeaderList, "Content-Type:application/json");
	
	if(HeaderList == NULL){
		fprintf(stderr, "Error:Header List Null\n");
	}
	else if (handle){	
		// For Debug only.
		curl_easy_setopt(handle, CURLOPT_DEBUGFUNCTION, debug_data);
		curl_easy_setopt(handle, CURLOPT_DEBUGDATA, (void*)"1");
		curl_easy_setopt(handle, CURLOPT_VERBOSE, TRUE);
		
		/*
		 * Set URL. Set PUT to preceed the main HTTP request. Set POST as the
		 * HTTP request. Set HTTP request to include custom headers.
		 */
		curl_easy_setopt(handle, CURLOPT_URL, Url);
		curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(handle, CURLOPT_POST, TRUE);
		curl_easy_setopt(handle, CURLOPT_HTTPHEADER, HeaderList);
		
		/*
		* Create a stream to a dynamic mem buffer & write our registration 
		* string. Point libcurl to stream. Set number of bytes in POST request
		* otherwise server will expect chunked transfer.
		*/
		stream = open_memstream(&Buf, &BufSize);
		fprintf(stream, "[{\"mac\": \"MK-Node0\",\"sensors\": "
				"[{\"name\": \"coffee\",\"type\": \"porkypig\"}]}]");
		fflush(stream);
		
		curl_easy_setopt(handle, CURLOPT_READDATA, (void*)stream);
		curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, (long)BufSize);
			
		// Perform transfer.
		curl_easy_perform(handle);
		
		// Clean-up stream & buffer.
		fclose(stream);
		free(Buf);
		
		if(CurlRtn != CURLE_OK)
			fprintf(stderr, "Error:Xfer Failed: %s\n", curl_easy_strerror(CurlRtn));
	}
	else {
		fprintf(stderr, "Error:Handle Null\n");
	}
	
	// Clean-up allocated resources.
	curl_slist_free_all(HeaderList);
	curl_easy_cleanup(handle);
	
	return(SUCCESS);
}

	int main(void)
{
	int funcRTN;
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