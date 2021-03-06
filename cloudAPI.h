/* 
 * File:   cloud.h
 * Author: mkardasi
 *
 * Created on September 2, 2015, 12:08 PM
 *
 * Library for uploading data to ADI IoT servers.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <curl/curl.h>
#include "debug.h"

#ifndef CLOUDAPI_H
#define CLOUDAPI_H

//For program clarity. DO NOT CHANGE
#define TRUE	1
#define FALSE	0
#define SUCCESS 0
#define FAILURE -1

struct Signal 
{
	char* Name;
	char* Type;
	char* Value;
};

struct Node
{
	char* Mac;
	struct Signal Sensor[8];
};
#endif /* CLOUDAPI_H */