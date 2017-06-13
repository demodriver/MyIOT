#ifndef _REMOT_DEF_H
#define _REMOT_DEF_H

//POST /devices/3017351/datapoints?type=3 HTTP/1.1
//Host:api.heclouds.com
//api-key:CGYhwTphnl6MDWgXuW1=vHyGpTc=
//Content-Length:20

//{"temperture":56.00}
#define devices1 "3017351"


char * URL_DevCreate = "/devices/%s/datapoints?type=3";

char * HttpHead_Format = 
"%s %s HTTP/1.1\r\n"            //fill with request method & URL
"Host: %s\r\n"                  //fill with host
"Accept: */*\r\n"
"U-ApiKey: %s\r\n"              //fill with ApiKey
"Content-Length: %d\r\n"        //fill with buf length
"Connection: close\r\n"
"\r\n";

char * HttpBuf_Format = 
"%s %s HTTP/1.1\r\n"            //fill with request method & URL
"Host: %s\r\n"                  //fill with host
"api-key:%s\r\n"              //fill with ApiKey
"Content-Length: %d\r\n"        //fill with buf length
//"Connection: close\r\n"
"\r\n"
"%s";                           //fill with http msg body

/*device info json define*/
//char * DevInfo_JsonFormat = 
//"{"
//	"\"title\":\"%s\","
//	"\"about\":\"%s\","
//	"\"tags\":[\"temp_humi\",\"lab\"],"
//	"\"location\":"
//	"{"
//		"\"local\":\"Shanghai\","
//		"\"latitude\":31.40,"
//		"\"longitude\":121.24"
//	"}"
//"}\r\n";

/*sensor info json define*/
//char * SensorInfo_JsonFormat = 
//"{"
//	"\"type\":\"value\","
//	"\"title\":\"%s\","
//	"\"about\":\"%s\","
//	"\"tags\":[\"tag1\",\"tag2\"],"
//	"\"unit\":"
//	"{"
//		"\"name\": \"%s\","
//		"\"symbol\": \"%s\""
//	"}"
//"}\r\n";

char * DataPoints_JsonFormat = 
"{\"temperture\":%d,\"humidity\":%d}\r\n";


#endif

