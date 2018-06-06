#include "controlpanel.h"

ControlPanel::ControlPanel(QWidget* parent) : QWidget(parent) {
	//if (_backgroundColor != nullptr) {
	//	_backgroundColor = new QPalette();
	//}

	setGeometry(0, 0, 500, 80);
	setStyleSheet("background-color:green;");
	//setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setMinimumSize(500, 80);
	//_backgroundColor->setColor(QPalette::Background, Qt::green);
	//setAutoFillBackground(true);
	//setPalette(*_backgroundColor);


}

ControlPanel::~ControlPanel() {
	//if (_backgroundColor != nullptr) {
	//	delete _backgroundColor;
	//	_backgroundColor = nullptr;
	//}
}