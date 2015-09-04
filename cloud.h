#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <curl/curl.h>

#define TRUE	1
#define FALSE	0

const char data[] = "[{\"mac\": \"MK-Node0\",\"sensors\": [{\"name\": \"coffee\",\"type\": \"temperature\"}]}]";

struct data {
  char trace_ascii;
};

struct WriteThis {
  const char *readptr;
  long sizeleft;
};

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
  struct WriteThis *pooh = (struct WriteThis *)userp;

  if(size*nmemb < 1)
    return 0;

  if(pooh->sizeleft) {
    *(char *)ptr = pooh->readptr[0]; /* copy one single byte */
    pooh->readptr++;                 /* advance pointer */
    pooh->sizeleft--;                /* less data left */
    return 1;                        /* we return 1 byte at a time! */
  }

  return 0;                          /* no more data left to deliver */
}

static void dump(const char *text, FILE *stream, unsigned char *ptr, size_t size, char nohex)
{
  size_t i;
  size_t c;

  unsigned int width=0x10;

  if(nohex)
    /* without the hex output, we can fit more on screen */
    width = 0x40;

  fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n",
          text, (long)size, (long)size);

  for(i=0; i<size; i+= width) {

    fprintf(stream, "%4.4lx: ", (long)i);

    if(!nohex) {
      /* hex not disabled, show it */
      for(c = 0; c < width; c++)
        if(i+c < size)
          fprintf(stream, "%02x ", ptr[i+c]);
        else
          fputs("   ", stream);
    }

    for(c = 0; (c < width) && (i+c < size); c++) {
      /* check for 0D0A; if found, skip past and start a new line of output */
      if (nohex && (i+c+1 < size) && ptr[i+c]==0x0D && ptr[i+c+1]==0x0A) {
        i+=(c+2-width);
        break;
      }
      fprintf(stream, "%c",
              (ptr[i+c]>=0x20) && (ptr[i+c]<0x80)?ptr[i+c]:'.');
      /* check again for 0D0A, to avoid an extra \n if it's at width */
      if (nohex && (i+c+2 < size) && ptr[i+c+1]==0x0D && ptr[i+c+2]==0x0A) {
        i+=(c+3-width);
        break;
      }
    }
    fputc('\n', stream); /* newline */
  }
  fflush(stream);
}

static int my_trace(CURL *handle, curl_infotype type, char *data, size_t size, void *userp)
{
  struct data *config = (struct data *)userp;
  const char *text;
  (void)handle; /* prevent compiler warning */

  switch (type) {
  case CURLINFO_TEXT:
    fprintf(stderr, "== Info: %s", data);
  default: /* in case a new one is introduced to shock us */
    return 0;

  case CURLINFO_HEADER_OUT:
    text = "=> Send header";
    break;
  case CURLINFO_DATA_OUT:
    text = "=> Send data";
    break;
  case CURLINFO_SSL_DATA_OUT:
    text = "=> Send SSL data";
    break;
  case CURLINFO_HEADER_IN:
    text = "<= Recv header";
    break;
  case CURLINFO_DATA_IN:
    text = "<= Recv data";
    break;
  case CURLINFO_SSL_DATA_IN:
    text = "<= Recv SSL data";
    break;
  }

  dump(text, stderr, (unsigned char *)data, size, config->trace_ascii);
  return 0;
}

int Init_cURL(void);
int CleanUp_cURL(void);
int RegNode(char *Url, char *apiKey);
int SendData(char *Url, char *apiKey);

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
		return (EXIT_FAILURE);
	}
	
	if (handle){	
		// For Debug.
		curl_easy_setopt(handle, CURLOPT_DEBUGFUNCTION, my_trace);
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
		return (EXIT_FAILURE);
	}
	
	//Resource cleanup.
	curl_slist_free_all(HeaderList);
	curl_easy_cleanup(handle);
	
	return (EXIT_SUCCESS);
};

int Init_cURL(void)
{
	curl_global_init(CURL_GLOBAL_SSL);
	
	return(EXIT_SUCCESS);
};

int CleanUp_cURL(void)
{
	curl_global_cleanup();
	
	return(EXIT_SUCCESS);
};

int SendtoCloud(char *Url, char *apiKey)
{
	
	return(EXIT_SUCCESS);
};
/*
int main(void){
      
    Test.Name = "Speed";
    Test.Type = "RPM";
        
    //Initialize array of Signal.
    for ( i = 0; i <8 ; i++)
        RasPi.Signal[i] = NULL;
        
    RasPi.Signal[0] = &Test;
    RasPi.Mac = "Milosz";
    
    printNode(RasPi);
    printf("##################################\n");
    
    RasPi.Signal[1] = createSensor("Current", "Amperage");
    
    printNode(RasPi);
    
    return 0;
};
*/