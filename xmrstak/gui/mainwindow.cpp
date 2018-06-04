#include "mainwindow.h"
#include "guimanager.h"

#include <QString>

MainWindow* MainWindow::oInst = nullptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	if (oInst == nullptr) {
		oInst = this;
	}

	resize(GUI_CONFIG::WINDOW_W, GUI_CONFIG::WINDOW_H);
	setWindowTitle(tr(GUI_CONFIG::WINDOW_TITLE.c_str()));

	_mainBackgroundImg = new QImage(100, 100, QImage::Format_RGB888);
	_mainBackgroundImg->fill(QColor(Qt::red).rgb());

	_mainBackgroundPalette = new QPalette();
	_mainBackgroundPalette->setBrush(this->backgroundRole(), QBrush(QPixmap::fromImage(*_mainBackgroundImg)));
	setPalette(*_mainBackgroundPalette);
	
	_window = new QWidget(this);

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

	_window->setLayout(_mainLayout);
	setCentralWidget(_window);
}

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
}

// Public section

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

// Public slot section
void MainWindow::slot_updateGUIOutput() {

}


// Private slot section
void MainWindow::slot_statsButtonClick() {
	bool withError = false;

	std::string tmp = GUIManager::inst()->getLogHead(&withError);

	if (!withError && tmp.size() > 0) {
		showLine(tmp);
	}
}

void MainWindow::slot_startButtonClick() {
	//TODO: ...
}

// Private section
void MainWindow::clearOutputMSG() {
	//TODO: ...
}

void MainWindow::showInitMSG() {
	//TODO: ...
}

void MainWindow::showPauseMSG() {
	//TODO: ...
}

void MainWindow::showRunningMSG() {
	//TODO: ...
}

void MainWindow::showStatsMSG() {
	//TODO: ...
}