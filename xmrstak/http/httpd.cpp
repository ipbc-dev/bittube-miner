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
#include <sstream>

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

const std::string CORS_ORIGIN = "*";

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

	std::string cpuCounResult = "";
	bool isCpuCountFound = false;
	std::string currentCpuCountResult = "";
	bool isCurrCpuCountFound = false;

	std::ifstream cpuFile(CPU_FILE);

	if (cpuFile.fail()) {

		//TODO: error handling. Search for cpu backup file and try to get info there 
		// ¿? needRestart -> true ¿?

	} else {
		for (std::string line; std::getline(cpuFile, line); ) {
			if (std::regex_match(line, base_match, regPattern)) {
				cpuCounResult = " \"cpu_count\" : ";
				cpuCounResult += base_match[2];
				cpuCounResult += ", \n";

				httpd::miner_config->cpu_count = std::stoi(base_match[2]);
				isCpuCountFound = true;
			}
			if (std::regex_match(line, base_match2, regPattern2)) {
				currentCpuCountResult = "\"current_cpu_count\" : ";
				currentCpuCountResult += base_match2[2];
				currentCpuCountResult += ", \n";

				httpd::miner_config->current_cpu_count = std::stoi(base_match2[2]);
				isCurrCpuCountFound = true;
			}
		}
		if (isCpuCountFound) {
			result += cpuCounResult;
		}
		else {
			result += " \"cpu_count\" : 0";
			result += ", \n";
			//TODO: error handling.  
			// ¿? needRestart -> true ¿?
		}

		if (isCurrCpuCountFound) {
			result += currentCpuCountResult;
		}
		else {
			result += "\"current_cpu_count\" : 0";
			result += ", \n";
			//TODO: error handling.  
			// ¿? needRestart -> true ¿?
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

	std::regex initGpuInfoPattern("[^*]*\(gpu_threads_conf\)\.*");
	std::regex oneGpuInfoPattern("[^*]*\(index\)\.*");
	std::regex endOneGpuInfoPattern("[^*]*\(\\}\)\.*");

	bool initGpuSection = false;
	bool initOneGpuInfo = false;
	bool endOneGpuInfo = false;
	bool endGpuInfoSection = false;

	int gpuIndex = 0;
	std::vector<std::string> gpuInfoList;

	std::ifstream nvidiaFile("./nvidia-bck.txt");

	std::string nvidiaListResult = "";
	bool nvidiaListFound = false;

	if (nvidiaFile.fail()) {

		//TODO: error handling. Search for nvidia backup file and try to get info there
		// ¿? needRestart -> true ¿?

	} else {

		httpd::miner_config->nvidia_list.clear();
		std::string partialResult = "";

		for (std::string line; std::getline(nvidiaFile, line); ) {
			if (initGpuSection) {
				if ((!initOneGpuInfo) && (std::regex_match(line, endPattern))) {
					initGpuSection = false;
					endGpuInfoSection = true;
				}

				if (initOneGpuInfo) {
					partialResult += line;
					partialResult += "\n";
					if (std::regex_match(line, endOneGpuInfoPattern)) {
						initOneGpuInfo = false;
						gpuInfoList.push_back(partialResult);
						partialResult = "";
					}

				}
				else {
					if (std::regex_match(line, oneGpuInfoPattern)) {
						initOneGpuInfo = true;
						partialResult += line;
						partialResult += "\n";
					}
				}

			}
			else {
				if ((!endGpuInfoSection) && (std::regex_match(line, initGpuInfoPattern))) {
					initGpuSection = true;
				}
				else {
					if (initArray && std::regex_match(line, endPattern)) {
						initArray = false;
						nvidiaListResult += "], \n";
					}
					else if (initArray) {
						if (std::regex_match(line, base_match, namePattern)) {
							nvidiaListFound = true;
							if (primera) {
								primera = false;
								nvidiaListResult += "\"nvidia_list\" : [";
							}
							else {
								nvidiaListResult += ", \n";
							}
							nvidiaListResult += base_match[1];
							httpd::miner_config->nvidia_list.push_back(base_match[1]);

							std::string idTmp = "nvidia-" + std::to_string(gpuIndex);
							httpd::miner_config->gpu_list.emplace(idTmp, gpu_data{
								base_match[1],
								true,
								gpuInfoList[gpuIndex] });
							httpd::miner_config->current_use_nvidia = true;
							++gpuIndex;
						}
					}
					else if (!initArray && std::regex_match(line, mainPattern)) {
						initArray = true;
					}
				}
			}
		}
	}

	if (nvidiaListFound) {
		result += nvidiaListResult;
	}
	else {
		result += "\"nvidia_list\" : [ ] , \n";
		//TODO: error handling.  
		// ¿? needRestart -> true ¿?
	}

	if (httpd::miner_config != nullptr) {
		result += "\"nvidia_current\" : ";
		if (httpd::miner_config->current_use_nvidia) {
			result += "true, \n";
		}
		else {
			result += "false, \n";
		}
	}
	else {
		result += "\"nvidia_current\" : false, \n";
		//TODO: error handling.  
		// create httpd::miner_config object ¿?
		// ¿? needRestart -> true ¿?
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

	std::regex initGpuInfoPattern("[^*]*\(gpu_threads_conf\)\.*");
	std::regex oneGpuInfoPattern("[^*]*\(index\)\.*");
	std::regex endOneGpuInfoPattern("[^*]*\(\\}\)\.*");

	bool initGpuSection = false;
	bool initOneGpuInfo = false;
	bool endOneGpuInfo = false;
	bool endGpuInfoSection = false;

	int gpuIndex = 0;
	std::vector<std::string> gpuInfoList;

	std::ifstream amdFile("./amd-bck.txt");

	std::string amdListResult = "";
	bool amdListFound = false;

	if (amdFile.fail()) {

		//TODO: error handling. Search for cpu backup file and try to get info there  
		// ¿? needRestart -> true ¿?

	}
	else {

		httpd::miner_config->amd_list.clear();
		std::string partialResult = "";

		for (std::string line; std::getline(amdFile, line); ) {
			if (initGpuSection) {
				if ((!initOneGpuInfo) && (std::regex_match(line, endPattern))) {
					initGpuSection = false;
					endGpuInfoSection = true;
				}

				if (initOneGpuInfo) {
					partialResult += line;
					if (std::regex_match(line, endOneGpuInfoPattern)) {
						initOneGpuInfo = false;
						gpuInfoList.push_back(partialResult);
						partialResult = "";
					}

				}
				else {
					if (std::regex_match(line, oneGpuInfoPattern)) {
						initOneGpuInfo = true;
						partialResult += line;
						partialResult += "\n";
					}
				}
			}
			else {
				if ((!endGpuInfoSection) && (std::regex_match(line, initGpuInfoPattern))) {
					initGpuSection = true;
				}
				else {

					if (initArray && std::regex_match(line, endPattern)) {
						initArray = false;
						amdListResult += "], \n";
					}
					else if (initArray) {
						if (std::regex_match(line, base_match, namePattern)) {
							amdListFound = true;
							if (primera) {
								primera = false;
								amdListResult += "\"amd_list\" : [";
							}
							else {
								amdListResult += ", \n";
							}
							amdListResult += base_match[1];
							httpd::miner_config->amd_list.push_back(base_match[1]);

							std::string idTmp = "amd-" + std::to_string(gpuIndex);
							httpd::miner_config->gpu_list.emplace(idTmp, gpu_data{
								base_match[1],
								true,
								gpuInfoList[gpuIndex] });
							httpd::miner_config->current_use_amd = true;
							++gpuIndex;
						}
					}
					else if (!initArray && std::regex_match(line, mainPattern)) {
						initArray = true;
					}
				}
			}
		}
	}

	if (amdListFound) {
		result += amdListResult;
	}
	else {
		result += "\"amd_list\" : [ ], \n";
		//TODO: error handling.  
		// ¿? needRestart -> true ¿?
	}

	if (httpd::miner_config != nullptr) {
		result += "\"amd_current\" : ";
		if (httpd::miner_config->current_use_amd) {
			result += "true, \n";
		}
		else {
			result += "false, \n";
		}
	}
	else {
		result += "\"amd_current\" : false, \n";
		//TODO: error handling.  
		// create httpd::miner_config object
		// ¿? needRestart -> true ¿?
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

	std::ifstream configFile(CONFIG_FILE);

	bool httpdFound = false;

	if (configFile.fail()) {

		//TODO: error handling. Search for cpu backup file and try to get info there  
		// ¿? needRestart -> true ¿?

	} else {
		for (std::string line; std::getline(configFile, line); ) {
			if (std::regex_match(line, base_match, regPattern)) {
				httpdFound = true;
				result = " \"httpd_port\" : ";
				result += base_match[2];
				result += ", \n";
				httpd::miner_config->http_port = std::stoi(base_match[2]);
			}
		}
	}

	if (!httpdFound) {
		result = " \"httpd_port\" : 0, \n";
		//TODO: error handling.  
		// ¿? needRestart -> true ¿?
	}

	return result;
}

/*
 * Description: Get important configuration data from pools.txt
 */
std::string httpd::parsePoolFile() {
	std::string result = "";
	std::smatch base_match;
	std::smatch base_match2;

	std::regex poolPattern("[^*]*\(pool_address\)\.*[:][^\"]*\"\([a-zA-Z0-9.-]+[:][0-9]+\)\"\.*[,]\.*wallet_address\.*");
	std::regex walletPattern("[^*]*\(wallet_address\)\.*[:]\.*\"\([a-zA-Z0-9]+\)\"\.*[,]\.*rig_id\.*");

	std::ifstream poolFile(POOL_FILE);

	std::string poolAddressResult = "";
	bool poolAddressFound = false;
	std::string walletAddressResult = "";
	bool walletAddressFound = false;

	if (poolFile.fail()) {

		//TODO: error handling. Search for pool backup file and try to get info there  
		// ¿? needRestart -> true ¿?

	}
	else {
		for (std::string line; std::getline(poolFile, line); ) {
			if (std::regex_match(line, base_match, poolPattern)) {
				poolAddressFound = true;
				poolAddressResult = " \"pool_address\" : \"";
				poolAddressResult += base_match[2];
				poolAddressResult += "\", \n";
				httpd::miner_config->pool_address = base_match[2];
			}

			if (std::regex_match(line, base_match2, walletPattern)) {
				walletAddressFound = true;
				walletAddressResult = " \"wallet_address\" : \"";
				walletAddressResult += base_match2[2];
				walletAddressResult += "\",  \n";
				httpd::miner_config->wallet_address = base_match[2];
			}
		}
	}

	if (poolAddressFound) {
		result += poolAddressResult;
	}
	else {
		result += " \"pool_address\" : \"fail\", \n";
		//TODO: error handling.  
		// ¿? needRestart -> true ¿?
	}

	if (walletAddressFound) {
		result += walletAddressResult;
	}
	else {
		result += " \"wallet_address\" : \"fail\", \n";
		//TODO: error handling.  
		// ¿? needRestart -> true ¿?
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

	if (isUpdateData) {
		if (cpuCount > -1) {
			std::ifstream cpuFile(CPU_FILE);

			if (cpuFile.fail()) {
				printer::inst()->print_msg(L0, "cpu.txt not found \n");

				//TODO: error handling.  

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

		result = true;
	}

	return result;
}

/*
 * Description: Update nvidia.txt with new config data
 */
bool httpd::updateGPUNvidiaFile() {
	std::regex gpuNvidiaPattern("\.*\(nvidia\)\.*");
	std::regex gpuSectionPattern("[^*]*\(gpu_threads_conf\)\.*");
	std::regex gpuSectionEndPattern("\.*\(gpu_info\)\.*");

	std::regex gpuWriteStartPattern("\.*\(\\[\)\.*");
	std::regex gpuWriteEndPattern("\.*\(\\],\)\.*");

	std::string nvidiaConfigContent = "";

	bool result = false;
	bool isUpdateData = true;
	bool isUsingNvidia = false;
	bool oldWebMode = false;

	bool isGpuSection = false;
	bool isGpuSectionEnd = false;
	bool startWriteConfig = false;
	bool writed = false;

	if (httpd::miner_config != nullptr) {
		isUsingNvidia = miner_config->current_use_nvidia;
		oldWebMode = miner_config->oldWeb;
	}
	else {
		isUpdateData = false;
		//TODO: error handling
	}

	if (isUpdateData) {
		std::ifstream nvidiaFile(NVIDIA_FILE);

		if (nvidiaFile.fail()) {
			printer::inst()->print_msg(L0, "nvidia.txt not found \n");
		}
		else {
			for (std::string line; std::getline(nvidiaFile, line); ) {
				if (isGpuSection) {
					if (std::regex_match(line, gpuWriteEndPattern)) {
						isGpuSection = false;
						startWriteConfig = false;
					}

					if (startWriteConfig) {
						if (!writed) {
							writed = true;
							for (auto const& x : httpd::miner_config->gpu_list) {
								if (std::regex_match(x.first, gpuNvidiaPattern)) {
									if (oldWebMode) {
										if (isUsingNvidia) {
											nvidiaConfigContent += x.second.config;
											nvidiaConfigContent += "\n";
										}
									}
									else if (x.second.isInUse) {
										nvidiaConfigContent += x.second.config;
										nvidiaConfigContent += "\n";
									}
								}
							}
						}
					}
					else {
						if (std::regex_match(line, gpuWriteStartPattern)) {
							startWriteConfig = true;
						}
						nvidiaConfigContent += line;
						nvidiaConfigContent += "\n \n";
					}

				}
				else {
					if (std::regex_match(line, gpuSectionPattern)) {
						isGpuSection = true;
					}

					nvidiaConfigContent += line;
					nvidiaConfigContent += "\n";
				}
			}
			nvidiaFile.close();
			std::ofstream out(NVIDIA_FILE);
			out << nvidiaConfigContent;
			out.close();
		}
	}

	return result;
}

/*
 * Description: Update amd.txt with new config data
 */
bool httpd::updateGPUAMD() {
	std::regex gpuAmdPattern("\.*\(amd\)\.*");
	std::regex gpuSectionPattern("[^*]*\(gpu_threads_conf\)\.*");
	std::regex gpuSectionEndPattern("\.*\(gpu_info\)\.*");

	std::regex gpuWriteStartPattern("\.*\(\\[\)\.*");
	std::regex gpuWriteEndPattern("\.*\(\\],\)\.*");

	std::string amdConfigContent = "";

	bool result = false;
	bool isUpdateData = true;
	bool isUsingAmd = false;
	bool oldWebMode = false;

	bool isGpuSection = false;
	bool isGpuSectionEnd = false;
	bool startWriteConfig = false;
	bool writed = false;

	if (httpd::miner_config != nullptr) {
		isUsingAmd = miner_config->current_use_amd;
		oldWebMode = miner_config->oldWeb;
	}
	else {
		isUpdateData = false;
		//TODO: error handling
	}

	if (isUpdateData) {
		std::ifstream amdFile(AMD_FILE);

		if (amdFile.fail()) {
			printer::inst()->print_msg(L0, "amd.txt not found \n");
		}
		else {
			for (std::string line; std::getline(amdFile, line); ) {
				if (isGpuSection) {
					if (std::regex_match(line, gpuWriteEndPattern)) {
						isGpuSection = false;
						startWriteConfig = false;
					}

					if (startWriteConfig) {
						if (!writed) {
							writed = true;
							for (auto const& x : httpd::miner_config->gpu_list) {
								if (std::regex_match(x.first, gpuAmdPattern)) {
									if (oldWebMode) {
										if (isUsingAmd) {
											amdConfigContent += x.second.config;
											amdConfigContent += "\n";
										}
									}
									else if (x.second.isInUse) {
										amdConfigContent += x.second.config;
										amdConfigContent += "\n";
									}
								}
							}
						}
					}
					else {
						if (std::regex_match(line, gpuWriteStartPattern)) {
							startWriteConfig = true;
						}
						amdConfigContent += line;
						amdConfigContent += "\n \n";
					}
				}
				else {
					if (std::regex_match(line, gpuSectionPattern)) {
						isGpuSection = true;
					}
					amdConfigContent += line;
					amdConfigContent += "\n";
				}


			}
			amdFile.close();
			std::ofstream out(AMD_FILE);
			out << amdConfigContent;
			out.close();
		}
	}

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

	if (isUpdateData) {
		if (httpdPort > -1) {
			std::ifstream configFile(CONFIG_FILE);

			if (configFile.fail()) {
				printer::inst()->print_msg(L0, "config.txt not found \n");

				//TODO: error handling.

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

		result = true;
	}

	return result;
}

/*
 * Description: Update pools.txt with new config data
 */
bool httpd::updatePoolFile() {
	bool result = false;
	bool isUpdateData = true;

	std::regex regPattern("[^*]*\(pool_address\)\.*[:][^\"]*\(\"[a-zA-Z0-9.-]+[:][0-9]+\"\)\.*[,]\.*(wallet_address)\.*[:]\.*(\"[a-zA-Z0-9]+\")\.*rig_id\.*");
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

	if (isUpdateData) {
		if ((poolAddress.compare("-1") != 0) &&
			(walletAddress.compare("-1") != 0)) {
			
			std::ifstream poolFile(POOL_FILE);

			if (poolFile.fail()) {
				printer::inst()->print_msg(L0, "pool.txt not found \n");

				//TODO: error handling. 

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

		result = true;
	}

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
	partialRes += getGPUInfo();
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
	//printer::inst()->print_msg(L0, "+++ %s => %s \n", keyIN, valueIN);

	if (keyIN.compare("cpu_count") == 0) {
		//printer::inst()->print_msg(L0, "cpu_count key found \n");

		try {

			int cpu_countTMP = std::stoi(valueIN);
			if ((httpd::miner_config->current_cpu_count != cpu_countTMP) &&
				(cpu_countTMP <= httpd::miner_config->cpu_count)) {

				httpd::miner_config->current_cpu_count = cpu_countTMP;
				httpd::miner_config->isNeedUpdate = true;
			}

		} catch (int param) { 
			printer::inst()->print_msg(L0, "int exception \n");
		} catch (char param) { 
			printer::inst()->print_msg(L0, "char exception \n");
		} catch (...) { 
			printer::inst()->print_msg(L0, "default exception \n");
		}

		result = true;
	} else if (keyIN.compare("nvidia_list") == 0) {
		bool resultTmp = false;
		if ((valueIN.compare("true") == 0) ||
			(valueIN.compare("True") == 0) ||
			(valueIN.compare("TRUE") == 0) ||
			(valueIN.compare("on") == 0) ||
			(valueIN.compare("1") == 0)) {
			resultTmp = true;
		}

		if (httpd::miner_config->current_use_nvidia != resultTmp) {
			httpd::miner_config->oldWeb = true;
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
			httpd::miner_config->oldWeb = true;
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
			printer::inst()->print_msg(L0, "int exception \n");
		}
		catch (char param) {
			printer::inst()->print_msg(L0, "char exception \n");
		}
		catch (...) {
			printer::inst()->print_msg(L0, "default exception \n");
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
	else if (keyIN.compare("gpu_list") == 0) {
		//printer::inst()->print_msg(L0, "--->>> gpu_list: %s", valueIN);

		std::string listTmp = valueIN;
		std::stringstream ss(listTmp);
		std::map<std::string, gpu_data>::iterator it;
		std::string token;

		for (auto &x : httpd::miner_config->gpu_list) {
			x.second.isInUse = false;
		}

		while (getline(ss, token, ',')) {
			it = httpd::miner_config->gpu_list.find(token);
			if (it != httpd::miner_config->gpu_list.end()) {
				it->second.isInUse = true;
			}
		}

		httpd::miner_config->oldWeb = false;
		httpd::miner_config->isNeedUpdate = true;
	}
	else {
		printer::inst()->print_msg(L0, "Key not found!! \n");
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
		executor::inst()->isPause = true;
		updateCPUFile();
		updateGPUNvidiaFile();
		updateGPUAMD();
		updateConfigFile();
		updatePoolFile();
		
		executor::inst()->needRestart = true;
		
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
	MHD_add_response_header(response, "Access-Control-Allow-Origin", CORS_ORIGIN.c_str());

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
		(strcmp(key, "gpu_list") == 0) ||
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

	if (!jconf::inst()->is_safe_to_touch()) {
		printer::inst()->print_msg(L0, "ABORT HTTP HANDLER, jconf not safe to touch. %s %s \n", method, url);
		return MHD_NO;
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
		MHD_add_response_header(rsp, "Access-Control-Allow-Origin", CORS_ORIGIN.c_str());

	}
	else if (strcasecmp(url, "/ping") == 0) {

		str = "{\"status\": \"pong\"}";

		rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "application/json; charset=utf-8");
		MHD_add_response_header(rsp, "Access-Control-Allow-Origin", CORS_ORIGIN.c_str());

	}
	else if (strcasecmp(url, "/api.json") == 0)
	{
		if (httpd::miner_config != nullptr) {
			if (httpd::miner_config->isMining) {
				executor::inst()->get_http_report(EV_HTML_JSON, str);

				rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
				MHD_add_response_header(rsp, "Content-Type", "application/json; charset=utf-8");
				MHD_add_response_header(rsp, "Access-Control-Allow-Origin", CORS_ORIGIN.c_str());
			}
			else {
				str = "{\"status\": \"error\", \"description\": \"need to start mining\"}";

				rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
				MHD_add_response_header(rsp, "Content-Type", "application/json; charset=utf-8");
				MHD_add_response_header(rsp, "Access-Control-Allow-Origin", CORS_ORIGIN.c_str());
			}
			
		}
		else {
			str = "{\"status\": \"error\", \"description\": \"unknow status, please restart the app\"}";

			rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
			MHD_add_response_header(rsp, "Content-Type", "application/json; charset=utf-8");
			MHD_add_response_header(rsp, "Access-Control-Allow-Origin", CORS_ORIGIN.c_str());
		}
	}
	else if (strcasecmp(url, "/start") == 0) {
		executor::inst()->isPause = false;
		str = "{\"status\": \"ok\"}";

		rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "application/json; charset=utf-8");
		MHD_add_response_header(rsp, "Access-Control-Allow-Origin", CORS_ORIGIN.c_str());

		if (httpd::miner_config != nullptr) {
			httpd::miner_config->isMining = true;
		}
	}
	else if (strcasecmp(url, "/stop") == 0) {
		executor::inst()->isPause = true;
		str = "{\"status\": \"ok\"}";

		rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "application/json; charset=utf-8");
		MHD_add_response_header(rsp, "Access-Control-Allow-Origin", CORS_ORIGIN.c_str());

		if (httpd::miner_config != nullptr) {
			httpd::miner_config->isMining = false;
		}
	}
	else if (strcasecmp(url, "/info") == 0)
	{
		str = getCustomInfo();

		rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "application/json; charset=utf-8");
		MHD_add_response_header(rsp, "Access-Control-Allow-Origin", CORS_ORIGIN.c_str());
	}
	else if (strcasecmp(url, "/style.css") == 0)
	{
		const char* req_etag = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "If-None-Match");

		if (req_etag != NULL && strcmp(req_etag, sHtmlCssEtag) == 0)
		{ 
			rsp = MHD_create_response_from_buffer(0, nullptr, MHD_RESPMEM_PERSISTENT);

			int ret = MHD_queue_response(connection, MHD_HTTP_NOT_MODIFIED, rsp);
			MHD_destroy_response(rsp);
			return ret;
		}

		rsp = MHD_create_response_from_buffer(sHtmlCssSize, (void*)sHtmlCssFile, MHD_RESPMEM_PERSISTENT);
		MHD_add_response_header(rsp, "ETag", sHtmlCssEtag);
		MHD_add_response_header(rsp, "Content-Type", "text/css; charset=utf-8");
	}
	else if (strcasecmp(url, "/b") == 0 || strcasecmp(url, "/bittube") == 0)
	{
		generateInfoHtml(str);

		rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "text/html; charset=utf-8");
	}
	else if (strcasecmp(url, "/h") == 0 || strcasecmp(url, "/hashrate") == 0)
	{
		executor::inst()->get_http_report(EV_HTML_HASHRATE, str);

		rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "text/html; charset=utf-8");
	}
	else if (strcasecmp(url, "/c") == 0 || strcasecmp(url, "/connection") == 0)
	{
		executor::inst()->get_http_report(EV_HTML_CONNSTAT, str);

		rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "text/html; charset=utf-8");
	}
	else if (strcasecmp(url, "/r") == 0 || strcasecmp(url, "/results") == 0)
	{
		executor::inst()->get_http_report(EV_HTML_RESULTS, str);

		rsp = MHD_create_response_from_buffer(str.size(), (void*)str.c_str(), MHD_RESPMEM_MUST_COPY);
		MHD_add_response_header(rsp, "Content-Type", "text/html; charset=utf-8");
	} 
	else {
		//Do a 302 redirect to /h
		char loc_path[256];
		const char* host_val = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "Host");

		if (host_val != nullptr)
			snprintf(loc_path, sizeof(loc_path), "http://%s/b", host_val);
		else
			snprintf(loc_path, sizeof(loc_path), "/b");

		rsp = MHD_create_response_from_buffer(0, nullptr, MHD_RESPMEM_PERSISTENT);
		int ret = MHD_queue_response(connection, MHD_HTTP_TEMPORARY_REDIRECT, rsp);
		MHD_add_response_header(rsp, "Location", loc_path);
		MHD_destroy_response(rsp);
		return ret;

		//send_page(connection, errorpage);
		//---std::string responseT(errorpage);
		//---rsp = MHD_create_response_from_buffer(responseT.size(), (void*)responseT.c_str(), MHD_RESPMEM_MUST_COPY);
		//---MHD_add_response_header(rsp, "Content-Type", "text/html; charset=utf-8");
		//---MHD_add_response_header(rsp, "Access-Control-Allow-Origin", CORS_ORIGIN.c_str());
	}

	int ret = MHD_queue_response(connection, MHD_HTTP_OK, rsp);
	MHD_destroy_response(rsp);
	return ret;
}

/*
 * Description: Starting the http daemon
 *
 */
bool httpd::start_daemon() {
	d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
		jconf::inst()->GetHttpdPort(), NULL, NULL,
		&httpd::req_handler,
		NULL, MHD_OPTION_NOTIFY_COMPLETED, &httpd::request_completed,
      NULL, MHD_OPTION_END);

	if(d == nullptr)
	{
		printer::inst()->print_msg(L0, "HTTP Daemon failed to start. \n");
		return false;
	}

	return true;
}
// http functionalities - end ------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
#pragma endregion

/*
* Description:
*/
std::string httpd::getGPUInfo() {
	std::string result = "";
	bool gpuActive = false;
	bool firstLoop = true;

	if ((httpd::miner_config != nullptr) &&
		(httpd::miner_config->gpu_list.size() > 0)) {

		result += "\"gpu_list\" : [ ";

		for (auto const& x : httpd::miner_config->gpu_list) {

			if (firstLoop) {
				firstLoop = false;
			}
			else {
				result += ", ";
			}

			result += "{ \"id\" : \"";
			result += x.first + "\", ";
			result += "\"name\" : ";
			result += x.second.name + ", ";
			result += "\"isUsing\" : ";
			if (x.second.isInUse) {
				result += "true";
			}
			else {
				result += "false";
			}
			result += "} ";
		}
		result += " ],\n";
	}
	else {
		//TODO: error handling
	}

	return result;
}

void httpd::generateInfoHtml(std::string& out) {
	if (httpd::miner_config->cpu_count < 0) {
		getCustomInfo();
	}
	char date[128];
	char buffer[4096];

	out.reserve(4096);

	snprintf(buffer, sizeof(buffer), sHtmlCommonHeader, "Bit-Tube Miner", " ", "General Config");
	out.append(buffer);

	//size_t iGoodRes = vMineResults[0].count, iTotalRes = iGoodRes;
	//size_t ln = vMineResults.size();

	//for (size_t i = 1; i < ln; i++)
	//	iTotalRes += vMineResults[i].count;

	//double fGoodResPrc = 0.0;
	//if (iTotalRes > 0)
	//	fGoodResPrc = 100.0 * iGoodRes / iTotalRes;

	//double fAvgResTime = 0.0;
	//if (iPoolCallTimes.size() > 0)
	//{
	//	using namespace std::chrono;
	//	fAvgResTime = ((double)duration_cast<seconds>(system_clock::now() - tPoolConnTime).count())
	//		/ iPoolCallTimes.size();
	//}

	snprintf(buffer, sizeof(buffer), sHtmlInfoBodyHigh,
		httpd::miner_config->http_port, httpd::miner_config->pool_address.c_str(), httpd::miner_config->wallet_address.c_str(),
		httpd::miner_config->cpu_count, httpd::miner_config->current_cpu_count);
	//	int_port(iTopDiff[0]), int_port(iTopDiff[1]), int_port(iTopDiff[2]), int_port(iTopDiff[3]),
	//	int_port(iTopDiff[4]), int_port(iTopDiff[5]), int_port(iTopDiff[6]), int_port(iTopDiff[7]),
	//	int_port(iTopDiff[8]), int_port(iTopDiff[9]));

	out.append(buffer);

	if (httpd::miner_config->gpu_list.size() > 0) {
		for (auto &x : httpd::miner_config->gpu_list) {
			if (x.second.isInUse) {
				snprintf(buffer, sizeof(buffer), sHtmlInfoGPUTableRow, x.first.c_str(), x.second.name.c_str(), "true");
			}
			else {
				snprintf(buffer, sizeof(buffer), sHtmlInfoGPUTableRow, x.first.c_str(), x.second.name.c_str(), "false");
			}
		}
		out.append(buffer);
	}
	else {
		out.append(sHtmlInfoGPUErrorRow);
	}

	out.append(sHtmlInfoBodyLow);
}
#endif

