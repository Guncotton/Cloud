/* 
 * File:   datastruc.h
 * Author: mkardasi
 *
 * Created on September 4, 2015, 5:39 PM
 */

#ifndef DATASTRUCTS_H
#define	DATASTRUCTS_H

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
#endif	/* DATASTRUCTS_H */