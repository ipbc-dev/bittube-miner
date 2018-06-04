#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>

namespace Ui {
	class MainWindow;
}

namespace GUI_CONFIG {
	const int WINDOW_W = 750;
	const int WINDOW_H = 350;
	const std::string WINDOW_TITLE = "Miner";

	const int OUTPUT_MAX_SIZE = 10000;
}

class QTLog : public QObject {
	Q_OBJECT
};

class MainWindow : public QMainWindow {
	Q_OBJECT

	public:
		static MainWindow* inst() {
		
			return oInst;
		};

		explicit MainWindow(QWidget *parent = 0);
		virtual ~MainWindow();

		void showLine(std::string textIN);
	
	public slots:
		void slot_updateGUIOutput();

	private slots:
		void slot_statsButtonClick();
		void slot_startButtonClick();
		

	private:
		static MainWindow* oInst;

		void clearOutputMSG();

		void showInitMSG();
		void showPauseMSG();
		void showRunningMSG();
		void showStatsMSG();


		void startMining();
		void stopMining();

		QWidget* _window = nullptr;
		QVBoxLayout* _mainLayout = nullptr;
		QImage* _mainBackgroundImg = nullptr;
		QPalette* _mainBackgroundPalette = nullptr;
	
		// Main section
		QHBoxLayout* _contentLayout = nullptr;
		QWidget* _contentWidget = nullptr;
		QHBoxLayout* _innerContentLayout = nullptr;

		QTextEdit* _outputText = nullptr;
		//---

		// Footer section
		QHBoxLayout* _footerLayout = nullptr;
		QWidget* _footerWidget = nullptr;
		QHBoxLayout* _innerFooterLayout = nullptr;

		QPushButton* _statsButton = nullptr;
		QPushButton* _startButton = nullptr;
		//---
};