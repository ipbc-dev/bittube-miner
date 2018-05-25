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

/*
 * TODO:
 *  - finish error handling
 *  - for getting data from files, not parsing these files every time we need data if we parsed before
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

#ifdef _WIN32
#define strcasecmp _stricmp
#endif // _WIN32

#include "xmrstak/params.hpp"

#pragma region typesAndConfig 
//----------------------------------------------------------------------------------------------------------
// http types and configuration - start --------------------------------------------------------------------

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

struct connection_info_struct {
	int connectiontype;
	char *answerstring;
	struct MHD_PostProcessor *postprocessor;
};

const std::string CONFIG_FILE = "./config.txt";
const std::string POOL_FILE = "./pools.txt";
const std::string CPU_FILE = "./cpu.txt";
const std::string NVIDIA_FILE = "./nvidia.txt";
const std::string AMD_FILE = "./amd.txt";

const int POSTBUFFERSIZE = 512;
const int MAXNAMESIZE = 124;
const int MAXANSWERSIZE = 1024;
const int GET = 0;
const int POST = 1;

config_data* httpd::miner_config = nullptr;

httpd* httpd::oInst = nullptr;

httpd::httpd() {
	if (httpd::miner_config == nullptr) {
		httpd::miner_config = new config_data();
		getCustomInfo();
	}
}

// http types and configuration - end ----------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
#pragma endregion

#pragma region httpTemplates 
//----------------------------------------------------------------------------------------------------------
// Dev and test http templates - start ---------------------------------------------------------------------

//FIXME: delete this page templates when finish the testing phase
const char* greatingpage="<html><body><h1>Welcome, %s!</center></h1></body></html>";

const char* errorpage="<html><body>This doesn't seem to be right.</body></html>";

const char* askpage = "<html><body>\
                       Miner config<br>\
					   <div> SystemInfo: %s </div>\n\
					   <hr>\
                       <form action=\"/config\" method=\"post\">\
						  <label for=\"portin\">Http port: </label>\
                          <input id=\"portin\" name=\"httpd_port\" type=\"number\" value=8282>\
						  <hr>\
						  <label for=\"poolin\">Pool adress: </label>\
                          <input id=\"poolin\" name=\"pool_address\" type=\"text\" value=\"mining.bit.tube:13333\">\
						  <label for=\"walletin\">Wallet id: </label>\
                          <input id=\"walletin\" name=\"wallet_address\" type=\"text\" value=\"bxd2iN7fUb2jA4ix9S37uw1eK2iyVxDbyRD5aVzCbFqj6PSMWP6G5eW1LgBEA6cqRUEUi7hMs1xXm5Mj9s4pDcJb2jfAw9Zvm\">\
						  <hr>\
					      <label for=\"cpuin\">Using cpu </label>\
                          <input id=\"cpuin\" name=\"cpu_count\" type=\"number\" value=4>\
					      <br>\n\
						  <hr>\
					      <label for=\"nvidiain\">Nvidia GPU </label>\
					      <input id=\"nvidiain\" name=\"nvidia_list\" type=\"checkbox\" checked>\
					      <br>\n\
					      <label for=\"amdin\">AMD GPU </label>\
					      <input id=\"amdin\" name=\"amd_list\" type=\"checkbox\">\
					      <br>\n\
                          <input type=\"submit\" value=\" Send \">\
					   </form>\
					   <br>\n\
                       </body></html>";

// Dev and test http templates - end -----------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
#pragma endregion


#pragma region gettingData 
//----------------------------------------------------------------------------------------------------------
// Getting data from config files - start ------------------------------------------------------------------

/*
 * Description: Get important configuration data from cpu.txt
 */
std::string httpd::parseCPUFile() {
	std::string result = "";
	std::regex regPattern("\[^_]*\(cpu_count\)\.*\([0-9]\+\)\.*");
	std::regex regPattern2("\.*\(current_cpu_count\)\.*\([0-9]\+\)\.*");
	std::smatch base_match;
	std::smatch base_match2;
	
	//TODO: check if we have parse this info before

	std::ifstream cpuFile(CPU_FILE);

	if (cpuFile.fail()) {

		//TODO: error handling. Search for cpu backup file and try to get info there  

	} else {
		for (std::string line; std::getline(cpuFile, line); ) {
			if (std::regex_match(line, base_match, regPattern)) {
				result = " \"cpu_count\" : ";
				result += base_match[2];
				result += ", \n";

				httpd::miner_config->cpu_count = std::stoi(base_match[2]); 
			}
			if (std::regex_match(line, base_match2, regPattern2)) {
				result += "\"current_cpu_count\" : ";
				result += base_match2[2];
				result += ", \n";
				httpd::miner_config->current_cpu_count = std::stoi(base_match2[2]);
			}
		}
	}
	return result;
}

/*
 * Description: Get important configuration data from nvidia.txt
 */
std::string httpd::parseGPUNvidiaFile() {
	std::string result = "";
	std::regex mainPattern("\.*\(gpu_info\)\.*");
	std::regex namePattern("\.*\(\"\.*\"\)\.*");
	std::regex endPattern("\.*\\],\.*");
	std::smatch base_match;

	bool initArray = false;
	bool primera = true;

	//TODO: check if we have parse this info before

	std::ifstream nvidiaFile(NVIDIA_FILE);

	if (nvidiaFile.fail()) {

		//TODO: error handling. Search for nvidia backup file and try to get info there

	} else {
		for (std::string line; std::getline(nvidiaFile, line); ) {
			if (initArray && std::regex_match(line, endPattern)) {
				initArray = false;
				result += "], \n";
			} else if (initArray) {
				if (std::regex_match(line, base_match, namePattern)) {
					if (primera) {
						primera = false;
						result += "\"nvidia_list\" : [";
					} else {
						result += ", \n";
					}
					result += base_match[1];
					httpd::miner_config->nvidia_list.push_back(base_match[1]);
				}
			} else if (!initArray && std::regex_match(line, mainPattern)) {
				initArray = true;
			}
		}
	}
	return result;
}

/*
 * Description: Get important configuration data from amd.txt
 */
std::string httpd::parseGPUAMD() {
	std::string result = "";
	std::regex mainPattern("\.*\(gpu_info\)\.*");
	std::regex namePattern("\.*\(\"\.*\"\)\.*");
	std::regex endPattern("\.*\\],\.*");
	std::smatch base_match;

	bool initArray = false;
	bool primera = true;

	//TODO: check if we have parse this info before
	
	std::ifstream amdFile(AMD_FILE);

	if (amdFile.fail()) {

		//TODO: error handling. Search for amd backup file and try to get info there
	}
	else {
		for (std::string line; std::getline(amdFile, line); ) {
			if (initArray && std::regex_match(line, endPattern)) {
				initArray = false;
				result += "], \n";
			}
			else if (initArray) {
				if (std::regex_match(line, base_match, namePattern)) {
					if (primera) {
						primera = false;
						result += "\"amd_list\" : [";
					} else {
						result += ", \n";
					}
					result += base_match[1];
					httpd::miner_config->amd_list.push_back(base_match[1]);
				}
			} else if (!initArray && std::regex_match(line, mainPattern)) {
				initArray = true;
			}
		}
	}
	return result;
}

/*
 * Description: Get important configuration data from config.txt
 */
std::string httpd::parseConfigFile() {
	std::string result = "";
	std::regex regPattern("[^*]*\(httpd_port\)\.*[:][^0-9]*\([0-9]+\)\.*[,]\.*");
	std::smatch base_match;

	//TODO: check if we have parse this info before

	std::ifstream configFile(CONFIG_FILE);

	if (configFile.fail()) {

		//TODO: error handling. Search for cpu backup file and try to get info there  

	} else {
		for (std::string line; std::getline(configFile, line); ) {
			if (std::regex_match(line, base_match, regPattern)) {
				result = " \"httpd_port\" : ";
				result += base_match[2];
				result += ", \n";
				httpd::miner_config->http_port = std::stoi(base_match[2]);
			}
		}
	}
	return result;
}

/*
 * Description: Get important configuration data from pools.txt
 */
std::string httpd::parsePoolFile() {
	std::string result = "";
	std::regex regPattern("[^*]*\(pool_address\)\.*[:][^\"]*\"\([a-zA-Z0-9.]+[:][0-9]+\)\"\.*[,]\.*(wallet_address)\.*[:]\.*\"\([a-zA-Z0-9]+\)\"\.*rig_id\.*");
	std::smatch base_match;

	//TODO: check if we have parse this info before

	std::ifstream poolFile(POOL_FILE);

	if (poolFile.fail()) {

		//TODO: error handling. Search for pool backup file and try to get info there  

	} else {
		for (std::string line; std::getline(poolFile, line); ) {
			if (std::regex_match(line, base_match, regPattern)) {

				result = " \"pool_address\" : \"";
				result += base_match[2];
				result += "\", \n";
				httpd::miner_config->pool_address = base_match[2];

				result += " \"wallet_address\" : \"";
				result += base_match[4];
				result += "\",  \n";
				httpd::miner_config->wallet_address = base_match[4];
			}
		}
	}

	return result;
}

// Getting data from config files - end --------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
#pragma endregion

#pragma region updatingFiles
//----------------------------------------------------------------------------------------------------------
// Updating config files - start ---------------------------------------------------------------------------

/*
 * Description: Update cpu.txt with new config data
 */
bool httpd::updateCPUFile() {
	bool result = false;
	bool isUpdateData = true;

	std::string cpuConfigContent = "";
	std::regex cpuSectionPattern("[^*]*\(cpu_threads_conf\)\.*");
	std::regex cpuSectionEndPattern("\.*\(\\]\,\)\.*");
	bool isCpuSection = false;
	bool isConfiguringCPU = false;

	std::regex cpuLinePattern("\.*\(low_power_mode\)\.*\(no_prefetch\)\.*\(affine_to_cpu\)\.*\([0-9]\)\.*");
	std::smatch cpuLine_match;
	int currentCPUIndex = 0;
	int cpuCount = -1;
	int cpuCountObjetive = 0;

	std::regex currCPUPattern("\.*\(current_cpu_count\)\.*[:]\.*\([0-9]+\)\.*");

	if (httpd::miner_config != nullptr) {
		cpuCount = httpd::miner_config->cpu_count;
		cpuCountObjetive = httpd::miner_config->current_cpu_count;
	} else {
		isUpdateData = false;

		//TODO: error handling

	}

	//TODO: check if needed to update cpu.txt with the new config

	// Updating cpu.txt file --------------------------------------
	if (isUpdateData) {
		if (cpuCount > -1) {
			std::ifstream cpuFile(CPU_FILE);

			if (cpuFile.fail()) {
				std::cout << "not cpu.txt found" << std::endl;

				//TODO: error handling. Search for cpu backup file and try to get info there  

			}
			else {
				for (std::string line; std::getline(cpuFile, line); ) {
					if (!isCpuSection) {
						if (std::regex_match(line, cpuSectionPattern)) {
							isCpuSection = true;
						}
						if (std::regex_match(line, currCPUPattern)) {
							cpuConfigContent += "\"current_cpu_count\" : ";
							cpuConfigContent += std::to_string(cpuCountObjetive);
							cpuConfigContent += " , \n";
						} else {
							cpuConfigContent += line;
							cpuConfigContent += "\n";
						}
					}
					else {
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
						}
						else {
							if (std::regex_match(line, cpuLine_match, cpuLinePattern)) {
								isConfiguringCPU = true;
								if (currentCPUIndex < cpuCount) {
									if (currentCPUIndex < cpuCountObjetive) {
										cpuConfigContent += "    { \"low_power_mode\" : true, \"no_prefetch\" : true, \"affine_to_cpu\" : ";
										cpuConfigContent += std::to_string(currentCPUIndex);
										cpuConfigContent += " },";
										cpuConfigContent += "\n";
										++currentCPUIndex;
									}
								}
							}
							else {
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
				cpuFile.close();
				std::ofstream out(CPU_FILE);
				out << cpuConfigContent;
				out.close();
			}
		}

		//TODO: check if cpu.txt file was updated and set result value

		result = true;
	}
	//-------------------------------------------------------------

	return result;
}

/*
 * Description: Update nvidia.txt with new config data
 */
bool httpd::updateGPUNvidiaFile() {
	bool result = false;
	bool isUpdateData = true;

	std::string nvidiaConfigContent = "";
	std::string amdConfigContent = "";
	std::regex gpuSectionPattern("[^*]*\(gpu_threads_conf\)\.*");
	std::regex gpuSectionEndPattern("\.*\(gpu_info\)\.*");
	bool isGpuSection = false;

	std::regex gpuLineStartPattern("\.*\(index\)\.*[0-9]\.*");
	std::regex gpuLineEndPattern("\.*\(affine_to_cpu\)\.*\(\sync_mode)\.*");
	bool isGpuLine = false;

	bool isUsingNvidia = false;

	if (httpd::miner_config != nullptr) {
		isUsingNvidia = miner_config->current_use_nvidia;
	} else {
		isUpdateData = false;

		//TODO: error handling

	}

	//TODO: check if needed to update nvidia.txt with the new config

	// Updating nvidia.txt file -----------------------------------
	if (isUpdateData) {
		if (isUsingNvidia) {
			std::ifstream nvidiaBCKFile("./nvidia-bck.txt");

			if (nvidiaBCKFile.fail()) {
				std::cout << "not nvidia.txt found" << std::endl;
			} else {
				if (remove(NVIDIA_FILE.c_str()) != 0) {
					std::cout << "Error deleting file [nvidia.txt]" << std::endl;
				} else {
					std::cout << "File successfully deleted [nvidia.txt]" << std::endl;
				}

				try {
					std::ofstream  dst(NVIDIA_FILE, std::ios::binary);
					dst << nvidiaBCKFile.rdbuf();
				}
				catch (...) {
					std::cout << "ERROR doing a config files backup" << std::endl;
				}
			}
		} else {

			std::ifstream nvidiaFile(NVIDIA_FILE);

			if (nvidiaFile.fail()) {
				std::cout << "not nvidia.txt found" << std::endl;

				//TODO: error handling. Search for cpu backup file and try to get info there  

			} else {
				for (std::string line; std::getline(nvidiaFile, line); ) {
					if (!isGpuSection) {
						nvidiaConfigContent += line;
						nvidiaConfigContent += "\n";
						if (std::regex_match(line, gpuSectionPattern)) {
							isGpuSection = true;
							nvidiaConfigContent += "[ \n";
						}
					}
					else {
						if (std::regex_match(line, gpuSectionEndPattern)) {
							nvidiaConfigContent += "], \n \n";
							nvidiaConfigContent += line;
							nvidiaConfigContent += "\n";
							isGpuSection = false;
						}
					}
				}
				nvidiaFile.close();
				std::ofstream out(NVIDIA_FILE);
				out << nvidiaConfigContent;
				out.close();
			}
		}

		//TODO: check if nvidia.txt file was updated and set result value

		result = true;
	}
	//-------------------------------------------------------------

	return result;
}

/*
 * Description: Update amd.txt with new config data
 */
bool httpd::updateGPUAMD() {
	bool result = false;
	bool isUpdateData = true;

	std::string amdConfigContent = "";
	std::regex gpuSectionPattern("\.*\(gpu_threads_conf\)\.*");
	std::regex gpuSectionEndPattern("\.*\(gpu_info\)\.*");
	bool isGpuSection = false;

	std::regex gpuLineStartPattern("\.*\(index\)\.*[0-9]\.*");
	std::regex gpuLineEndPattern("\.*\(affine_to_cpu\)\.*\(\sync_mode)\.*");
	bool isGpuLine = false;

	bool isUsingAmd = false;

	if (httpd::miner_config != nullptr) {
		isUsingAmd = miner_config->current_use_amd;
	} else {
		isUpdateData = false;

		//TODO: error handling

	}

	//TODO: check if needed to update amd.txt with the new config

	// Updating amd.txt file --------------------------------------
	if (isUpdateData) {
		if (isUsingAmd) {
			std::ifstream amdBCKFile("./amd-bck.txt");

			if (amdBCKFile.fail()) {
				std::cout << "not amd-bck.txt found" << std::endl;
			} else {
				if (remove(AMD_FILE.c_str()) != 0) {
					std::cout << "Error deleting file [amd.txt]" << std::endl;
				}
				else {
					std::cout << "File successfully deleted [amd.txt]" << std::endl;
				}

				try {
					std::ofstream  dst(AMD_FILE, std::ios::binary);
					dst << amdBCKFile.rdbuf();
				}
				catch (...) {
					std::cout << "ERROR doing a config files backup" << std::endl;
				}
			}
		} else {
			std::ifstream amdFile(AMD_FILE);

			if (amdFile.fail()) {
				std::cout << "not nvidia.txt found" << std::endl;

				//TODO: error handling. Search for cpu backup file and try to get info there  

			}
			else {
				for (std::string line; std::getline(amdFile, line); ) {
					if (!isGpuSection) {
						amdConfigContent += line;
						amdConfigContent += "\n";
						if (std::regex_match(line, gpuSectionPattern)) {
							isGpuSection = true;
							amdConfigContent += " \n";
						}
					}
					else {
						if (std::regex_match(line, gpuSectionEndPattern)) {
							amdConfigContent += "], \n \n";
							amdConfigContent += line;
							amdConfigContent += "\n";
							isGpuSection = false;
						}
					}
				}
				amdFile.close();
				std::ofstream out(AMD_FILE);
				out << amdConfigContent;
				out.close();
			}
		}

		//TODO: check if nvidia.txt file was updated and set result value

		result = true;
	}
	//-------------------------------------------------------------

	return result;
}

/*
 * Description: Update config.txt with new config data
 */
bool httpd::updateConfigFile() {
	bool result = false;
	bool isUpdateData = true;

	std::regex regPattern("[^*]*\(httpd_port\)\.*[:][^0-9]*\([0-9]+\)\.*[,]\.*");
	int httpdPort = -1;

	std::string genConfigContent = "";

	if (httpd::miner_config != nullptr) {
		httpdPort = miner_config->http_port;
	} else {
		isUpdateData = false;

		//TODO: error handling

	}

	//TODO: check if needed to update cpu.txt with the new config

	// Updating cpu.txt file --------------------------------------
	if (isUpdateData) {
		if (httpdPort > -1) {
			std::ifstream configFile(CONFIG_FILE);

			if (configFile.fail()) {
				std::cout << "not config.txt found" << std::endl;

				//TODO: error handling. Search for cpu backup file and try to get info there  

			} else {
				for (std::string line; std::getline(configFile, line); ) {
					if (std::regex_match(line, regPattern)) {
					
						genConfigContent += "\"httpd_port\" : ";
						genConfigContent += std::to_string(httpdPort);
						genConfigContent += ", \n";

					} else {
						genConfigContent += line;
						genConfigContent += "\n";
					}
				}
				configFile.close();
				std::ofstream out(CONFIG_FILE);
				out << genConfigContent;
				out.close();
			}
		}

		//TODO: check if cpu.txt file was updated and set result value

		result = true;
	}
	//-------------------------------------------------------------

	return result;
}

/*
 * Description: Update pools.txt with new config data
 */
bool httpd::updatePoolFile() {
	bool result = false;
	bool isUpdateData = true;

	std::regex regPattern("[^*]*\(pool_address\)\.*[:][^\"]*\(\"[a-zA-Z0-9.]+[:][0-9]+\"\)\.*[,]\.*(wallet_address)\.*[:]\.*(\"[a-zA-Z0-9]+\")\.*rig_id\.*");
	std::smatch base_match;
	std::string poolAddress = "-1";
	std::string walletAddress = "-1";

	std::string poolConfigContent = "";

	if (httpd::miner_config != nullptr) {
		poolAddress = miner_config->pool_address;
		walletAddress = miner_config->wallet_address;
	} else {
		isUpdateData = false;

		//TODO: error handling

	}

	//TODO: check if needed to update cpu.txt with the new config

	// Updating cpu.txt file --------------------------------------
	if (isUpdateData) {
		if ((poolAddress.compare("-1") != 0) &&
			(walletAddress.compare("-1") != 0)) {
			
			std::ifstream poolFile(POOL_FILE);

			if (poolFile.fail()) {
				std::cout << "not config.txt found" << std::endl;

				//TODO: error handling. Search for cpu backup file and try to get info there  

			} else {
				for (std::string line; std::getline(poolFile, line); ) {
					if (std::regex_match(line, base_match, regPattern)) {
						poolConfigContent += " {\"pool_address\" : \"";
						poolConfigContent += poolAddress;
						poolConfigContent += "\", \"wallet_address\" : \"";
						poolConfigContent += walletAddress;
						poolConfigContent += "\", \"rig_id\" : \"\", ";
						poolConfigContent += "\"pool_password\" : \"x\", ";
						poolConfigContent += "\"use_nicehash\" : false, ";
						poolConfigContent += "\"use_tls\" : false, ";
						poolConfigContent += "\"tls_fingerprint\" : \"\", ";
						poolConfigContent += "\"pool_weight\" : 1 }, ";
						poolConfigContent += "\n";

					} else {
						poolConfigContent += line;
						poolConfigContent += "\n";
					}
				}
				poolFile.close();
				std::ofstream out(POOL_FILE);
				out << poolConfigContent;
				out.close();
			}
		}

		//TODO: check if cpu.txt file was updated and set result value

		result = true;
	}
	//-------------------------------------------------------------

	return result;
}

// Updating config files - end -----------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
#pragma endregion

#pragma region httpFunctionalities
//----------------------------------------------------------------------------------------------------------
// http functionalities - start ----------------------------------------------------------------------------


/*
 * Description: obtain data to send to frontend
 * 
 * TODO: Optimize using, not parsing config files for every get request
 */
std::string httpd::getCustomInfo () {
	std::string result = "";
	std::string partialRes = "-1";

	partialRes = parseCPUFile();
	partialRes += parseGPUNvidiaFile();
	partialRes += parseGPUAMD();
	partialRes += parseConfigFile();
	partialRes += parsePoolFile();
	if (httpd::miner_config != nullptr) {
		partialRes += "\"isMining\" :";
		if (httpd::miner_config->isMining) {
			partialRes += " true";
		}
		else {
			partialRes += " false";
		}
		partialRes += "  \n";
	}
	
	
	if ((partialRes.compare("-1") != 0) &&
		!(partialRes.empty())){

		result = " { ";
		result += partialRes;
		result += " } \n";
	}
	return result;
}

/*
 * Description: parsing data received from frontend
 */
bool httpd::parseCustomInfo (std::string keyIN, std::string valueIN) {
	bool result = false;

	if (keyIN.compare("cpu_count") == 0) {
		std::cout << "cpu_count key found" << std::endl;

		try {

			int cpu_countTMP = std::stoi(valueIN);
			if ((httpd::miner_config->current_cpu_count != cpu_countTMP) &&
				(cpu_countTMP <= httpd::miner_config->cpu_count)) {

				httpd::miner_config->current_cpu_count = cpu_countTMP;
				httpd::miner_config->isNeedUpdate = true;
			}

		} catch (int param) { 
			std::cout << "int exception"; 
		} catch (char param) { 
			std::cout << "char exception";
		} catch (...) { 
			std::cout << "default exception"; 
		}

		result = true;
	} else if (keyIN.compare("nvidia_list") == 0) {
		bool resultTmp = false;
		if ((valueIN.compare("true") == 0) ||
			(valueIN.compare("True") == 0) ||
			(valueIN.compare("TRUE") == 0) ||
			(valueIN.compare("1") == 0)) {
			resultTmp = true;
		}

		if (httpd::miner_config->current_use_nvidia != resultTmp) {
			httpd::miner_config->current_use_nvidia = resultTmp;
			httpd::miner_config->isNeedUpdate = true;
		}
		result = true;
	} else if (keyIN.compare("amd_list") == 0) {
		bool resultTmp = false;
		if ((valueIN.compare("true") == 0) ||
			(valueIN.compare("True") == 0) ||
			(valueIN.compare("TRUE") == 0) ||
			(valueIN.compare("1") == 0)) {
			resultTmp = true;
		}

		if (httpd::miner_config->current_use_amd != resultTmp) {
			httpd::miner_config->current_use_amd = resultTmp;
			httpd::miner_config->isNeedUpdate = true;
		}
		result = true;
	}
	else if (keyIN.compare("httpd_port") == 0) {
		try {
			int hhtp_portTMP = std::stoi(valueIN);
			if (httpd::miner_config->http_port != hhtp_portTMP) {
				httpd::miner_config->http_port = std::stoi(valueIN);
				httpd::miner_config->isNeedUpdate = true;
			}
			
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
	}
	else if (keyIN.compare("pool_address") == 0) {
		if (valueIN.compare(httpd::miner_config->pool_address) != 0) {
			httpd::miner_config->pool_address = valueIN;
			httpd::miner_config->isNeedUpdate = true;
		}
		
	}
	else if (keyIN.compare("wallet_address") == 0) {
		if (valueIN.compare(httpd::miner_config->wallet_address) != 0) {
			httpd::miner_config->wallet_address = valueIN;
			httpd::miner_config->isNeedUpdate = true;
		}
	}
	else {
		std::cout << "Key not found!!" << std::endl;
	}
	return result;
}


/*
 * Description: updates all config files with new config data
 *
 * TODO: error handling
 */
void httpd::updateConfigFiles () {
	if ((httpd::miner_config != nullptr) && (httpd::miner_config->isNeedUpdate)) {
		httpd::miner_config->isNeedUpdate = false;
		executor::isPaused = true;
		updateCPUFile();
		updateGPUNvidiaFile();
		updateGPUAMD();
		updateConfigFile();
		updatePoolFile();
		
		executor::needRestart = true;
		
	}
}

//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

/*
 *	Description: sending reponses to frontend 
 */
int httpd::send_page (struct MHD_Connection *connection, const char *page) {
	int ret;
	struct MHD_Response *response;

	response = MHD_create_response_from_buffer (strlen (page), 
												(void *) page,
												MHD_RESPMEM_PERSISTENT);
	MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");

  if (!response) {
    return MHD_NO;
  }

  ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response);

  return ret;
}

/*
 * Description: Function called for every post field
 */
int httpd::iterate_post (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
								 const char *filename, const char *content_type,
								 const char *transfer_encoding, const char *data, 
								 uint64_t off, size_t size) {

	struct connection_info_struct *con_info = (connection_info_struct*)coninfo_cls;

	if ((strcmp (key, "cpu_count") == 0) || 
		(strcmp (key, "nvidia_list") == 0) || 
		(strcmp (key, "amd_list") == 0) || 
		(strcmp(key, "httpd_port") == 0) || 
		(strcmp(key, "pool_address") == 0) || (strcmp(key, "wallet_address") == 0)){

		if ((size > 0) && (size <= MAXNAMESIZE)) {
			std::string keyParse (key);
			std::string valueParse (data);

			parseCustomInfo (keyParse, valueParse);
			con_info->answerstring = "ok";

		}  else  {
			con_info->answerstring = NULL;
		}

		return MHD_YES;
	}
	return MHD_YES;
}

/*
 * Description: Function called when the post finish 
 */
void httpd::request_completed (void *cls, 
							   struct MHD_Connection *connection, 
							   void **con_cls,
							   enum MHD_RequestTerminationCode toe) {
	std::cout << "----[httpd::request_completed(...)]" << std::endl;

	updateConfigFiles ();
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
		postResult = MHD_post_process (con_infoEx->postprocessor, upload_data,	
								*upload_data_size);

		*upload_data_size = 0;
		return MHD_YES;

	} else if (NULL != con_infoEx->answerstring) {
		return send_page (connection, con_infoEx->answerstring);
	}

	return send_page(connection, errorpage);
}

/*
 * Description: Function called for every http request
 */
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
		if ((strcmp(method, "POST") == 0) &&
			(strcasecmp(url, "/config") == 0)) {

			retValue = starting_process_post(connection, method, upload_data, upload_data_size, ptr);

			if (httpd::miner_config != nullptr) {
				httpd::miner_config->isMining = false;
			}

			return retValue;
		} else {
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

	char transform[2048];
	std::string responsetxt;
	*ptr = nullptr;
	std::string str;
	if(strcasecmp(url, "/devtest") == 0) {
		
		snprintf(transform, 2048, askpage, getCustomInfo().c_str());
		std::string responsetxtAux(transform);
		responsetxt = responsetxtAux;

		rsp = MHD_create_response_from_buffer(responsetxt.size(), (void*)responsetxt.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "text/html; charset=utf-8");
		MHD_add_response_header(rsp, "Access-Control-Allow-Origin", "*");

	}
	else if (strcasecmp(url, "/api.json") == 0)
	{
		if (httpd::miner_config != nullptr) {
			if (httpd::miner_config->isMining) {
				executor::inst()->get_http_report(EV_HTML_JSON, str);

				rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
				MHD_add_response_header(rsp, "Content-Type", "application/json; charset=utf-8");
				MHD_add_response_header(rsp, "Access-Control-Allow-Origin", "*");
			}
			else {
				str = "{\"status\": \"error\", \"description\": \"need to start mining\"}";

				rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
				MHD_add_response_header(rsp, "Content-Type", "application/json; charset=utf-8");
				MHD_add_response_header(rsp, "Access-Control-Allow-Origin", "*");
			}
			
		}
		else {
			str = "{\"status\": \"error\", \"description\": \"unknow status, please restart the app\"}";

			rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
			MHD_add_response_header(rsp, "Content-Type", "application/json; charset=utf-8");
			MHD_add_response_header(rsp, "Access-Control-Allow-Origin", "*");
		}
	}
	else if (strcasecmp(url, "/start") == 0) {
		executor::isPaused = false;
		str = "{\"status\": \"ok\"}";

		rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "application/json; charset=utf-8");
		MHD_add_response_header(rsp, "Access-Control-Allow-Origin", "*");

		if (httpd::miner_config != nullptr) {
			httpd::miner_config->isMining = true;
		}
	}
	else if (strcasecmp(url, "/stop") == 0) {
		executor::isPaused = true;
		str = "{\"status\": \"ok\"}";

		rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "application/json; charset=utf-8");
		MHD_add_response_header(rsp, "Access-Control-Allow-Origin", "*");

		if (httpd::miner_config != nullptr) {
			httpd::miner_config->isMining = false;
		}
	}
	else if (strcasecmp(url, "/info") == 0)
	{
		str = getCustomInfo();

		rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "application/json; charset=utf-8");
		MHD_add_response_header(rsp, "Access-Control-Allow-Origin", "*");
	} else {
		//send_page(connection, errorpage);
		std::string responseT(errorpage);
		rsp = MHD_create_response_from_buffer(responseT.size(), (void*)responseT.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "text/html; charset=utf-8");
		MHD_add_response_header(rsp, "Access-Control-Allow-Origin", "*");

		//Do a 302 redirect to /h
		/*char loc_path[256];
		const char* host_val = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "Host");

		if(host_val != nullptr)
			snprintf(loc_path, sizeof(loc_path), "http://%s/h", host_val);
		else
			snprintf(loc_path, sizeof(loc_path), "/h");

		rsp = MHD_create_response_from_buffer(0, nullptr, MHD_RESPMEM_PERSISTENT);
		int ret = MHD_queue_response(connection, MHD_HTTP_TEMPORARY_REDIRECT, rsp);
		MHD_add_response_header(rsp, "Location", loc_path);
		MHD_destroy_response(rsp);
		return ret;*/
	}

	int ret = MHD_queue_response(connection, MHD_HTTP_OK, rsp);
	MHD_destroy_response(rsp);
	return ret;
}

/*
 * Description: Starting the http daemon
 */
bool httpd::start_daemon() {
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
// http functionalities - end ------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
#pragma endregion
#endif

