/*
 * user_espconn.c
 *
 *  Created on: 07-Jun-2021
 *      Author: harsh
 */

#include "user_espconn.h"

//system includes
#include "osapi.h"
#include "user_interface.h"
#include "mem.h"
#include "espconn.h"

//user includes
#include "user_webpage.h"
#include "user_wifi.h"

//driver libs
#include "driver/http.h"

#define ASSERT_N_SKIP(var, condition, label)	if(var != condition) goto label

//Set-Up Debugging Macros
#ifndef ESP_ESPCONN_LOGGER
	#define ESPCONN_DEBUG(message)					do {} while(0)
	#define ESPCONN_DEBUG_ARGS(message, args...)	do {} while(0)
#else
	#define ESPCONN_DEBUG(message)					do {os_printf("[ESPCONN-DEBUG] %s", message); os_printf("\r\n");} while(0)
	#define ESPCONN_DEBUG_ARGS(message, args...)	do {os_printf("[ESPCONN-DEBUG] "); os_printf(message, args); os_printf("\r\n");} while(0)
#endif

//static placeholders
static os_event_t taskQueue[1] = {0};
static bool RunServer = false;
static struct espconn espconn;

/******** Function Definitions ********/

/***************************************************************************************
 * FunctionName	:  _processHttpData
 * Description	:  process received HTTP data
 * Parameters	:  arg -- espconn obj
 * 				   pdata -- received data
 * 				   len -- received data length
 * 				   iMsgType -- HTTP message type
 **************************************************************************************/
void ICACHE_FLASH_ATTR _processHttpData(void *arg, char *pdata, unsigned short len, HTTP_MESSAGE_TYPE iMsgType){
	bool ret = false;
	struct espconn *pesp_conn = arg;

	if(iMsgType == HTTP_REQUEST){
		ESPCONN_DEBUG("It is HTTP Request");

		//initialize HTTP obj
		HTTP_REQUEST_PACKET httpRequest;
		httpRequest.routePath = NULL;
		httpRequest.routeLength = -1;
		httpRequest.data = NULL;
		httpRequest.dataLength = -1;

		//process http request
		ret = processHttpRequest(pdata, len, &httpRequest);
		ASSERT_N_SKIP(ret, true, SKIP_PROCESS);

		if(httpRequest.httpMethod == HTTP_GET && httpRequest.routeLength > 0){
			ESPCONN_DEBUG("HTTP request type : GET");

			#ifdef ESP_ESPCONN_LOGGER
				//print route
				char *routePath = (char*) os_zalloc(httpRequest.routeLength);
				os_memcpy(routePath, httpRequest.routePath, httpRequest.routeLength);
				ESPCONN_DEBUG_ARGS("HTTP Route path : %s", routePath);
				os_free(routePath);
			#endif

			//send response based on route
			HTTP_RESPONSE_PACKET responsePacket;
			responsePacket.connection = Closed;

			if(os_strncmp(httpRequest.routePath, "/", httpRequest.routeLength) == 0){
				responsePacket.httpStatusCode = HTTP_OK;
				responsePacket.content = GetWifi_AP_HTML();
				responsePacket.contentLength = os_strlen(responsePacket.content);
				responsePacket.contentType = text_html;
			}
			else if(os_strncmp(httpRequest.routePath, "styles.css", httpRequest.routeLength) == 0){
				responsePacket.httpStatusCode = HTTP_OK;
				responsePacket.content = GetWifi_AP_CSS();
				responsePacket.contentLength = os_strlen(responsePacket.content);
				responsePacket.contentType = text_css;
			}
			else if(os_strncmp(httpRequest.routePath, "script.js", httpRequest.routeLength) == 0){
				responsePacket.httpStatusCode = HTTP_OK;
				responsePacket.content = GetWifi_AP_JS();
				responsePacket.contentLength = os_strlen(responsePacket.content);
				responsePacket.contentType = application_javascript;
			}
			else {
				responsePacket.httpStatusCode = HTTP_Not_Found;
				responsePacket.content = "";
				responsePacket.contentLength = 0;
				responsePacket.contentType = text_html;
			}

			ret = sendHttpResponse(pesp_conn, &responsePacket);
			ESPCONN_DEBUG_ARGS("HTTP response send : %d", ret);
		}
		else if(httpRequest.httpMethod == HTTP_POST && httpRequest.routeLength > 0){
			ESPCONN_DEBUG("HTTP request type : POST");

			#ifdef ESP_ESPCONN_LOGGER
				//print route
				char *routePath = (char*) os_zalloc(httpRequest.routeLength);
				os_memcpy(routePath, httpRequest.routePath, httpRequest.routeLength);
				ESPCONN_DEBUG_ARGS("HTTP Route path : %s", routePath);
				os_free(routePath);
			#endif

			//send response based on route
			HTTP_RESPONSE_PACKET responsePacket;
			responsePacket.connection = Closed;
			responsePacket.contentType = text_html;

			if(os_strncmp(httpRequest.routePath, "/", httpRequest.routeLength) == 0){

				//send content to client

				responsePacket.httpStatusCode = HTTP_OK;
				responsePacket.content = "";
				responsePacket.contentLength = 0;

			}
			else {
				responsePacket.httpStatusCode = HTTP_Not_Found;
				responsePacket.content = "";
				responsePacket.contentLength = 0;
			}

			ret = sendHttpResponse(pesp_conn, &responsePacket);
			ESPCONN_DEBUG_ARGS("HTTP response send: %d", ret);

			if(httpRequest.data != NULL && httpRequest.dataLength > 0){
				ConnectToStation(httpRequest.data, httpRequest.dataLength);
			}
		}
	}
	else if(iMsgType == HTTP_RESPONSE){

	}

	SKIP_PROCESS:;
}

/***************************************************************************************
 * FunctionName	:  _webserver_recv
 * Description	:  Callback when data is received over TCP
 * Parameters	:  arg -- espconn obj
 * 				   pdata -- received data
 * 				   len -- received data length
 **************************************************************************************/
void ICACHE_FLASH_ATTR _webserver_recv (void *arg, char *pdata, unsigned short len){
	ESPCONN_DEBUG("Inside data receive callback");

	bool ret = false;
	struct espconn *pesp_conn = arg;
	if(pdata != NULL && len > 0)
		ESPCONN_DEBUG_ARGS("Data Received: %s", pdata);

	//********************* HTTP DATA HANDLING *********************//

	//check if received data is of type HTTP and it's Message type
	HTTP_MESSAGE_TYPE httpMsgType;
	ret = isHttp(pdata, len, &httpMsgType);
	if(ret){
		ESPCONN_DEBUG("It is HTTP data");
		_processHttpData(arg, pdata, len, httpMsgType);
	}
	//**************************************************************//
}

/***************************************************************************************
 * FunctionName	:  _webserver_sent
 * Description	:  Callback when data is sent over TCP.
 * Parameters	:  arg -- espconn obj
 **************************************************************************************/
void ICACHE_FLASH_ATTR _webserver_sent(void *arg){
	struct espconn *pesp_conn = arg;
	ESPCONN_DEBUG("webserver data sent");
}

/***************************************************************************************
 * FunctionName	:  _webserver_discon
 * Description	:  Callback when TCP connection is closed/disconnected.
 * Parameters	:  arg -- espconn obj
 **************************************************************************************/
void ICACHE_FLASH_ATTR _webserver_discon(void *arg){
    struct espconn *pesp_conn = arg;

    ESPCONN_DEBUG_ARGS("webserver's %d.%d.%d.%d:%d disconnect", pesp_conn->proto.tcp->remote_ip[0],
        		pesp_conn->proto.tcp->remote_ip[1],pesp_conn->proto.tcp->remote_ip[2],
        		pesp_conn->proto.tcp->remote_ip[3],pesp_conn->proto.tcp->remote_port);

    if(RunServer == false){
    	sint8 ret = false;
		ret = espconn_delete(&espconn);
    }
}

/***************************************************************************************
 * FunctionName	:  _deleteConn
 * Description	:  User task function callback for disconnecting TCP connection.
 * Parameters	:  event -- user task event
 **************************************************************************************/
void ICACHE_FLASH_ATTR _deleteConn(os_event_t *event){
	ESPCONN_DEBUG_ARGS("Inside _DeleteConn user task, event : %d", event->sig);

	sint8 ret = false;
	switch (event->sig) {
	case 0:
		ret = espconn_regist_disconcb(&espconn, _webserver_discon);
		ESPCONN_DEBUG_ARGS("register disconnect cb, ret : %d", ret);
		ret = espconn_disconnect(&espconn);
		ESPCONN_DEBUG_ARGS("disconnecting all TCP connections on server port: %d, ret : %d", espconn.proto.tcp->local_port, ret);
		break;
	default:
		break;
	}
}

/***************************************************************************************
 * FunctionName	:  _webserver_listen
 * Description	:  Callback when TCP connection is established. Used to Register
 * 				   other callbacks
 * Parameters	:  arg -- espconn obj
 **************************************************************************************/
void ICACHE_FLASH_ATTR _webserver_listen (void *arg){
	ESPCONN_DEBUG("Inside webserver_listen callback");

	sint8 ret = 0;
	struct espconn *pesp_conn = arg;

	if(RunServer == false){
		//Register user task to delete connection
		ret = system_os_task(_deleteConn, USER_TASK_PRIO_1,taskQueue, 1);
		ret = system_os_post(USER_TASK_PRIO_1, 0, 0);
	}
	else{
		//register callbacks
		ret = espconn_regist_recvcb(pesp_conn, _webserver_recv);
		ESPCONN_DEBUG_ARGS("register data receive callback, ret : %d", ret);
		ret = espconn_regist_sentcb(pesp_conn, _webserver_sent);
		ESPCONN_DEBUG_ARGS("register data sent callback, ret : %d", ret);
		ret = espconn_regist_disconcb(pesp_conn, _webserver_discon);
		ESPCONN_DEBUG_ARGS("register tcp dissconnect callback, ret : %d", ret);
	}
}

/***************************************************************************************
 * FunctionName	:  _webserver_recon
 * Description	:  Callback when reconnecting TCP connection
 * Parameters	:  arg -- espconn obj
 * 				   err -- disconnect error type
 **************************************************************************************/
void ICACHE_FLASH_ATTR _webserver_recon(void *arg, sint8 err){
	struct espconn *pesp_conn = arg;

	ESPCONN_DEBUG_ARGS("webserver's reconnect error : %d", err);
	ESPCONN_DEBUG_ARGS("webserver's %d.%d.%d.%d:%d disconnect", pesp_conn->proto.tcp->remote_ip[0],
	        		pesp_conn->proto.tcp->remote_ip[1],pesp_conn->proto.tcp->remote_ip[2],
	        		pesp_conn->proto.tcp->remote_ip[3],pesp_conn->proto.tcp->remote_port);

	_webserver_listen(arg);
}

/*******************************************************************************************
 * FunctionName	:  StopWebServer
 * Description	:  Stops the Web Server
 * Return		:  0 if successful, else failed
 ******************************************************************************************/
sint8 ICACHE_FLASH_ATTR StopWebServer(void){
	ESPCONN_DEBUG_ARGS("Stopping Web Server on port: %d", espconn.proto.tcp->local_port);
	RunServer = false;

	//Register user task to delete connection
	sint8 ret = false;
	ret = system_os_task(_deleteConn, USER_TASK_PRIO_1,taskQueue, 1);
	ret = system_os_post(USER_TASK_PRIO_1, 0, 0);
	return ret;
}

/*******************************************************************************************
 * FunctionName	:  StartWebServer
 * Description	:  Starts the Web Server
 * Return		:  0 if successful, else failed
 ******************************************************************************************/
sint8 ICACHE_FLASH_ATTR StartWebServer(void){
	ESPCONN_DEBUG_ARGS("Starting Web Server on port: %d", espconn.proto.tcp->local_port);
	sint8 ret = false;
	ret = espconn_accept(&espconn);
	RunServer = true;
	return ret;
}

/*******************************************************************************************
 * FunctionName	:  InitWebServer
 * Description	:  Initializes Web Server
 * Return		:  0 if successful, else failed
 ******************************************************************************************/
sint8 ICACHE_FLASH_ATTR InitWebServer(void){
	ESPCONN_DEBUG("Inside Init Web Server");

	static esp_tcp espTcp;

	espconn.type = ESPCONN_TCP;
	espconn.state = ESPCONN_NONE;
	espconn.proto.tcp = &espTcp;
	espconn.proto.tcp->local_port = TCP_LOCAL_PORT;

	sint8 ret = false;
	ret = espconn_regist_connectcb(&espconn, _webserver_listen);
	ESPCONN_DEBUG_ARGS("register server listen connect callback, ret : %d", ret);

	ret = espconn_regist_reconcb(&espconn, _webserver_recon);
	ESPCONN_DEBUG_ARGS("register tcp reconnect callback, ret : %d", ret);

	ret = espconn_accept(&espconn);
	ESPCONN_DEBUG_ARGS("create TCP server listening port: %d, ret : %d", espconn.proto.tcp->local_port, ret);

	RunServer = false;
	return ret;
}
