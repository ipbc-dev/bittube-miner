#include "statspanel.h"

StatsPanel::StatsPanel(QWidget* parent) : QWidget(parent) {
	//if (_backgroundColor == nullptr) {
	//	_backgroundColor = new QPalette();
	//}

	setGeometry(0, 0, 500, 300);
	setStyleSheet("background-color:yellow;");
	setMinimumSize(500, 300);
	//_backgroundColor->setColor(QPalette::Background, Qt::yellow);
	//setAutoFillBackground(true);
	//setPalette(*_backgroundColor);
}

StatsPanel::~StatsPanel() {
	//if (_backgroundColor != nullptr) {
	//	delete _backgroundColor;
	//	_backgroundColor = nullptr;
	//}
}