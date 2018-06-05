#pragma once

#include "xmrstak/misc/environment.hpp"

#include <string>
#include <vector>
#include <mutex>

#include <QJsonObject>
#include <QString>


namespace enviroment {
	const int LOG_MAX_SIZE = 10000;

	struct log_data {
		bool isMinig = false;

		std::recursive_mutex logMutex;
		std::vector<std::string> gui_logQueque;
		std::vector<std::string> gui_backupQueque;

		std::recursive_mutex statMutex;
		QJsonObject* gui_statObject = nullptr;//TODO: safe thread, add mutex to read and write
		std::string gui_backupStat;

		bool needParseMinerStats = true;
		std::recursive_mutex minerStatsMutex;
		QJsonObject* gui_minerStatsObject = nullptr;

		bool needParseResults = true;
		std::recursive_mutex minerResultsMutex;
		QJsonObject* gui_minerResultsObject = nullptr;

		bool needParseConnectionData = true;
		std::recursive_mutex connectionMutex;
		QJsonObject* gui_connectionObject = nullptr;

	};
}


class GUIManager {

	public:
		static GUIManager* inst() {

			auto& env = xmrstak::environment::inst();
			if (env.pGUIManager == nullptr)
				env.pGUIManager = new GUIManager;
			return env.pGUIManager;

		};
		
		void test();

		bool isLogEmpty();
		void addLogLine(std::string lineIN);
		std::string getLogHead(bool* onerror);

		bool isMinig();
		void setIsMinig(bool value);

		void parseStats(QString jsonObject);

		QJsonObject* getStats();
		QJsonObject* getMiningStats();
		QJsonObject* getMiningResults();
		QJsonObject* getConnectionData();

		void setForceParsingStats(bool value) {
			if (_guiLog != nullptr) {
				_guiLog->needParseMinerStats = value;
			}
		};
		
		void setForceParsingResults(bool value) {
			if (_guiLog != nullptr) {
				_guiLog->needParseResults = value;
			}
		};

		void setForceParsingConnectionData(bool value) {
			if (_guiLog != nullptr) {
				_guiLog->needParseConnectionData = value;
			}
		}

	private:
		GUIManager();
		~GUIManager();
		static GUIManager* oInst;

		enviroment::log_data* _guiLog = nullptr;
};