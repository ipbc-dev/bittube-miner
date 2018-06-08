#include "statspanel.h"

StatsPanel::StatsPanel(QWidget* parent) : QWidget(parent) {
	setGeometry(0, 0, 500, 300);
	setStyleSheet("background-color: yellow");
	setMinimumSize(500, 300);
	_mainlayout = new QVBoxLayout(this);

	_outputText = new QTextEdit();
	_mainlayout->addWidget(_outputText);

	setLayout(_mainlayout);
}

StatsPanel::~StatsPanel() {
	//if (_backgroundColor != nullptr) {
	//	delete _backgroundColor;
	//	_backgroundColor = nullptr;
	//}
}