#pragma once

#include "xmrstak/misc/environment.hpp"

#include <string>
#include <vector>
#include <mutex>

namespace enviroment {
	const int LOG_MAX_SIZE = 10000;

	struct log_data {
		std::recursive_mutex logMutex;
		std::vector<std::string> gui_logQueque;
		std::vector<std::string> gui_backupQueque;
	};
}


class GUIManager {

	public:
		static GUIManager* inst() {

			//if (oInst == nullptr) oInst = new GUIManager;
			//return oInst;

			auto& env = xmrstak::environment::inst();
			if (env.pGUIManager == nullptr)
				env.pGUIManager = new GUIManager;
			return env.pGUIManager;

		};
		
		void test();

		bool isLogEmpty();
		void addLogLine(std::string lineIN);
		std::string getLogHead(bool* onerror);


	private:
		GUIManager();
		~GUIManager();
		static GUIManager* oInst;

		enviroment::log_data* _guiLog = nullptr;
};