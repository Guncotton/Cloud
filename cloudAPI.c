/* 
 * File:   cloudAPI.c
 * Author: mkardasi
 *
 * Created on September 2, 2015, 12:08 PM
 *
 * Functions for uploading data to IoT cloud servers. These functions are 
 * dependant on cURL libraries (libcurl).
 */

#include "cloudAPI.h"

int CreatePayloadBuf(FILE** fp, char*** buf, size_t** bufsize)
{
	*fp = open_memstream(*buf, *bufsize);
	if(fp == 0) 
	{
		ErrorF("NULL stream");
		return (FAILURE);
	}
	return (SUCCESS);
}
void DestroyPayloadBuf(FILE* fp, char* buf)
{
	if(fp != NULL)
	{
		fclose(fp);
		free(buf);
	}
}
/*
 * Function communicates with the Cloud server. Two modes are supported:
 * -Mode defines request type. 0 = registration, 0 = data upload. 
 * -stream is a pointer to a memory block of type FILE which contains the 
 *  registration string.
 * -streamSize is the size of stream.
 * -Url points to a string which contains the host HTTPS address.
 * -apiKey points to a string which contains the server passcode.
 */
void SendToSrver(char Mode, void* stream, size_t* streamSize, char* Url, char* apiKey)
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
	
	if(HeaderList == NULL) ErrorF("Error:Header List Null");
	else if (handle){
		//For Debug only.
		curl_easy_setopt(handle, CURLOPT_DEBUGFUNCTION, debug_data);
		curl_easy_setopt(handle, CURLOPT_DEBUGDATA, (void*)"1");
		curl_easy_setopt(handle, CURLOPT_VERBOSE, TRUE);
		
		/*
		 * Configure libcurl options depending on type of transfer. Note
		 * CURLOPT_POST is a data encoding setting not an HTTP POST cmd.
		 */
		curl_easy_setopt(handle, CURLOPT_URL, Url);
		if(Mode == 0) curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PUT");
		else curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(handle, CURLOPT_POST, TRUE);
		curl_easy_setopt(handle, CURLOPT_HTTPHEADER, HeaderList);	
		curl_easy_setopt(handle, CURLOPT_READDATA, stream);
		curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, (long)*streamSize);
			
		// Perform transfer.
		curl_easy_perform(handle);

		if(CurlRtn != CURLE_OK)
			fprintf(stderr, "Error:Xfer Failed: %s\n", curl_easy_strerror(CurlRtn));
	}
	else ErrorF("Error:Handle Null");
	
	// Clean-up allocated resources.
	curl_slist_free_all(HeaderList);
	curl_easy_cleanup(handle);
}

/*
 * Function creates a string which is the payload to be sent to the server. Func
 * will create a Node(s) registration string or sensor data string.
 * -Mode: value of 0 generates registration payload, value of 1 generates data payload 
 * -target: points to a type FILE stream to which the payload string will be written.
 * -source: points to a structure of type Node, which contains the Node/Sensor info.
 * -nNode: is number of nodes configured.
 * -nSensor: is the number of configured sensors
 */
void CreateHTTPStr(char Mode, FILE* target, void* source, size_t nNode, size_t nSensor)
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
				fprintf(target, "{\"name\":\"%s\",\"value\":%s}",
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
	char* Key = "apiKey:ithinkthereforeiam";
	char* Buffer = NULL;
	size_t BufSize;
	FILE* stream = NULL;
	struct Node SomeNode[2];
	
	//memset(&SomeNode, '0', sizeof(SomeNode)); 
	
	//Initialize libcurl.
	curl_global_init(CURL_GLOBAL_SSL);
	
	// Test config.
	SomeNode[0].Mac = "MK-Node0";
	SomeNode[0].Sensor[0].Name = "V1";
	SomeNode[0].Sensor[0].Type = "Voltage";
	SomeNode[0].Sensor[0].Value = "5000";
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
	* Create dynamic mem buffer for data to server. Create
	* either registration or data payload. Send the data to
	* the server. Lastly, cleanup resources used by payload
	* buffer.
	*/	
	//stream = open_memstream(&Buffer, &BufSize);
	if(CreatePayloadBuf(&stream, *Buffer, *BufSize) == SUCCESS) 
	{
		CreateHTTPStr(REGISTR, stream, &SomeNode, 1, 2);
		//SendToSrver(REGISTR, stream, &BufSize, Host, Key);
		DestroyPayloadBuf(stream, Buffer);
	}
	
	//We would sample some data here....
	//struct Node* ptrNode = SomeNode;
	
	srand(time(NULL));
	char* new = "1234";
	
	size_t From = strlen(new);
	size_t To = strlen(SomeNode[0].Sensor[0].Value);
	
	printf("len new:%u\tlen Value:%u\nsize From: %u\tsize To:%u\n", From, To, sizeof(new), sizeof(SomeNode[0].Sensor[0].Value));
	
	
	stream = open_memstream(&Buffer, &BufSize);
	if(stream == NULL)
	{
		ErrorF("NULL stream");
	}
	else
	{
		CreateHTTPStr(DATA, stream, &SomeNode, 1, 2);
		//SendToSrver(DATA, stream, &BufSize, Host, Key);
		DestroyPayloadBuf(stream, Buffer);
	}
	//Free resources acq'd by libcurl.		
	curl_global_cleanup();
	return (EXIT_SUCCESS);
}
