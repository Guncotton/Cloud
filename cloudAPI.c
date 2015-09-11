/* 
 * File:   cloud.c
 * Author: mkardasi
 *
 * Created on September 2, 2015, 12:08 PM
 *
 * Functions for uploading data to IoT cloud servers. These functions are 
 * dependant on cURL libraries (libcurl).
 */

#include "cloudAPI.h"

/*
 * Function registers a Node(s) with the server. A Node must be registered before
 * any data is uploaded.
 */
void XferToSrver(FILE* stream, size_t* streamSize, char* Url, char* apiKey)
{
	CURL *handle;
	CURLcode CurlRtn = 0;
	struct curl_slist* HeaderList;
		
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
		/*
		 * For Debug only.
		 */
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
		curl_easy_setopt(handle, CURLOPT_READDATA, (void*)stream);
		curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, (long)*streamSize);
			
		// Perform transfer.
		curl_easy_perform(handle);
		
		if(CurlRtn != CURLE_OK)
			fprintf(stderr, "Error:Xfer Failed: %s\n", curl_easy_strerror(CurlRtn));
	}
	else {
		fprintf(stderr, "Error:Handle Null\n");
	}
	
	// Clean-up allocated resources.
	//curl_slist_free_all(HeaderList);
	curl_easy_cleanup(handle);
}

/*
 * Generates the HTTP string to register nNodes & nSensors
 */
void BuildHTTPStr(FILE* target, void* source, size_t nNode, size_t nSensor, char Mode)
{
	int i, j;
	struct Node* buffer = (struct Node*)source;
	
	//target = stdout;
	
	rewind(target);
	
	// Registration string.
	if(Mode == 0)
	{
		putc('[', target);
		for(i = 0; i < nNode; i++)
		{
			if(i!=0) putc(',', target);
			fprintf(target, "{\"mac\":\"%s\",\"sensors\":[", buffer[i].Mac);
			for(j = 0; j < nSensor; j++)
			{
				if(j!=0) putc(',', target);
				fprintf(target, "{\"name\":\"%s\",\"type\":\"%s\"}",
						buffer[i].Sensor[j].Name, buffer[i].Sensor[j].Type);
			}
			fprintf(target, "]");
		}
		fprintf(target, "}]");

		fflush(target);
	}
	else // data string.
	{
		putc('[', target);
		for(i = 0; i < nNode; i++)
		{
			if(i!=0) putc(',', target);
			fprintf(target, "{\"mac\":\"%s\",\"sensors\":[", buffer[i].Mac);
			for(j = 0; j < nSensor; j++)
			{
				if(j!=0) putc(',', target);
				fprintf(target, "{\"name\":\"%s\",\"value\":\"%s\"}",
						buffer[i].Sensor[j].Name, buffer[i].Sensor[j].Value);
			}
			fprintf(target, "]");
		}
		fprintf(target, "}]");

		fflush(target);
	}
}

int main(void)
{
	int funcRTN = -1;
	char* Host = "https://api-iot.analoggarage.com/api/nodes";
	char* Key = "apiKey:witchcraft";
	char* Buf;
	size_t BufSize;
	FILE* stream;
	struct Node SomeNode[2];
	
	//Initialize libcurl.
	curl_global_init(CURL_GLOBAL_SSL);
	
	// Test config.
	SomeNode[0].Mac = "MK-Node0";
	SomeNode[0].Sensor[0].Name = "V1";
	SomeNode[0].Sensor[0].Type = "Voltage";
	SomeNode[0].Sensor[0].Value = "120";
	SomeNode[0].Sensor[1].Name = "A1";
	SomeNode[0].Sensor[1].Type = "Current";
	SomeNode[0].Sensor[1].Value = "12";
	
	SomeNode[1].Mac = "MK-Node1";
	SomeNode[1].Sensor[0].Name = "V2";
	SomeNode[1].Sensor[0].Type = "Voltage";
	SomeNode[1].Sensor[0].Value = "240";
	SomeNode[1].Sensor[1].Name = "A2";
	SomeNode[1].Sensor[1].Type = "Current";
	SomeNode[1].Sensor[1].Value = "6";
	/*
	* Create a stream to a dynamic mem buffer & write our registration 
	* string. Point libcurl to stream.
	*/
	stream = open_memstream(&Buf, &BufSize);
	BuildHTTPStr(stream, &SomeNode, 1, 2, REGISTR);
	fclose(stream);
	XferToSrver(stream, &BufSize, Host, Key);
	free(Buffer);
	
	// We would sample some data here....
	
	stream = opem_memstream(&Buffer, &BufSize);
	BuildHTTPStr(stream, &SomeNode, 1, 2, DATA);
	fclose(stream);
	XferToSrver(stream, &BufSize, Host, Key);
	free(Buffer);
	
	//Free resources acq'd by libcurl.		
	curl_global_cleanup();
	return (EXIT_SUCCESS);
}
