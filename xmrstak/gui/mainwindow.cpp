#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	resize(750, 350);
	setWindowTitle("Miner");

	_mainBackgroundImg = new QImage(100, 100, QImage::Format_RGB888);
	_mainBackgroundImg->fill(QColor(Qt::red).rgb());

	_mainBackgroundPalette = new QPalette();
	_mainBackgroundPalette->setBrush(this->backgroundRole(), QBrush(QPixmap::fromImage(*_mainBackgroundImg)));
	setPalette(*_mainBackgroundPalette);
	
	_window = new QWidget(this);

	mainLayout = new QVBoxLayout(this);
	

	contentLayout = new QHBoxLayout();
	mainLayout->addLayout(contentLayout);
	contentWidget = new QWidget();
	contentWidget->setStyleSheet("background-color: green");
	contentLayout->addWidget(contentWidget);

	innerContentLayout = new QHBoxLayout();
	contentWidget->setLayout(innerContentLayout);

	outputText = new QTextEdit();
	outputText->setText("Hello im testing it");
	innerContentLayout->addWidget(outputText);

	//---

	footerLayout = new QHBoxLayout();
	mainLayout->addLayout(footerLayout);
	footerWidget = new QWidget();
	footerWidget->setStyleSheet("background-color: white");
	footerLayout->addWidget(footerWidget);

	innerFooterLayout = new QHBoxLayout();
	footerWidget->setLayout(innerFooterLayout);

	statsButton = new QPushButton();
	statsButton->setText("Stats");
	innerFooterLayout->addWidget(statsButton);

	startButton = new QPushButton();
	startButton->setText("Start");
	innerFooterLayout->addWidget(startButton);

	_window->setLayout(mainLayout);
	setCentralWidget(_window);
}

MainWindow::~MainWindow() {
	// Main section
	if (outputText != nullptr) {
		delete outputText;
		outputText = nullptr;
	}
	if (innerContentLayout != nullptr) {
		delete innerContentLayout;
		innerContentLayout = nullptr;
	}
	if (contentWidget != nullptr) {
		delete contentWidget;
		contentWidget = nullptr;
	}
	if (contentLayout != nullptr) {
		delete contentLayout;
		contentLayout = nullptr;
	}
	//---

	// Footer section
	if (statsButton != nullptr) {
		delete statsButton;
		statsButton = nullptr;
	}
	if (startButton != nullptr) {
		delete startButton;
		startButton = nullptr;
	}
	if (innerFooterLayout != nullptr) {
		delete innerFooterLayout;
		innerFooterLayout = nullptr;
	}
	if (footerWidget != nullptr) {
		delete footerWidget;
		footerWidget = nullptr;
	}
	if (footerLayout != nullptr) {
		delete footerLayout;
		footerLayout = nullptr;
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

	if (mainLayout != nullptr) {
		delete mainLayout;
		mainLayout = nullptr;
	}

	if (_window != nullptr) {
		delete _window;
		_window = nullptr;
	}
}