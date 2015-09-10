/* 
 * File:   cloud.c
 * Author: mkardasi
 *
 * Created on September 2, 2015, 12:08 PM
 *
 * Functions for uploading data to IoT cloud servers.
 */

#include "cloudAPI.h"

/*
 * Function registers a Node(s) with the server. A Node must be registered before
 * any data is uploaded.
 */
int RegisterNode(FILE* stream, size_t* streamSize, char* Url, char* apiKey)
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
	curl_slist_free_all(HeaderList);
	curl_easy_cleanup(handle);
	
	return(SUCCESS);
}

/*
 * Generates the HTTP string to register nNodes & nSensors
 */
int BuildRegString(size_t nNode, size_t nSensor)
{
	int i, j;
	
	fprintf(stdout, "[{"
					"\"mac\":\"MK-Node0\","
						"\"sensors\": [{"
							"\"name\":\"V1\","
							"\"type\":\"Volts\"},"
							"{\"name\":\"A1\","
							"\"type\":\"Amps\""
							"}]"
					"}]");
	printf("\n");
	
	struct Node MyNode[2];
	
	MyNode[0].Mac = "MK-Node0";
	MyNode[0].Sensor[0].Name = "V1";
	MyNode[0].Sensor[0].Type = "Volts";
	MyNode[0].Sensor[1].Name = "A1";
	MyNode[0].Sensor[1].Type = "Amps";
	
	MyNode[1].Mac = "MK-Node1";
	MyNode[1].Sensor[0].Name = "V2";
	MyNode[1].Sensor[0].Type = "Volts";
	MyNode[1].Sensor[1].Name = "A2";
	MyNode[1].Sensor[1].Type = "Amps";
	
	putchar('[');
	for(i = 0; i < nNode; i++)
	{
		if(i!=0) putchar(',');
		printf("{\"mac\": \"%s\",\"sensors\": [", MyNode[i].Mac);
		for(j = 0; j < nSensor; j++)
		{
			if(j!=0) putchar(',');
			printf("{\"name\": \"%s\",\"type\": \"%s\"}",
					MyNode[i].Sensor[j].Name, MyNode[i].Sensor[j].Type);
		}
		printf("]");
	}
	printf("}]");
		
	return 0;
}

int main(void)
{
	int funcRTN = -1;
	char* host = "https://api-iot.analoggarage.com/api/nodes";
	char* Key = "apiKey:witchcraft";
	char* Buf;
	size_t BufSize;
	FILE* stream;
	
	//Initialize libcurl
	curl_global_init(CURL_GLOBAL_SSL);
	
	//funcRTN = BuildRegString(2,2);
	
	/*
	* Create a stream to a dynamic mem buffer & write our registration 
	* string. Point libcurl to stream. Set number of bytes in POST request
	* otherwise server will expect chunked transfer.
	*/
	stream = open_memstream(&Buf, &BufSize);

	// manually creating the string for test purposes.
	fprintf(stream, "[{"
						"\"mac\":\"MK-Node0\","
							"\"sensors\":[{"
								"\"name\":\"coffee\","
								"\"type\":\"porkypig\"}]"
					"}]");
	fflush(stream);
	
	//Registers Node/Sensors with Cloud server.
	funcRTN = RegisterNode(stream, &BufSize, host, Key);
	
	//DebugF("RegisterNode:", &funcRTN);
	
	//Free resources acq'd by libcurl.		
	// Clean-up stream & buffer.
	fclose(stream);
	free(Buf);	
	curl_global_cleanup();

	return (EXIT_SUCCESS);
}