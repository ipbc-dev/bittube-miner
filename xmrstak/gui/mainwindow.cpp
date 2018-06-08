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

	//_mainBackgroundImg = new QImage(100, 100, QImage::Format_RGB888);
	//_mainBackgroundImg->fill(QColor(Qt::red).rgb());

	//_mainBackgroundPalette = new QPalette();
	//_mainBackgroundPalette->setBrush(this->backgroundRole(), QBrush(QPixmap::fromImage(*_mainBackgroundImg)));
	//setPalette(*_mainBackgroundPalette);
	
	// - New GUI-------------------------------------------------------------
	//    - tercera: usando qframes ---------------------

	//_mainVLayout = new QVBoxLayout(this);

	_mainFrame = new QFrame(this);
	_mainFrame->setGeometry(0, 0, 500, 700);
	_mainFrame->setFrameShape(QFrame::StyledPanel);
	_mainFrame->setFrameShadow(QFrame::Raised);
	_mainFrame->setStyleSheet("background-color:rgba(0,0,0,0)");
	//_mainFrame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	_sec_T_Frame = new QFrame(_mainFrame);
	_sec_T_Frame->setGeometry(0, 0, 500, 300);
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
	_sec_B_Frame->setGeometry(0, 400, 500, 300);
	_sec_B_Frame->setFrameShape(QFrame::StyledPanel);
	_sec_B_Frame->setFrameShadow(QFrame::Raised);
	_sec_B_Frame->setStyleSheet("background-color:rgba(0,0,0,0)");
	//_sec_B_Frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	_statsPanel = new StatsPanel(_sec_B_Frame);

	//_sec_T_Frame->show();
	//_sec_C_Frame->show();
	//_sec_B_Frame->show();
	//_mainFrame->show();

	//_mainVLayout->addWidget(_mainFrame);


	//    - tercera: usando qframes ---------------------

	//_mainWindow = new QWidget(this);

	/*_mainVLayout = new QVBoxLayout(this);

	_main_T_Group = new QGroupBox(this);
	_main_T_Group->setGeometry(0,0,500,400);
	_consolePanel = new ConsolePanel(this);
	_main_TGroup_Layout = new QVBoxLayout(_main_T_Group);
	_main_TGroup_Layout->addWidget(_consolePanel);
	_main_T_Group->setLayout(_main_TGroup_Layout);
	_mainVLayout->addWidget(_main_T_Group);

	_main_C_Group = new QGroupBox(this);
	_main_C_Group->setGeometry(0, 400, 500, 100);
	_controlPanel = new ControlPanel(this);
	_main_CGroup_Layout = new QHBoxLayout(_main_C_Group);
	_main_CGroup_Layout->addWidget(_controlPanel);
	_main_C_Group->setLayout(_main_CGroup_Layout);
	_mainVLayout->addWidget(_main_C_Group);
	
	_main_B_Group = new QGroupBox(this);
	_main_B_Group->setGeometry(0, 500, 500, 400);
	_statsPanel = new StatsPanel(this);
	_main_BGroup_Layout = new QVBoxLayout(_main_B_Group);
	_main_BGroup_Layout->addWidget(_statsPanel);
	_main_B_Group->setLayout(_main_BGroup_Layout);
	_mainVLayout->addWidget(_main_B_Group);*/

	//_mainWindow->setLayout(_mainVLayout);


	// - Old GUI-------------------------------------------------------------
	
	/*_window = new QWidget(this);

	_mainLayout = new QVBoxLayout(this);
	

	_contentLayout = new QHBoxLayout();
	_mainLayout->addLayout(_contentLayout);
	_contentWidget = new QWidget();
	_contentWidget->setStyleSheet("background-color: green");
	_contentLayout->addWidget(_contentWidget);

	_innerContentLayout = new QHBoxLayout();
	_contentWidget->setLayout(_innerContentLayout);

	_outputText = new QTextEdit();
	_outputText->setText("Hello im testing it");
	_innerContentLayout->addWidget(_outputText);

	//---

	_footerLayout = new QHBoxLayout();
	_mainLayout->addLayout(_footerLayout);
	_footerWidget = new QWidget();
	_footerWidget->setStyleSheet("background-color: white");
	_footerLayout->addWidget(_footerWidget);

	_innerFooterLayout = new QHBoxLayout();
	_footerWidget->setLayout(_innerFooterLayout);

	_statsButton = new QPushButton();
	_statsButton->setText("Stats");
	connect(_statsButton, SIGNAL(released()), this, SLOT(slot_statsButtonClick()));
	_innerFooterLayout->addWidget(_statsButton);

	_startButton = new QPushButton();
	_startButton->setText("Start");
	connect(_startButton, SIGNAL(released()), this, SLOT(slot_startButtonClick()));
	_innerFooterLayout->addWidget(_startButton);

	//FIXME: test ---
	_mainLayout->addLayout(_mainVLayout);
	//---------------

	_window->setLayout(_mainLayout);
	setCentralWidget(_window);*/
	// -   -------------------------------------------------------------

	startGUILoop();

	connect(_worker, SIGNAL(signal_updateGUIOutput()), this, SLOT(slot_updateGUIOutput()));
}

/*
 * Description: ...
 */
MainWindow::~MainWindow() {
	// Main section
	if (_outputText != nullptr) {
		delete _outputText;
		_outputText = nullptr;
	}
	if (_innerContentLayout != nullptr) {
		delete _innerContentLayout;
		_innerContentLayout = nullptr;
	}
	if (_contentWidget != nullptr) {
		delete _contentWidget;
		_contentWidget = nullptr;
	}
	if (_contentLayout != nullptr) {
		delete _contentLayout;
		_contentLayout = nullptr;
	}
	//---

	// Footer section
	if (_statsButton != nullptr) {
		delete _statsButton;
		_statsButton = nullptr;
	}
	if (_startButton != nullptr) {
		delete _startButton;
		_startButton = nullptr;
	}
	if (_innerFooterLayout != nullptr) {
		delete _innerFooterLayout;
		_innerFooterLayout = nullptr;
	}
	if (_footerWidget != nullptr) {
		delete _footerWidget;
		_footerWidget = nullptr;
	}
	if (_footerLayout != nullptr) {
		delete _footerLayout;
		_footerLayout = nullptr;
	}
	//---

	if (_mainBackgroundImg != nullptr) {
		delete _mainBackgroundImg;
		_mainBackgroundImg = nullptr;
	}

	if(_mainBackgroundPalette != nullptr) {
		delete _mainBackgroundPalette;
		_mainBackgroundPalette = nullptr;
	}

	if (_mainLayout != nullptr) {
		delete _mainLayout;
		_mainLayout = nullptr;
	}

	if (_window != nullptr) {
		delete _window;
		_window = nullptr;
	}

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
	

	std::string outputContent = _outputText->toPlainText().toStdString();
	int diff = 0;
	int finalLength = outputContent.size() + textIN.size();

	if (finalLength > GUI_CONFIG::OUTPUT_MAX_SIZE) {
		diff = finalLength - GUI_CONFIG::OUTPUT_MAX_SIZE;
		outputContent = outputContent.substr(diff, finalLength - diff);
		outputContent += textIN;

		QString lineIN = QString::fromStdString(outputContent);

		_outputText->setText(lineIN);

	}
	else {
		QString lineIN = QString::fromStdString(textIN);
		_outputText->append(lineIN);
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

/*
 * Description: ...
 */
void MainWindow::clearOutputMSG() {
	//TODO: ...
}

/*
 * Description: ...
 */
void MainWindow::showInitMSG() {
	//TODO: ...
}

/*
 * Description: ...
 */
void MainWindow::showPauseMSG() {
	//TODO: ...
}

/*
 * Description: ...
 */
void MainWindow::showRunningMSG() {
	//TODO: ...
}

/*
 * Description: ...
 */
void MainWindow::showStatsMSG() {
	//TODO: ...
}
#pragma endregion