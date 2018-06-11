#pragma once

#include "controlpanel.h"
#include "consolepanel.h"
#include "statspanel.h"
#include "configpanel.h"


#include <QMainWindow>

#include <QGroupBox>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>

#include <QTimer>
#include <QThread>
#include <QTime>

#include <QFrame>

#include <QPropertyAnimation>

namespace Ui {
	class MainWindow;
}

namespace GUI_CONFIG {
	const int WINDOW_W = 500;
	const int WINDOW_H = 700;
	const std::string WINDOW_TITLE = "Miner";

	const int OUTPUT_MAX_SIZE = 10000;

	const int QT_LOOP_OUPUT_FPS = 20;
	const int QT_LOOP_MINING_STATS_FPS = 1000 / 5; //data in miliseconds (5 FPS)
	const int QT_LOOP_MINING_RESULTS_FPS = 1000 / 1; //data in miliseconds (1 FPS)
}

#pragma region CLASS QT_GUILoop 
class QT_GUILoop : public QObject {
	Q_OBJECT
	public:
		explicit QT_GUILoop(QObject* parent = 0) :QObject(parent) {};
		virtual ~QT_GUILoop();
		
		void init(QObject* parent);
		QTimer* getMainTimer() { return _mainTimer; };
		QThread* getMAinThread() { return _mainThread; };


		QTime* getStartTime() { return _startTime; };
		QTime* getStartResultsTime() { return _startResultsTime; };
		QTime* getCurrentTime() { return _currentTime; };


	public slots:
		void slot_start();//TODO:
		void slot_update();
		void slot_end();//TODO:

	signals:
		void signal_updateGUIOutput();
		void signal_updateMinerStats();
		void signal_updateMinerResults();
		void signal_updateMinerConfig();

	private:
		//bool _isRunnig;
		QTimer* _mainTimer = nullptr;
		QThread* _mainThread = nullptr;
		QTime* _startTime = nullptr;
		QTime* _startResultsTime = nullptr;
		QTime* _currentTime = nullptr;
};
#pragma endregion

#pragma region CLASS MainWindow
class MainWindow : public QMainWindow {
	Q_OBJECT

	public:
		static MainWindow* inst() {
		
			return oInst;
		};

		explicit MainWindow(QWidget *parent = 0);
		virtual ~MainWindow();

		void showLine(std::string textIN);
		void startGUILoop();
		void stopGUILoop();

		bool isConsolePOpen() {
			return isConsolePanelOpen;
		};

		bool isStatsPOpen() {
			return isStatsPanelOpen;
		};

		bool isResultPOpen() {
			return isResultPanelOpen;
		};

		bool isConnectionPOpen() {
			return isConnectionPanelOpen;
		};
	
	public slots:
		void slot_updateGUIOutput();
		void slot_updateMinerStats();
		void slot_updateMinerResults();
		void slot_updateMinerConfig();
	
		void slot_switchConsolePanel(bool openIN);
		void slot_switchConfigPanel(bool openIN);
		void slot_switchStatsPanel(bool openIN);
		void slot_switchResultsPanel(bool openIN);

	private slots:
		void slot_statsButtonClick();//TODO:
		void slot_startButtonClick();//TODO:


		

	private:
		static MainWindow* oInst;

		bool isConsolePanelOpen = false;
		bool isStatsPanelOpen = false;
		bool isResultPanelOpen = false;
		bool isConnectionPanelOpen = false;
		bool isConfigurationPanelOpen = false;

		QT_GUILoop* _worker = nullptr; //TODO: liberar memoria 

		QFrame* _mainFrame = nullptr;
		QFrame* _sec_T_Frame = nullptr;
		QFrame* _sec_C_Frame = nullptr;
		QFrame* _sec_B_Frame = nullptr;

		ConsolePanel* _consolePanel = nullptr;
		ControlPanel* _controlPanel = nullptr;
		StatsPanel* _statsPanel = nullptr;
		ConfigPanel* _configPanel = nullptr;

		QPropertyAnimation* _open_secTFrame = nullptr;
		QPropertyAnimation* _close_secTFrame = nullptr;

		QPropertyAnimation* _open_secBFrame = nullptr;
		QPropertyAnimation* _close_secBFrame = nullptr;
		//-------------------------------------------------------------
};
#pragma endregion