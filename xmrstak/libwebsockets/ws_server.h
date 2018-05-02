#pragma once

#include <cstddef>
#include <cstdarg>

namespace LWS {
#include <libwebsockets.h>
}

//#include <list>
//#include <mutex>
//#include <thread>
//#include <map>
//#include <stdio.h>
//#include <time.h>



class WSServer {
   public:
      WSServer();
      ~WSServer();

      static WSServer* ws_self;
   private:
      		// Holds pointer to arbitrary data, and length of data. Safe way of handling what we need to send. ~F
		//---struct WS_Message {
		//---	size_t size;
		//---	char* data;
		//---};

		// Websocket Connection Struct, to hold everything related to a single connection
		//---struct Connection
		//---{
		//---	std::list<WS_Message>       buffer;     // Ordered list of pending messages to flush out when socket is writable
		//---	std::list<WS_Message>       bin_buffer;     // Ordered list of pending messages to flush out when socket is writable
		//---	std::map<std::string, std::string> keyValueMap;
			//time_t createTime;

		//---	std::string origin = "";
		//---	int socket_id = -1;
		//---};

		// Map of socket_ids to connection structs.
		//---std::map<int, Connection> connections;
		//---std::recursive_mutex _connections_mutex;

		//---std::recursive_mutex _wsi_mutex;

		//---struct LWS::lws_context *context;
		//---struct LWS::lws_vhost *vhost;
		//---std::thread* main_loop_thr;
		//--bool run_main_loop = true;

		// Core internals
		//---friend int callback_http(LWS::lws * wsi, LWS::lws_callback_reasons reason, void * user, void * in, size_t len); // Super friendly handler function!
		//--void New_Connection_Handler(Connection new_c);
		//---void Event_Loop();

};