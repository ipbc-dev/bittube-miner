#include "guimanager.h"

#include <iostream>

#include <QJsonDocument>
#include <QJsonParseError>

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
		if (_guiLog->gui_statObject != nullptr) {
			delete _guiLog->gui_statObject;
			_guiLog->gui_statObject = nullptr;
		}

		if (_guiLog->gui_minerStatsObject != nullptr) {
			delete _guiLog->gui_minerStatsObject;
			_guiLog->gui_minerStatsObject = nullptr;
		}

		if (_guiLog->gui_minerResultsObject != nullptr) {
			delete _guiLog->gui_minerResultsObject;
			_guiLog->gui_minerResultsObject = nullptr;
		}

		if (_guiLog->gui_connectionObject != nullptr) {
			delete _guiLog->gui_connectionObject;
			_guiLog->gui_connectionObject = nullptr;
		}

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

/*
 * Description: ...
 */
bool GUIManager::isMinig() {
	bool result = false;

	if (_guiLog != nullptr) {
		result = _guiLog->isMinig;
	}

	return result;
}

/*
 * Description: ...
 */
void GUIManager::setIsMinig(bool value) {
	if (_guiLog != nullptr) {
		_guiLog->isMinig = value;
	}
}

/*
 * Description: ...
 */
QJsonObject* GUIManager::getStats() {
	return _guiLog->gui_statObject;
}

/*
 * Description: ...
 */
void GUIManager::parseStats(QString jsonObject) {
	QJsonParseError jerror;

	if (_guiLog != nullptr) {
		if (_guiLog->needParseMinerStats || _guiLog->needParseResults || _guiLog->needParseConnectionData) {
			QJsonDocument document = QJsonDocument::fromJson(jsonObject.toUtf8(), &jerror);
			if (jerror.error == QJsonParseError::NoError) {
				if (_guiLog->statMutex.try_lock()) {

					if (_guiLog->gui_statObject != nullptr) {
						delete _guiLog->gui_statObject;
					}
					_guiLog->gui_statObject = new QJsonObject(document.object());
					_guiLog->statMutex.unlock();

					if (_guiLog->needParseMinerStats)
						if (_guiLog->gui_statObject->contains("hashrate")) {
							if (_guiLog->gui_statObject->value("hashrate").isObject()) {
								if (_guiLog->minerStatsMutex.try_lock()) {

									if (_guiLog->gui_minerStatsObject != nullptr) {
										delete _guiLog->gui_minerStatsObject;
									}
									_guiLog->gui_minerStatsObject = new QJsonObject(_guiLog->gui_statObject->value("hashrate").toObject());
									_guiLog->needParseMinerStats = false;
									_guiLog->minerStatsMutex.unlock();
								}
								else {
									//TODO: create logic when the mutex is locked
								}
							}
							else {
								//TODO: error handling when hashrate key isn´t json object
							}
						}
						else {
							//TODO: no hashrate key found error
						}

						if (_guiLog->needParseResults)
							if (_guiLog->gui_statObject->contains("results")) {
								if (_guiLog->gui_statObject->value("results").isObject()) {

									if (_guiLog->minerResultsMutex.try_lock()) {

										if (_guiLog->gui_minerResultsObject != nullptr) {
											delete _guiLog->gui_minerResultsObject;
										}
										_guiLog->gui_minerResultsObject = new QJsonObject(_guiLog->gui_statObject->value("hashrate").toObject());
										_guiLog->needParseResults = false;
										_guiLog->minerResultsMutex.unlock();
									}
									else {
										//TODO: create logic when the mutex is locked
									}
								}
								else {
									//TODO: error handling when results key isn´t json object
								}
							}
							else {
								//TODO: no results key found error
							}

							if (_guiLog->needParseConnectionData)
								if (_guiLog->gui_statObject->contains("connection")) {
									if (_guiLog->gui_statObject->value("connection").isObject()) {
										if (_guiLog->connectionMutex.try_lock()) {

											if (_guiLog->gui_connectionObject != nullptr) {
												delete _guiLog->gui_connectionObject;
											}
											_guiLog->gui_connectionObject = new QJsonObject(_guiLog->gui_statObject->value("hashrate").toObject());
											_guiLog->needParseConnectionData = false;
											_guiLog->connectionMutex.unlock();
										}
										else {
											//TODO: create logic when the mutex is locked
										}
									}
									else {
										//TODO: error handling when connection key isn´t json object
									}
								}
								else {
									//TODO: no connection key found error
								}
				}
				else {
					//TODO: create logic when the mutex is locked
				}
			}
			else {
				//TODO: parsing error handling
			}
		}// no need to parsing anything
	} else {
		//TODO: (_guiLog == nullptr) error handling
	}
}

/*
 * Description: ...
 */
QJsonObject* GUIManager::getMiningStats() {
	QJsonObject* result = nullptr;

	if (_guiLog != nullptr) {
		if (_guiLog->gui_minerStatsObject != nullptr) {
			if (_guiLog->minerStatsMutex.try_lock()) {
				result = _guiLog->gui_minerStatsObject;
				_guiLog->minerStatsMutex.unlock();
			} else {
				//TODO: create logic when the mutex is locked
			}
		} else {
			//TODO: (gui_minerStatsObject == nullptr) error handling
		}
	} else {
		//TODO: (_guiLog == nullptr) error handling
	}

	return result;
}

/*
 * Description: ...
 */
QJsonObject* GUIManager::getMiningResults() {
	QJsonObject* result = nullptr;

	if (_guiLog != nullptr) {
		if (_guiLog->gui_minerResultsObject != nullptr) {
			if (_guiLog->minerResultsMutex.try_lock()) {
				result = _guiLog->gui_minerResultsObject;
				_guiLog->minerResultsMutex.unlock();
			} else {
				//TODO: create logic when the mutex is locked
			}
		} else {
			//TODO: (gui_minerResultsObject == nullptr) error handling
		}
	} else {
		//TODO: (_guiLog == nullptr) error handling
	}

	return result;
}

/*
 * Description: ...
 */
QJsonObject* GUIManager::getConnectionData() {
	QJsonObject* result = nullptr;

	if (_guiLog != nullptr) {
		if (_guiLog->gui_connectionObject != nullptr) {
			if (_guiLog->connectionMutex.try_lock()) {
				result = _guiLog->gui_connectionObject;
				_guiLog->connectionMutex.unlock();
			} else {
				//TODO: create logic when the mutex is locked
			}
		} else {
			//TODO: (gui_connectionObject == nullptr) error handling
		}
	} else {
		//TODO: (_guiLog == nullptr) error handling
	}

	return result;
}