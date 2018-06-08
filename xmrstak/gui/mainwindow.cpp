#include "mainwindow.h"
#include "guimanager.h"

#include <QString>
#include <QSizePolicy>

#include "xmrstak/misc/executor.hpp"
#include "xmrstak/http/httpd.hpp"

#include <iostream>

MainWindow* MainWindow::oInst = nullptr;

#pragma region CLASS QT_GUILoop 
/*
 * Description: ...
 */
QT_GUILoop::~QT_GUILoop() {
	if (_startTime != nullptr) {
		delete _startTime;
		_startTime = nullptr;
	}

	if (_startResultsTime != nullptr) {
		delete _startResultsTime;
		_startResultsTime = nullptr;
	}

	if (_currentTime != nullptr) {
		delete _currentTime;
		_currentTime = nullptr;
	}

	if (_mainTimer != nullptr) {
		if (_mainTimer->isActive()) {
			_mainTimer->stop();
		}
		delete _mainTimer;
		_mainTimer = nullptr;
	}

	if (_mainThread != nullptr) {
		if (_mainThread->isRunning()) {
			_mainThread->terminate();
			_mainThread->wait();
		}
	}
}

/*
 * Description: ...
 */
void QT_GUILoop::init(QObject* parent) {
	if (_mainTimer == nullptr) {
		_mainTimer = new QTimer(parent);
	}

	if (_mainThread == nullptr) {
		_mainThread = new QThread(parent);
	}
	 
	if (_startTime == nullptr) {
		_startTime = new QTime();
	}

	if (_startResultsTime == nullptr) {
		_startResultsTime = new QTime();
	}

	if (_currentTime == nullptr) {
		_currentTime = new QTime();
	}


}

/*
 * Description: ...
 */
void QT_GUILoop::slot_start() {
	//TODO: ...
}

/*
 * Description: ...
 */
void QT_GUILoop::slot_update() {
	int diff = -1;
	int diffResult = -1;
	bool parseGeneralStats = false;

	if (_currentTime != nullptr) {
		_currentTime->start();
	}
	if ((_startTime != nullptr) && (_currentTime != nullptr)) {
		diff = _startTime->msecsTo(*_currentTime);
	}
	if ((_startResultsTime != nullptr) && (_currentTime != nullptr)) {
		diffResult = _startResultsTime->msecsTo(*_currentTime);
	}

	if (MainWindow::inst()->isStatsPOpen())
	if ((GUIManager::inst()->isUpdateReqStats()) || ((diff > -1) && (diff >= GUI_CONFIG::QT_LOOP_MINING_STATS_FPS))) {
		GUIManager::inst()->setForceParsingStats(true);
		parseGeneralStats = true;
	}
	if (MainWindow::inst()->isResultPOpen())
	if ((GUIManager::inst()->isUpdateReqResults()) || ((diffResult > -1) && (diffResult >= GUI_CONFIG::QT_LOOP_MINING_RESULTS_FPS))) {
		GUIManager::inst()->setForceParsingResults(true);
		parseGeneralStats = true;
	}
	if(MainWindow::inst()->isConnectionPOpen())
	if (GUIManager::inst()->isUpdateReqConnection()) {
		GUIManager::inst()->setForceParsingConnectionData(true);
		parseGeneralStats = true;
	}

	emit signal_updateGUIOutput();

	if (parseGeneralStats)  {
		if (GUIManager::inst()->isMinig()) {
			std::string statsResult;
			executor::inst()->get_http_report(EV_HTML_JSON, statsResult);
			GUIManager::inst()->parseStats(QString::fromUtf8(statsResult.c_str()));
		}

		if ((diff > -1) && (diff >= GUI_CONFIG::QT_LOOP_MINING_STATS_FPS)) {
			_startTime->start();
		}

		if ((diffResult > -1) && (diffResult >= GUI_CONFIG::QT_LOOP_MINING_RESULTS_FPS)) {
			_startResultsTime->start();
		}
	}
}

/*
 * Description: ...
 */
void QT_GUILoop::slot_end() {
	//TODO: ...
}
#pragma endregion

#pragma region CLASS MainWindow
/*
 * Description: ...
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	if (oInst == nullptr) {
		oInst = this;
	}

	resize(GUI_CONFIG::WINDOW_W, GUI_CONFIG::WINDOW_H);
	setWindowTitle(tr(GUI_CONFIG::WINDOW_TITLE.c_str()));

	setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
	setParent(0); // Create TopLevel-Widget
	setAttribute(Qt::WA_NoSystemBackground, true);
	setAttribute(Qt::WA_TranslucentBackground, true);

	//---
	_mainFrame = new QFrame(this);
	_mainFrame->setGeometry(0, 0, 500, 700);
	_mainFrame->setFrameShape(QFrame::StyledPanel);
	_mainFrame->setFrameShadow(QFrame::Raised);
	_mainFrame->setStyleSheet("background-color:rgba(0,0,0,0)");
	//_mainFrame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	_sec_T_Frame = new QFrame(_mainFrame);
	_sec_T_Frame->setGeometry(0, 50, 500, 400);
	_sec_T_Frame->setFrameShape(QFrame::StyledPanel);
	_sec_T_Frame->setFrameShadow(QFrame::Raised);
	_sec_T_Frame->setStyleSheet("background-color:rgba(0,0,0,0)");
	//_sec_T_Frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_consolePanel = new ConsolePanel(_sec_T_Frame);
	

	_sec_C_Frame = new QFrame(_mainFrame);
	_sec_C_Frame->setGeometry(0, 300, 500, 100);
	_sec_C_Frame->setFrameShape(QFrame::StyledPanel);
	_sec_C_Frame->setFrameShadow(QFrame::Raised);
	_sec_C_Frame->setStyleSheet("background-color:rgba(0,0,0,0)");
	//_sec_C_Frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_controlPanel = new ControlPanel(_sec_C_Frame);

	_sec_B_Frame = new QFrame(_mainFrame);
	_sec_B_Frame->stackUnder(_sec_C_Frame);
	_sec_B_Frame->setGeometry(0, 365, 500, 400);
	_sec_B_Frame->setFrameShape(QFrame::StyledPanel);
	_sec_B_Frame->setFrameShadow(QFrame::Raised);
	_sec_B_Frame->setStyleSheet("background-color:rgba(0,0,0,0)");
	//_sec_B_Frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_statsPanel = new StatsPanel(_sec_B_Frame);


	startGUILoop();

	connect(_worker, SIGNAL(signal_updateGUIOutput()), this, SLOT(slot_updateGUIOutput()));
}

/*
 * Description: ...
 */
MainWindow::~MainWindow() {
	oInst = nullptr;
	if (_worker != nullptr) {
		delete _worker;
		_worker = nullptr;
	}
}

// Public section

/*
 * Description: ...
 */
void MainWindow::showLine(std::string textIN) {
	
	//FIXME: use correct _outputText
	//---std::string outputContent = _outputText->toPlainText().toStdString();
	std::string outputContent = ""; // Delete this
	//---

	int diff = 0;
	//FIXME: use correct _outputText
	//---int finalLength = outputContent.size() + textIN.size();
	int finalLength = 0; //Delete this
	//---


	if (finalLength > GUI_CONFIG::OUTPUT_MAX_SIZE) {
		diff = finalLength - GUI_CONFIG::OUTPUT_MAX_SIZE;
		outputContent = outputContent.substr(diff, finalLength - diff);
		outputContent += textIN;

		QString lineIN = QString::fromStdString(outputContent);

		//FIXME: use correct _outputText
		//_outputText->setText(lineIN);

	}
	else {
		QString lineIN = QString::fromStdString(textIN);

		//FIXME: use correct _outputText
		//_outputText->append(lineIN);
	}

	
}

/*
 * Description: ...
 */
void MainWindow::startGUILoop() {
	_worker = new QT_GUILoop();
	_worker->init(this);
	_worker->moveToThread(_worker->getMAinThread());
	connect(_worker->getMainTimer(), SIGNAL(timeout()), _worker, SLOT(slot_update()));
	connect(_worker->getMAinThread(), SIGNAL(finished()), _worker, SLOT(deleteLater()));
	_worker->getMAinThread()->start();
	_worker->getMainTimer()->start((1000 / GUI_CONFIG::QT_LOOP_OUPUT_FPS));
	_worker->getStartTime()->start();
	_worker->getStartResultsTime()->start();
}

/*
 * Description: ...
 */
void stopGUILoop() {
	//TODO: ...
}

// Public slot section

/*
 * Description: ...
 */
void MainWindow::slot_updateGUIOutput() {
	bool withError = false;

	std::string tmp = GUIManager::inst()->getLogHead(&withError);

	if (!withError && tmp.size() > 0) {
		//showLine(tmp);
	}
}

/*
 * Description: ...
 */
void MainWindow::slot_updateMinerStats() {
	QJsonObject* jsonStats = GUIManager::inst()->getMiningStats();

	if (jsonStats != nullptr) {
		//TODO: show data on GUI
	} else {
		//TODO: jsonStats == nullptr error handling
	}
}

/*
 * Description: ...
 */
void MainWindow::slot_updateMinerResults() {
	QJsonObject* jsonResults = GUIManager::inst()->getMiningResults();

	if (jsonResults != nullptr) {
		//TODO: show data on GUI
	}
	else {
		//TODO: jsonStats == nullptr error handling
	}
}

/*
 * Description: ...
 */
void MainWindow::slot_updateMinerConfig() {
	QJsonObject* jsonConfig = GUIManager::inst()->getConnectionData();

	if (jsonConfig != nullptr) {
		//TODO: show data on GUI
	}
	else {
		//TODO: jsonStats == nullptr error handling
	}
}


// Private slot section

/*
 * Description: ...
 */
void MainWindow::slot_statsButtonClick() {
	//bool withError = false;

	//std::string tmp = GUIManager::inst()->getLogHead(&withError);

	//if (!withError && tmp.size() > 0) {
	//	showLine(tmp);
	//}
}

/*
 * Description: ...
 */
void MainWindow::slot_startButtonClick() {
	//TODO: ...
}

// Private section

#pragma endregion