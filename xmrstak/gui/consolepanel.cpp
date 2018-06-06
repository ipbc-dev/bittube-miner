#include "consolepanel.h"

ConsolePanel::ConsolePanel(QWidget* parent) : QWidget(parent) {
	//if (_backgroundColor == nullptr) {
	//	_backgroundColor = new QPalette();
	//}

	setGeometry(0, 0, 500, 300);
	setStyleSheet("background-color: yellow");
	setMinimumSize(500, 300);
	_mainlayout = new QVBoxLayout(this);

	_outputText = new QTextEdit();
	_mainlayout->addWidget(_outputText);

	setLayout(_mainlayout);
	
	//_backgroundColor->setColor(QPalette::Background, Qt::yellow);
	//setAutoFillBackground(true);
	//setPalette(*_backgroundColor);
}

ConsolePanel::~ConsolePanel() {
	//if (_backgroundColor != nullptr) {
	//	delete _backgroundColor;
	//	_backgroundColor = nullptr;
	//}
}