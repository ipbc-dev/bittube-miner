#include "ws_server.h"

//#include <string>

#include <string.h>
#include <stdio.h>

//#include <vector>

namespace LWS {
   const int BUFFER_SIZE = (1024 * 300000);
   //std::string fragmentMSG;

   struct LWS::lws_protocols protocols[] = {
   /* The first protocol must always be the HTTP handler */
      {
         "http-only",		     /* name */
   //---      callback_http,  /* callback */
         0,					     /* No per session data. */
         (size_t)BUFFER_SIZE,	 /* max frame size / rx buffer */
      },
      { NULL, NULL, 0, 0 } /* terminator */
   };

   //---enum protocols {
   //---   PROTOCOL_HTTP = 0
   //---};

   //---static bool do_check_allowed_origins = true;

   //---std::vector<std::string> allowed_origins = {
   //---   "https://vviewer.com",
   //---   "https://local.v-viewer.com:23042",
   //--   "https://ipbc.io",
   //---   "https://www.ipbc.io",
   //---   "http://compiler.magic:8000",
   //---   "https://v-viewer.com",
   //---   "https://testing.ipbc.io",
   //---   "https://bit.tube"
   //---};
}


/*
 *
 */
WSServer::WSServer() {
  //---ws_self = this;
}

/*
 *
 */
WSServer::~WSServer() {

}