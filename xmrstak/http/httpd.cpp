/*
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  *
  * Additional permission under GNU GPL version 3 section 7
  *
  * If you modify this Program, or any covered work, by linking or combining
  * it with OpenSSL (or a modified version of that library), containing parts
  * covered by the terms of OpenSSL License and SSLeay License, the licensors
  * of this Program grant you additional permission to convey the resulting work.
  *
  */

#ifndef CONF_NO_HTTPD


#include "httpd.hpp"
#include "webdesign.hpp"
#include "xmrstak/net/msgstruct.hpp"
#include "xmrstak/misc/console.hpp"
#include "xmrstak/misc/executor.hpp"
#include "xmrstak/jconf.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

//#include <microhttpd.h>
#ifdef _WIN32
#define strcasecmp _stricmp
#endif // _WIN32

//----------------------------------------------------------------------------------------------------------
// POST additions start ---

struct post_response {
	bool isError = false;
	char* error = "Texto del error";

	bool isData = false;
	char* resData = "Datos en formato json";
};

const int POSTBUFFERSIZE = 512;
const int MAXNAMESIZE = 20;
const int MAXANSWERSIZE = 512;
const int GET = 0;
const int POST = 1;

const char* greatingpage="<html><body><h1>Welcome, %s!</center></h1></body></html>";

const char* errorpage="<html><body>This doesn't seem to be right.</body></html>";

const char* askpage = "<html><body>\n\
                       Upload a file, please!<br>\n\
                       There are %u clients uploading at the moment.<br>\n\
                       <form action=\"/filepost\" method=\"post\" \
                         enctype=\"multipart/form-data\">\n\
                       <input name=\"file\" type=\"file\">\n\
                       <input type=\"submit\" value=\" Send \"></form>\n\
                       </body></html>";


struct connection_info_struct {
  int connectiontype;
  char *answerstring;
  struct MHD_PostProcessor *postprocessor; 
};

int httpd::send_page (struct MHD_Connection *connection, const char *page) {
	int ret;
	struct MHD_Response *response;

	response = MHD_create_response_from_buffer (strlen (page), 
															  (void *) page,
															  MHD_RESPMEM_PERSISTENT);

  if (!response) {
    return MHD_NO;
  }

  ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response);

  return ret;
}

int httpd::iterate_post (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
								 const char *filename, const char *content_type,
								 const char *transfer_encoding, const char *data, 
								 uint64_t off, size_t size) {
	std::cout << "----[httpd::iterate_post(...)]" << std::endl;
	
	std::cout << "Reciving post 002a[iterate_post] - data size: " << size << std::endl;
	std::cout << "Data: " << data << std::endl; 

	struct connection_info_struct *con_info = (connection_info_struct*)coninfo_cls;

	if (strcmp (key, "name") == 0) {

		std::cout << "Reciving a name" << std::endl;

		if ((size > 0) && (size <= MAXNAMESIZE)) {
          char *answerstring;
          answerstring = (char*)malloc (MAXANSWERSIZE);

          if (!answerstring) {
				 return MHD_NO;
			 }
      
          snprintf (answerstring, MAXANSWERSIZE, greatingpage, data);
          con_info->answerstring = answerstring;      
      
		} else  {
			con_info->answerstring = NULL;
		}

		return MHD_NO;
	}

	
	con_info->answerstring = "Bueeeeeeeno esto es embarazoso...";
	return MHD_YES;
}

void httpd::request_completed (void *cls, 
										 struct MHD_Connection *connection, 
										 void **con_cls,
										 enum MHD_RequestTerminationCode toe) {
	
	std::cout << "----[httpd::request_completed(...)]" << std::endl;

	struct connection_info_struct *con_info = (connection_info_struct*)*con_cls;

	if (NULL == con_info) {
		return;	
	}

	if (con_info->connectiontype == POST) {
		MHD_destroy_post_processor (con_info->postprocessor);        

		if (con_info->answerstring) {
			free (con_info->answerstring);
		}
	}
  
	free (con_info);
	*con_cls = NULL;   
}

int httpd::starting_process_post (MHD_Connection* connection,
												const char* method,
												const char* upload_data,
												size_t* upload_data_size,
												void ** ptr) {
	std::cout << "[httpd::starting_process_post(...)]Receiving a post msg..."  << std::endl;

	if(NULL == *ptr) {
		struct connection_info_struct *con_info;

		con_info = (connection_info_struct*)malloc (sizeof (struct connection_info_struct));
				
		if (NULL == con_info) { 
			return MHD_NO;
		}
				
		con_info->answerstring = NULL;

		//---if (strcmp (method, "POST") == 0) {      
			con_info->postprocessor = MHD_create_post_processor (connection, POSTBUFFERSIZE, 
																						  iterate_post, (void*) con_info);   

			if (con_info->postprocessor == NULL) {
				free (con_info); 
				return MHD_NO;
			}

			con_info->connectiontype = POST;
		//---} else  {
		//---	con_info->connectiontype = GET;
		//---}

		*ptr = (void*) con_info; 
		return MHD_YES;
	}

	//---if (strcmp (method, "GET") == 0) {
	//---	return send_page (connection, askpage);     
	//---}

	//---if (strcmp (method, "POST") == 0) {
		struct connection_info_struct *con_info = (connection_info_struct*)*ptr;

		if (*upload_data_size != 0) {
			MHD_post_process (con_info->postprocessor, upload_data,	
									*upload_data_size);

			*upload_data_size = 0;

			return MHD_YES;
		} else if (NULL != con_info->answerstring) {
			return send_page (connection, con_info->answerstring);
		}
	//---}

	return send_page(connection, errorpage); 

}
// POST additions end ---
//----------------------------------------------------------------------------------------------------------


httpd* httpd::oInst = nullptr;

httpd::httpd() {}

int httpd::req_handler(void * cls,
							  MHD_Connection* connection,
							  const char* url,
							  const char* method,
							  const char* version,
							  const char* upload_data,
							  size_t* upload_data_size,
							  void ** ptr) {

	struct MHD_Response * rsp;
	std::cout << "Receiving a http request..."  << std::endl;

	int retValue;

	if (strcmp(method, "GET") != 0) {
		//--------------------------------------------------------------------------------------
		// POST additions 2 start ---
		if (strcmp(method, "POST") == 0) {
			retValue = starting_process_post(connection, method, upload_data, upload_data_size, ptr);

			return retValue;

		} else {
		// POST additions 2 end ---
		//--------------------------------------------------------------------------------------
			return MHD_NO;
		}
	}

	if(strlen(jconf::inst()->GetHttpUsername()) != 0) {
		char* username;
		int ret;

		username = MHD_digest_auth_get_username(connection);
		if (username == NULL) {

			rsp = MHD_create_response_from_buffer(sHtmlAccessDeniedSize, (void*)sHtmlAccessDenied, MHD_RESPMEM_PERSISTENT);
			ret = MHD_queue_auth_fail_response(connection, sHttpAuthRelam, sHttpAuthOpaque, rsp, MHD_NO);
			MHD_destroy_response(rsp);
			return ret;
		}
		free(username);

		ret = MHD_digest_auth_check(connection, sHttpAuthRelam, jconf::inst()->GetHttpUsername(), jconf::inst()->GetHttpPassword(), 300);
		if (ret == MHD_INVALID_NONCE || ret == MHD_NO)
		{
			rsp = MHD_create_response_from_buffer(sHtmlAccessDeniedSize, (void*)sHtmlAccessDenied, MHD_RESPMEM_PERSISTENT);
			ret = MHD_queue_auth_fail_response(connection, sHttpAuthRelam, sHttpAuthOpaque, rsp, (ret == MHD_INVALID_NONCE) ? MHD_YES : MHD_NO);
			MHD_destroy_response(rsp);
			return ret;
		}
	}

	*ptr = nullptr;
	std::string str;
	if(strcasecmp(url, "/style.css") == 0)
	{
		const char* req_etag = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "If-None-Match");

		if(req_etag != NULL && strcmp(req_etag, sHtmlCssEtag) == 0)
		{ //Cache hit
			rsp = MHD_create_response_from_buffer(0, nullptr, MHD_RESPMEM_PERSISTENT);

			int ret = MHD_queue_response(connection, MHD_HTTP_NOT_MODIFIED, rsp);
			MHD_destroy_response(rsp);
			return ret;
		}

		rsp = MHD_create_response_from_buffer(sHtmlCssSize, (void*)sHtmlCssFile, MHD_RESPMEM_PERSISTENT);
		MHD_add_response_header(rsp, "ETag", sHtmlCssEtag);
		MHD_add_response_header(rsp, "Content-Type", "text/css; charset=utf-8");
	}
	else if(strcasecmp(url, "/api.json") == 0)
	{
		executor::inst()->get_http_report(EV_HTML_JSON, str);

		rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "application/json; charset=utf-8");
	}
	else if(strcasecmp(url, "/h") == 0 || strcasecmp(url, "/hashrate") == 0)
	{
		executor::inst()->get_http_report(EV_HTML_HASHRATE, str);

		rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "text/html; charset=utf-8");
	}
	else if(strcasecmp(url, "/c") == 0 || strcasecmp(url, "/connection") == 0)
	{
		executor::inst()->get_http_report(EV_HTML_CONNSTAT, str);

		rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "text/html; charset=utf-8");
	}
	else if(strcasecmp(url, "/r") == 0 || strcasecmp(url, "/results") == 0)
	{
		executor::inst()->get_http_report(EV_HTML_RESULTS, str);

		rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "text/html; charset=utf-8");
	}
	else
	{
		//Do a 302 redirect to /h
		char loc_path[256];
		const char* host_val = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "Host");

		if(host_val != nullptr)
			snprintf(loc_path, sizeof(loc_path), "http://%s/h", host_val);
		else
			snprintf(loc_path, sizeof(loc_path), "/h");

		rsp = MHD_create_response_from_buffer(0, nullptr, MHD_RESPMEM_PERSISTENT);
		int ret = MHD_queue_response(connection, MHD_HTTP_TEMPORARY_REDIRECT, rsp);
		MHD_add_response_header(rsp, "Location", loc_path);
		MHD_destroy_response(rsp);
		return ret;
	}

	int ret = MHD_queue_response(connection, MHD_HTTP_OK, rsp);
	MHD_destroy_response(rsp);
	return ret;
}

bool httpd::start_daemon()
{
	d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
		jconf::inst()->GetHttpdPort(), NULL, NULL,
		&httpd::req_handler,
		NULL, MHD_OPTION_END);

	if(d == nullptr)
	{
		printer::inst()->print_str("HTTP Daemon failed to start.");
		return false;
	}

	return true;
}

#endif

