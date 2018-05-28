#pragma once

#include <stdlib.h>
#include <string>
#include <vector>

#include <microhttpd.h>

struct MHD_Daemon;
struct MHD_Connection;

struct config_data {
	bool isNeedUpdate = false;

	int http_port = 8282;
	std::string pool_address = "mining.bit.tube:13333";
	std::string wallet_address = "bxd2iN7fUb2jA4ix9S37uw1eK2iyVxDbyRD5aVzCbFqj6PSMWP6G5eW1LgBEA6cqRUEUi7hMs1xXm5Mj9s4pDcJb2jfAw9Zvm";

	int cpu_count = -1;
	std::vector<std::string> nvidia_list;
	std::vector<std::string> amd_list;

	int current_cpu_count = -1;
	bool current_use_nvidia = false;
	bool current_use_amd = false;

	bool isMining = false;
};

class httpd {

public:
	static httpd* inst() {

		if (oInst == nullptr) oInst = new httpd;
		return oInst;
	};

	static void cls() {
		if (httpd::miner_config != nullptr) {
			delete httpd::miner_config;
			httpd::miner_config = nullptr;
		}

		if (oInst->d != nullptr) {
			MHD_stop_daemon(oInst->d);
			
			//delete oInst->d;
			//oInst->d = nullptr;

			delete oInst;
			oInst = nullptr;
		}


	};

	static config_data* miner_config;

	static void miningState(bool isMiningIN) {
		if (httpd::miner_config != nullptr) {
			httpd::miner_config->isMining = isMiningIN;
		}
	};

	

	bool start_daemon();

private:
	httpd();
	static httpd* oInst;



	static std::string parseCPUFile();
	static std::string parseGPUNvidiaFile();
	static std::string parseGPUAMD();
	static std::string parseConfigFile();
	static std::string parsePoolFile();

	static bool updateCPUFile();
	static bool updateGPUNvidiaFile();
	static bool updateGPUAMD();
	static bool updateConfigFile();
	static bool updatePoolFile();

	static std::string getCustomInfo ();
	static bool parseCustomInfo (std::string keyIN, std::string valueIN);
	static void updateConfigFiles ();

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

	static void request_completed (void *cls, 
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
