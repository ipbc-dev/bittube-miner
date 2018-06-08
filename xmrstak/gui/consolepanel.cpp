#include "consolepanel.h"

ConsolePanel::ConsolePanel(QWidget* parent) : QWidget(parent) {
	setGeometry(0, 0, 480, 285);
	setStyleSheet("background-color: yellow");
	setMinimumSize(480, 285);

	_main_T_Frame = new QFrame(this);
	_main_T_Frame->setGeometry(10, 0, 480, 260);
	_main_T_Frame->setStyleSheet("background-color:white");

	_main_B_Frame = new QFrame(this);
	_main_B_Frame->setGeometry(10, 260, 480, 25);
	_main_B_Frame->setStyleSheet("background-color:rgba(235, 235, 235, 1)");

	_outputText = new QTextEdit(_main_T_Frame);
	_outputText->setGeometry(2, 2, 466, 252);
	_outputText->setStyleSheet("background-color:rgba(235, 235, 235, 1)");

}

ConsolePanel::~ConsolePanel() {
	//if (_backgroundColor != nullptr) {
	//	delete _backgroundColor;
	//	_backgroundColor = nullptr;
	//}
}