#include "guimanager.h"

#include <iostream>

GUIManager* GUIManager::oInst = nullptr;

/*
 * Description: ...
 */
GUIManager::GUIManager() {
	if (_guiLog == nullptr) {
		_guiLog = new enviroment::log_data();
	}
}

/*
 * Description: ...
 */
GUIManager::~GUIManager() {
	if (_guiLog != nullptr) {
		delete _guiLog;
		_guiLog = nullptr;
	}
}

/*
 * Description: ...
 */
void GUIManager::test() {
	std::cout << "Realizando test nueva clase" << std::endl;
}

/*
 * Description: ...
 */
bool GUIManager::isLogEmpty() {
	bool result = true;

	if ((_guiLog != nullptr) && (_guiLog->gui_logQueque.size() > 0)) {
		result = false;
	}

	return result;
}

/*
* Description: ...
*/
void GUIManager::addLogLine(std::string lineIN) {
	if ((_guiLog != nullptr) && (_guiLog->logMutex.try_lock())) {
		//TODO: check log max size
		for (int i = 0; i < _guiLog->gui_backupQueque.size(); ++i) {
			_guiLog->gui_logQueque.push_back(_guiLog->gui_backupQueque[i]);
		}
		_guiLog->gui_backupQueque.clear();
		_guiLog->gui_logQueque.push_back(lineIN);
		_guiLog->logMutex.unlock();
	}
	else if (_guiLog != nullptr){
		_guiLog->gui_backupQueque.push_back(lineIN);
	}
}

/*
* Description: ...
*/
std::string GUIManager::getLogHead(bool* onerror) {
	std::string result = "";
	int resultSize = 0;
	bool continueLoop = false;
	bool error = false;
	int loopIndex = 0;

	*onerror = false;

	if ((_guiLog != nullptr) && (_guiLog->gui_logQueque.size() > 0)) {
		continueLoop = true;
		error = !(_guiLog->logMutex.try_lock());


		if (!error) {
			while (continueLoop) {
				if ((result.size() + _guiLog->gui_logQueque[loopIndex].size()) < enviroment::LOG_MAX_SIZE) {
					result += _guiLog->gui_logQueque[loopIndex];
					result += " \n";
					++resultSize;
				}
				else {
					continueLoop = false;
				}

				if (continueLoop && (loopIndex >= (_guiLog->gui_logQueque.size() - 1))) {
					continueLoop = false;
				}
				else {
					++loopIndex;
				}
			}

			if (resultSize > 0) {
				_guiLog->gui_logQueque.erase(_guiLog->gui_logQueque.begin(), _guiLog->gui_logQueque.begin() + resultSize);
			}

			_guiLog->logMutex.unlock();
		}
		else {
			*onerror = true;
		}
	}
	return result;
}