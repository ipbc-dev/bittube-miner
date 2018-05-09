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

#include <iostream>
#include <fstream>

#include "xmrstak/rapidjson/document.h"
#include "xmrstak/rapidjson/istreamwrapper.h"

#include <regex>

//#include <microhttpd.h>
#ifdef _WIN32
#define strcasecmp _stricmp
#endif // _WIN32

//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
// POST additions start ---

struct config_data {
	bool isConfiguring = false;//FIXME: really needed, there is posible parallel updatings¿?

	int cpu_count = -1;
	std::vector<std::string> nvidia_list; //TODO: fill the list and using internally, not parse file every post request
	std::vector<std::string> amd_list;	//TODO: fill the list and using internally, not parse file every post request

	int current_cpu_count = -1;
	bool current_use_nvidia = false;
	bool current_use_amd = false;

	bool* current_cpu;//TODO: using for disable concrete cpu not only the counter
	bool* current_nvidia; //TODO: using for disable concrete gpu, not only gpu process or not
	bool* current_amd; //TODO: using for disable concrete gpu, not only gpu process or not

	int sizeLeft = 0;
};

config_data* httpd::miner_config = nullptr;

const int POSTBUFFERSIZE = 512;
const int MAXNAMESIZE = 20;
const int MAXANSWERSIZE = 512;
const int GET = 0;
const int POST = 1;

//-----------------------------------------------------------------------------------------
//FIXME: delete this page templates when finish the testing phase
const char* greatingpage="<html><body><h1>Welcome, %s!</center></h1></body></html>";

const char* errorpage="<html><body>This doesn't seem to be right.</body></html>";

const char* askpage = "<html><body>\
                       Miner config<br>\
					   <div> SystemInfo: %s </div>\n\
                       <form action=\"/config\" method=\"post\">\
					   <label for=\"cpuin\">Using cpu </label>\
                       <input id=\"cpuin\" name=\"cpu_count\" type=\"number\">\
					   <br>\n\
					   <label for=\"nvidiain\">Nvidia GPU </label>\
					   <input id=\"nvidiain\" name=\"nvidia_list\" type=\"checkbox\">\
					   <br>\n\
					   <label for=\"amdin\">AMD GPU </label>\
					   <input id=\"amdin\" name=\"amd_list\" type=\"checkbox\">\
					   <br>\n\
                       <input type=\"submit\" value=\" Send \"></form>\
					   <br>\n\
                       </body></html>";
//-----------------------------------------------------------------------------------------

struct connection_info_struct {
  int connectiontype;
  char *answerstring;
  struct MHD_PostProcessor *postprocessor; 
};

//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

/*
 * Description: obtain data to send to frontend
 * 
 * TODO: Optimize using, not parsing config files for every get request
 */
std::string httpd::getCustomInfo () {
	bool filldata = false;

	if (httpd::miner_config == NULL) {
		httpd::miner_config = new config_data();
		filldata = true;
	}

	std::string result = "{ \"cpu_count\" : ";

	// check if exist cpu.txt file and load cpu count
	std::ifstream cpuFile("./cpu.txt");

   if(cpuFile.fail()){
        //---std::cout << "cpu.txt file NOT exists 001\n" << std::endl;
		  result += std::string("" + -1);
   } else {
		//---std::cout << "cpu.txt file exists\n" << std::endl;

		std::regex regPattern("\.*\(cpu_count\)\.*\([0-9]\+\)\.*");
		std::smatch base_match;

		//---std::cout << "------------------------------------------------------------------------------" << std::endl;
		for( std::string line; std::getline( cpuFile, line ); ) {

			//---std::cout << line << std::endl;
			if (std::regex_match(line, base_match, regPattern)) {
				//---std::cout << "Found cpu_count value: " << base_match[2] << std::endl;
				result += base_match[2];

				if (filldata) {
					httpd::miner_config->cpu_count = std::stoi(base_match[2]); //<------------------------------ TODO: use only this¿?
				}
			}
		}
		//---std::cout << "------------------------------------------------------------------------------" << std::endl;
		//---std::cout << result << std::endl;

	}
	result += ", \n";


	result += "\"nvidia_list\" : [";
	bool initArray = false;
	bool primera = true;
	// check if exist nvidia.txt file and load nvidia names string
	std::ifstream nvidiaFile("./nvidia.txt");

	if(nvidiaFile.fail()){
		//---std::cout << "nvidia.txt file couldn\'t be opened (not existing or failed to open)\n";
	} else { 
		//---std::cout << "nvidia.txt file exists\n" << std::endl;

		std::regex mainPattern("\.*\(gpu_info\)\.*");
		std::regex namePattern("\.*\(\"\.*\"\)\.*");
		std::regex endPattern("\.*\],\.*");
		std::smatch base_match;

		//---std::cout << "------------------------------------------------------------------------------" << std::endl;
		for( std::string line; std::getline( nvidiaFile, line ); ) {
			//---std::cout << line << std::endl;

			if (initArray && std::regex_match(line, endPattern)) {
				//---std::cout << ">>>>>----Found end of array" << std::endl;
				initArray = false;
			} else if (initArray) {
				if (std::regex_match(line, base_match, namePattern)) {
					//---std::cout << ">>>>>---- Found end of name" << std::endl;
					if (primera) {
						primera = false;
					} else {
						result += ", \n";
					}
					result += base_match[1];

					if (filldata) {
						httpd::miner_config->nvidia_list.push_back(base_match[1]); //<------------------------------ TODO: use only this¿?
					}

				}
			} else if (!initArray && std::regex_match(line, mainPattern)) {
				//---std::cout << ">>>>>---- Found gpu_info value" << std::endl;
				initArray = true;
			}
		}
	}
	result += "], \n";


	result += "\"amd_list\" : [";
	initArray = false;
	primera = true;
	// check if exist amd.txt file and load amd names string
	std::ifstream amdFile("./amd.txt");

	if(amdFile.fail()){
		//---std::cout << "amd.txt file couldn\'t be opened (not existing or failed to open)\n";
	} else { 
		//---std::cout << "amd.txt file exists\n" << std::endl;

		std::regex mainPattern("\.*\(gpu_info\)\.*");
		std::regex namePattern("\.*\(\"\.*\"\)\.*");
		std::regex endPattern("\.*\],\.*");
		std::smatch base_match;

		//---std::cout << "------------------------------------------------------------------------------" << std::endl;
		for( std::string line; std::getline( amdFile, line ); ) {
			std::cout << line << std::endl;

			if (initArray && std::regex_match(line, endPattern)) {
				//---std::cout << ">>>>>----Found end of array" << std::endl;
				initArray = false;
			} else if (initArray) {
				if (std::regex_match(line, base_match, namePattern)) {
					//---std::cout << ">>>>>---- Found end of name" << std::endl;
					if (primera) {
						primera = false;
					} else {
						result += ", \n";
					}
					result += base_match[1];

					if (filldata) {
						httpd::miner_config->amd_list.push_back(base_match[1]);  //<------------------------------ TODO: use only this¿?
					}
				}
			} else if (!initArray && std::regex_match(line, mainPattern)) {
				//---std::cout << ">>>>>---- Found gpu_info value" << std::endl;
				initArray = true;
			}
		}
	}
	result += "] } \n";

	//---std::cout << "--------------------------------<<" << std::endl;
	//---std::cout << result << std::endl;

	return result;
}

/*
 * Description: parsing data received from frontend
 * 
 *  TODO: 
 */
bool httpd::parseCustomInfo (std::string keyIN, std::string valueIN) {
	bool result = false;

	if (httpd::miner_config == NULL) {
		getCustomInfo ();
	}

	std::cout << "Parsing post data \n   - key: " << keyIN << " \n   - value: " << valueIN << std::endl;

	if (keyIN.compare("cpu_count") == 0) {
		std::cout << "cpu_count key found" << std::endl;
		//httpd::miner_config->current_cpu_count= std::stoi(valueIN);

		try {
			httpd::miner_config->current_cpu_count= std::stoi(valueIN);
		} catch (int param) { 
			std::cout << "int exception"; 
		} catch (char param) { 
			std::cout << "char exception";
		} catch (...) { 
			std::cout << "default exception"; 
		}

		result = true;
	} else if (keyIN.compare("nvidia_list") == 0) {
		std::cout << "nvidia_list key found" << std::endl;
		bool resultTmp = false;
		if ((valueIN.compare("true") == 0) ||
			(valueIN.compare("True") == 0) ||
			(valueIN.compare("TRUE") == 0) ||
			(valueIN.compare("1") == 0)) {
			resultTmp = true;
		}

		try {
			httpd::miner_config->current_use_nvidia = resultTmp;
		}
		catch (int param) {
			std::cout << "int exception";
		}
		catch (char param) {
			std::cout << "char exception";
		}
		catch (...) {
			std::cout << "default exception";
		}

		result = true;
	} else if (keyIN.compare("amd_list") == 0) {
		std::cout << "amd_list key found" << std::endl;
		bool resultTmp = false;
		if ((valueIN.compare("true") == 0) ||
			(valueIN.compare("True") == 0) ||
			(valueIN.compare("TRUE") == 0) ||
			(valueIN.compare("1") == 0)) {
			resultTmp = true;
		}

		try {
			httpd::miner_config->current_use_amd = resultTmp;
		}
		catch (int param) {
			std::cout << "int exception";
		}
		catch (char param) {
			std::cout << "char exception";
		}
		catch (...) {
			std::cout << "default exception";
		}

		result = true;
	} else {
		std::cout << "Key not found!!" << std::endl;
	}

	return result;
}


/*
 * Description:
 * 
 * TODO: select a concrete cpu or gpu to disable or enable, not only using counters
 */
void httpd::updateConfigFiles () {
	bool isUpdateData = true;
	// cpu section ------------------------------------------------
	std::string cpuConfigContent = "";
	std::regex cpuSectionPattern("[^*]*\(cpu_threads_conf\)\.*");
	//std::regex cpuSectionEndPattern("\.*\(cpu_count\)\.*");
	std::regex cpuSectionEndPattern("\.*\(\\]\,\)\.*");
	bool isCpuSection = false;
	bool isConfiguringCPU = false;

	std::regex cpuLinePattern("\.*\(low_power_mode\)\.*\(no_prefetch\)\.*\(affine_to_cpu\)\.*\([0-9]\)\.*");
	std::smatch cpuLine_match;
	int currentCPUIndex = 0;
	int cpuCount = -1;
	int cpuCountObjetive = 0;
	//-------------------------------------------------------------

	// GPU section ------------------------------------------------
	std::string nvidiaConfigContent = "";
	std::string amdConfigContent = "";
	std::regex gpuSectionPattern("\.*\(gpu_threads_conf\)\.*");
	std::regex gpuSectionEndPattern("\.*\(gpu_info\)\.*");
	bool isGpuSection = false;
	bool isUsingNvidia = false;

	std::regex gpuLineStartPattern("\.*\(index\)\.*[0-9]\.*");
	std::regex gpuLineEndPattern("\.*\(affine_to_cpu\)\.*\(\sync_mode)\.*");
	bool isGpuLine = false;
	bool isUsingAmd = false;
	//-------------------------------------------------------------

	// Getting updating data --------------------------------------
	if (httpd::miner_config != nullptr) {
		cpuCount = miner_config->cpu_count;
		cpuCountObjetive = miner_config->current_cpu_count;
		isUsingNvidia = miner_config->current_use_nvidia;
		isUsingAmd =  miner_config->current_use_amd;
		miner_config->isConfiguring = true;
	} else{
		isUpdateData = false;
	}
	//-------------------------------------------------------------

	// Updating cpu.txt file --------------------------------------
	if (isUpdateData)
	if(cpuCount > -1) {
		std::ifstream cpuFile("./cpu.txt");

		if(cpuFile.fail()){
			std::cout << "not cpu.txt found" << std::endl;
		} else {

			for( std::string line; std::getline( cpuFile, line ); ) {
				if(!isCpuSection) {
					if (std::regex_match(line, cpuSectionPattern)) {
						isCpuSection = true;
					}
					cpuConfigContent += line;
					cpuConfigContent += "\n";
				} else {
					if (std::regex_match(line, cpuSectionEndPattern)) {
						if (currentCPUIndex < cpuCountObjetive) {
							int initvalue = currentCPUIndex;
							for (int i = initvalue; i < cpuCountObjetive; ++i) {
								cpuConfigContent += "    { \"low_power_mode\" : true, \"no_prefetch\" : true, \"affine_to_cpu\" : ";
								cpuConfigContent += std::to_string(currentCPUIndex);
								cpuConfigContent += " },";
								cpuConfigContent += "\n";
								++currentCPUIndex;
							}
						}
						isCpuSection = false;
						isConfiguringCPU = false;
						cpuConfigContent += line;
						cpuConfigContent += "\n";
					} else {
						if (std::regex_match(line, cpuLine_match, cpuLinePattern)) {
							isConfiguringCPU = true;
							if (currentCPUIndex < cpuCount) {
								if (currentCPUIndex < cpuCountObjetive) {
									cpuConfigContent += line;
									cpuConfigContent += "\n";
									++currentCPUIndex;
								}
							} 
						} else {
							if (isConfiguringCPU) {
								int initvalue = currentCPUIndex;
								if (currentCPUIndex < cpuCountObjetive)
								for (int i = initvalue; i < cpuCountObjetive; ++i) {
									cpuConfigContent += "    { \"low_power_mode\" : true, \"no_prefetch\" : true, \"affine_to_cpu\" : ";
									cpuConfigContent += std::to_string(currentCPUIndex);
									cpuConfigContent += " },";
									cpuConfigContent += "\n";
									++currentCPUIndex;
								}
								isConfiguringCPU = false;
							} 
							cpuConfigContent += line;
							cpuConfigContent += "\n";
						}
					}
				}
			}
		}
		cpuFile.close();
		std::ofstream out("cpu.txt");
		out << cpuConfigContent;
		out.close();
	}
	//-------------------------------------------------------------

	// Updating nvidia.txt file -----------------------------------
	if (isUpdateData)
	if (isUsingNvidia) {
		std::ifstream nvidiaBCKFile("./nvidia-bck.txt");

		if(nvidiaBCKFile.fail()){
			std::cout << "not nvidia.txt found" << std::endl;
		} else {
			try {
				std::ofstream  dst("./nvidia.txt",   std::ios::binary);

				dst << nvidiaBCKFile.rdbuf();
			} catch (...) {
				std::cout << "ERROR doing a config files backup" << std::endl;
			}
		}


		//std::ifstream nvidiaFile("./nvidia.txt");
		//if(nvidiaFile.fail()){
		//	std::cout << "not nvidia.txt found" << std::endl;
		//} else { 
		//	std::ifstream nvidiaBCKFile("./nvidia-bck.txt");
			//for( std::string line; std::getline( nvidiaFile, line ); ) {
			//	
			//}
		//}

	} else {
		if( remove( "./nvidia.txt" ) != 0 ) {
			std::cout << "Error deleting file [nvidia.txt]" << std::endl;
		} else {
			std::cout << "File successfully deleted [nvidia.txt]" << std::endl;
		}
	}
	//-------------------------------------------------------------

	// Updating amd.txt file --------------------------------------
	if (isUpdateData)
	if (isUsingAmd) {
		std::ifstream amdBCKFile("./amd-bck.txt");

		if(amdBCKFile.fail()){
			std::cout << "not amd.txt found" << std::endl;
		} else {
					try {
						std::ofstream  dst("./amd.txt",   std::ios::binary);

						dst << amdBCKFile.rdbuf();
					} catch (...) {
						std::cout << "ERROR doing a config files backup" << std::endl;
					}
		}


		//std::ifstream amdFile("./amd.txt");
		//if(amdFile.fail()){
		//} else { 
			//TODO: check and update amd.txt
		//	for( std::string line; std::getline( amdFile, line ); ) {
		//	}
		//}

	} else {
		if( remove( "./amd.txt" ) != 0 ) {
			std::cout << "Error deleting file [amd.txt]" << std::endl;
		} else {
			std::cout << "File successfully deleted [amd.txt]" << std::endl;
		}
	}
	//-------------------------------------------------------------
	if (httpd::miner_config != nullptr) {
		miner_config->isConfiguring = false;
	}
}

//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

/*
 *	Description: sending reponses to frontend 
 *
 * TODO: change name and refactor to optimize function for sending json data not html
 */
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

/*
 * Description: Function called for every ¿Post chunk?
 * 
 * TODO: check why this function is only calling with the first field of the post form
 */
int httpd::iterate_post (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
								 const char *filename, const char *content_type,
								 const char *transfer_encoding, const char *data, 
								 uint64_t off, size_t size) {

	std::cout << "----[httpd::iterate_post(...)]" << std::endl;
	std::cout << "Reciving post 002a[iterate_post]\n   - key: " << key << "\n   - value: " << data << std::endl;
	std::cout << "off: " << off << std::endl;
	std::cout << "size: " << size << std::endl;
	
	//---std::cout << "Data: " << data << std::endl; 

	struct connection_info_struct *con_info = (connection_info_struct*)coninfo_cls;

	if ((strcmp (key, "cpu_count") == 0)
		|| (strcmp (key, "nvidia_list") == 0)
		|| (strcmp (key, "amd_list") == 0)){ 

	// FIXME: delete example ---------------
	//
	//	std::cout << "Reciving a name" << std::endl;
	//
	//	if ((size > 0) && (size <= MAXNAMESIZE)) {
   //       char *answerstring;
   //       answerstring = (char*)malloc (MAXANSWERSIZE);
	//
   //       if (!answerstring) {
	//			 return MHD_NO;
	//		 }
   //   
   //       snprintf (answerstring, MAXANSWERSIZE, greatingpage, data);
   //       con_info->answerstring = answerstring;      
   //   
	//	} else  {
	//		con_info->answerstring = NULL;
	//	}
	//
	//	return MHD_NO;
	//} else { //--------------------------------------------------------------
		int sum = 0;

		if ((size > 0) && (size <= MAXNAMESIZE)) {
			std::string keyParse (key);
			sum += keyParse.size();

			std::string valueParse (data);
			sum += valueParse.size();

			parseCustomInfo (keyParse, valueParse);
			con_info->answerstring = "ok";

			std::cout << "data count: " << sum << std::endl;
			if (httpd::miner_config != nullptr) {
				httpd::miner_config->sizeLeft = sum;
			}
		}  else  {
			con_info->answerstring = NULL;
		}

		return MHD_YES;
	}
	return MHD_YES;
}

/*
 * Description: Function called when the post finish 
 * ¿sure? TODO: check this behaviour
 */
void httpd::request_completed (void *cls, 
										 struct MHD_Connection *connection, 
										 void **con_cls,
										 enum MHD_RequestTerminationCode toe) {
	
	std::cout << "----[httpd::request_completed(...)]" << std::endl;

	// ----------------------------------------------------------------------
	// First aproach --------------------------------------------------------
	//while (miner_config->isConfiguring) {
		//FIXME: need waiting for parallel updating ¿?
	//}

	updateConfigFiles ();
	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------

/*	struct connection_info_struct *con_info = (connection_info_struct*)*con_cls;

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
	*con_cls = NULL;   */
}

/*
 * Description: preparing post processing
 */
int httpd::starting_process_post (MHD_Connection* connection,
												const char* method,
												const char* upload_data,
												size_t* upload_data_size,
												void ** ptr) {

	std::cout << "[httpd::starting_process_post(...)]Receiving a post msg... " << std::endl;

	if(NULL == *ptr) {
		struct connection_info_struct *con_info;

		con_info = (connection_info_struct*)malloc (sizeof (struct connection_info_struct));
				
		if (NULL == con_info) { 
			return MHD_NO;
		}
				
		con_info->answerstring = NULL;
		con_info->postprocessor = MHD_create_post_processor (connection, POSTBUFFERSIZE, 
																						  iterate_post, (void*) con_info);   

		if (con_info->postprocessor == NULL) {
			free (con_info); 
			return MHD_NO;
		}

		con_info->connectiontype = POST;
		*ptr = (void*) con_info; 
		return MHD_YES;
	}

	struct connection_info_struct *con_infoEx = (connection_info_struct*)*ptr;
	int postResult = -1;

	if (*upload_data_size != 0) {
		std::cout << "-------------------------- upload_data_size: " << *upload_data_size << std::endl;

		postResult = MHD_post_process (con_infoEx->postprocessor, upload_data,	
								*upload_data_size);


		std::cout << "-------------------------- upload_data_size: " << *upload_data_size << std::endl;
		std::cout << "-------------------------- postResult: " << postResult << std::endl;


		*upload_data_size = 0;
		return MHD_YES;

	} else if (NULL != con_infoEx->answerstring) {
		return send_page (connection, con_infoEx->answerstring);
	}

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
	//std::cout << "   - url: " << url  << std::endl;
	//std::cout << "   - method: " << method  << std::endl;
	//std::cout << "   - version: " << version  << std::endl;
	//std::cout << "   - upload_data: " << upload_data  << std::endl;

	int retValue;

	if (strcmp(method, "GET") != 0) {
		//--------------------------------------------------------------------------------------
		// POST additions 2 start ---
		if ((strcmp(method, "POST") == 0) &&
			(strcasecmp(url, "/config") == 0)) {

			retValue = starting_process_post(connection, method, upload_data, upload_data_size, ptr);

			return retValue;
			
			//=============================================================================================================
				/*if(NULL == *ptr) {
					struct connection_info_struct *con_info;

					con_info = (connection_info_struct*)malloc (sizeof (struct connection_info_struct));
							
					if (NULL == con_info) { 
						return MHD_NO;
					}
							
					con_info->answerstring = NULL;
					con_info->postprocessor = MHD_create_post_processor (connection, POSTBUFFERSIZE, 
																									iterate_post, (void*) con_info);   

					if (con_info->postprocessor == NULL) {
						free (con_info); 
						return MHD_NO;
					}

					con_info->connectiontype = POST;
					*ptr = (void*) con_info; 
					return MHD_YES;
				}

				struct connection_info_struct *con_infoEx = (connection_info_struct*)*ptr;
				int postResult = -1;

				if (*upload_data_size != 0) {
					std::cout << "-------------------------- upload_data_size: " << *upload_data_size << std::endl;

					postResult = MHD_post_process (con_infoEx->postprocessor, upload_data,	
											*upload_data_size);


					std::cout << "-------------------------- upload_data_size: " << *upload_data_size << std::endl;
					std::cout << "-------------------------- postResult: " << postResult << std::endl;



					*upload_data_size = 0;
					return MHD_YES;
				} else if (NULL != con_infoEx->answerstring) {
					return send_page (connection, con_infoEx->answerstring);
				}

				return send_page(connection, errorpage); */
			//=============================================================================================================



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

	char transform[1024];
	std::string responsetxt; // (askpage);
	*ptr = nullptr;
	std::string str;
	if(strcasecmp(url, "/devtest") == 0) { //FIXME: delete this when finish the testinf phase
		
		snprintf(transform, 1024, askpage, getCustomInfo().c_str());
		std::string responsetxtAux(transform);
		responsetxt = responsetxtAux;

		rsp = MHD_create_response_from_buffer(responsetxt.size(), (void*)responsetxt.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "text/html; charset=utf-8");

	} else if(strcasecmp(url, "/style.css") == 0) {
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
	else if(strcasecmp(url, "/info") == 0)
	{
		str =  getCustomInfo ();
		//executor::inst()->get_http_report(EV_HTML_JSON, str);

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

/*
 * Description: ...
 */
bool httpd::start_daemon() {
	//getCustomInfo (); //FIXME: only for testing, deleting this line when work done

	/*d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
		jconf::inst()->GetHttpdPort(), NULL, NULL,
		&httpd::req_handler,
		NULL, MHD_OPTION_END);*/
	
	//MHD_USE_SELECT_INTERNALLY

	d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
		jconf::inst()->GetHttpdPort(), NULL, NULL,
		&httpd::req_handler,
		NULL, MHD_OPTION_NOTIFY_COMPLETED, &httpd::request_completed,
      NULL, MHD_OPTION_END);

	if(d == nullptr)
	{
		printer::inst()->print_str("HTTP Daemon failed to start.");
		return false;
	}

	return true;
}

#endif

