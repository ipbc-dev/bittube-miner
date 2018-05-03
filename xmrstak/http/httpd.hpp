#pragma once

#include <stdlib.h>

#include <microhttpd.h>

struct MHD_Daemon;
struct MHD_Connection;
//enum MHD_ValueKind: int;
//enum MHD_RequestTerminationCode: int;

class httpd
{
public:
	static httpd* inst() {

		if (oInst == nullptr) oInst = new httpd;
		return oInst;
	};

	bool start_daemon();

private:
	httpd();
	static httpd* oInst;

	static int starting_process_post (MHD_Connection* connection,
												  const char* method,
												  const char* upload_data,
												  size_t* upload_data_size,
												  void ** ptr);

	static int send_page (struct MHD_Connection *connection,
								 const char *page);

	static int iterate_post (void *coninfo_cls, 
									 MHD_ValueKind kind, 
									 const char *key,
									 const char *filename, 
									 const char *content_type,
									 const char *transfer_encoding, 
									 const char *data, 
									 uint64_t off, 
									 size_t size);

	void request_completed (void *cls, 
									struct MHD_Connection *connection, 
									void **con_cls,
									enum MHD_RequestTerminationCode toe);

	static int req_handler(void * cls,
								  MHD_Connection* connection,
								  const char* url,
								  const char* method,
								  const char* version,
								  const char* upload_data,
								  size_t* upload_data_size,
								  void ** ptr);

	MHD_Daemon *d;
};
