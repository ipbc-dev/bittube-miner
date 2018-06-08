#include "controlpanel.h"

ControlPanel::ControlPanel(QWidget* parent) : QWidget(parent) {
	//if (_backgroundColor != nullptr) {
	//	_backgroundColor = new QPalette();
	//}

	setGeometry(0, 0, 500, 80);
	setStyleSheet("background-color:green;");
	//setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setMinimumSize(500, 100);
	//_backgroundColor->setColor(QPalette::Background, Qt::green);
	//setAutoFillBackground(true);
	//setPalette(*_backgroundColor);

	// Left button panel
	_main_L_Frame = new QFrame(this);
	_main_L_Frame->setGeometry(0, 0, 200, 100);
	_main_L_Frame->setStyleSheet("background-color: gray");

	_sec_T_LFrame = new QFrame(_main_L_Frame);
	_sec_T_LFrame->setGeometry(0, 0, 200, 25);
	_sec_T_LFrame->setStyleSheet("background-color: black");

	_sec_C_LFrame = new QFrame(_main_L_Frame);
	_sec_C_LFrame->setGeometry(0, 25, 200, 50);
	_sec_C_LFrame->setStyleSheet("background-color: gray");

	_console_button = new QPushButton(_sec_C_LFrame);
	_console_button->setAccessibleName("console_button");
	_console_button->setText("Console");
	_console_button->setToolTip("Show/hide console panel");
	_console_button->setToolTipDuration(2000);
	_console_button->setGeometry(144, 2, 46, 46);
	_console_button->setStyleSheet("[accessibleName=\"console_button\"] { background-color: white; \
								     border-style: solid; \
									 border-width:2px; \
									 border-radius:22px; \
									 border-color: red; \
									 max-width:42px; \
									 max-height:42px; \
									 min-width:42px; \
									 min-height:42px; }");

	_config_button = new QPushButton(_sec_C_LFrame);
	_config_button->setAccessibleName("configuration_button");
	_config_button->setText("Configuration");
	_config_button->setToolTip("Show/Hide configuration panel");
	_config_button->setToolTipDuration(2000);
	_config_button->setGeometry(104, 8, 34, 34);
	_config_button->setStyleSheet("[accessibleName=\"configuration_button\"] { background-color: white; \
								    border-style: solid; \
							        border-width:2px; \
							        border-radius:16px; \
							 	    border-color: red; \
								    max-width:30px; \
								    max-height:30px; \
								    min-width:30px; \
								    min-height:30px; }");

	_sec_B_LFrame = new QFrame(_main_L_Frame);
	_sec_B_LFrame->setGeometry(0,75, 200, 25);
	_sec_B_LFrame->setStyleSheet("background-color: black");
	//---
	
	// Center button panel
	_main_C_Frame = new QFrame(this);
	_main_C_Frame->setGeometry(200, 0, 100, 100);
	_main_C_Frame->setStyleSheet("background-color:rgba(0,0,0,0)");

	_sec_T_CFrame = new QFrame(_main_C_Frame);
	_sec_T_CFrame->setGeometry(0, 0, 100, 25);
	_sec_T_CFrame->setStyleSheet("background-color: black");

	_sec_C_CFrame = new QFrame(_main_C_Frame);
	_sec_C_CFrame->setGeometry(0, 25, 100, 50);
	_sec_C_CFrame->setStyleSheet("background-color: gray");

	_sec_B_CFrame = new QFrame(_main_C_Frame);
	_sec_B_CFrame->setGeometry(0, 75, 100, 25);
	_sec_B_CFrame->setStyleSheet("background-color: black");

	_play_button = new QPushButton(_main_C_Frame);
	_play_button->setAccessibleName("play_button");
	_play_button->setText("Play");
	_play_button->setToolTip("Start/Stop mining");
	_play_button->setToolTipDuration(2000);
	_play_button->setGeometry(1, 1, 98, 98);
	_play_button->setStyleSheet("[accessibleName=\"play_button\"] {  background-color: white; \
								  border-style: solid; \
							      border-width:4px; \
							      border-radius:49px; \
							 	  border-color: red; \
								  max-width:90px; \
								  max-height:90px; \
								  min-width:90px; \
								  min-height:90px; }");
	//---

	// Right button panel
	_main_R_Frame = new QFrame(this);
	_main_R_Frame->setGeometry(300, 0, 200, 100);
	_main_R_Frame->setStyleSheet("background-color: gray");

	_sec_T_RFrame = new QFrame(_main_R_Frame);
	_sec_T_RFrame->setGeometry(0, 0, 200, 25);
	_sec_T_RFrame->setStyleSheet("background-color: black");

	_sec_C_RFrame = new QFrame(_main_R_Frame);
	_sec_C_RFrame->setGeometry(0, 25, 200, 50);
	_sec_C_RFrame->setStyleSheet("background-color: gray");

	_stats_button = new QPushButton(_sec_C_RFrame);
	_stats_button->setAccessibleName("stats_button");
	_stats_button->setText("Stats");
	_stats_button->setToolTip("Show/Hide stats panel");
	_stats_button->setToolTipDuration(2000);
	_stats_button->setGeometry(10, 2, 46, 46);
	_stats_button->setStyleSheet("[accessibleName=\"stats_button\"] {  background-color: white; \
								   border-style: solid; \
							       border-width:2px; \
							       border-radius:22px; \
							 	   border-color: red; \
								   max-width:42px; \
								   max-height:42px; \
								   min-width:42px; \
								   min-height:42px; }");

	_result_button = new QPushButton(_sec_C_RFrame);
	_result_button->setAccessibleName("result_button");
	_result_button->setText("Results");
	_result_button->setToolTip("Show/Hide results panel");
	_result_button->setToolTipDuration(2000);
	_result_button->setGeometry(64, 8, 34, 34);
	_result_button->setStyleSheet("[accessibleName=\"result_button\"] {  background-color: white; \
								    border-style: solid; \
							        border-width:2px; \
							        border-radius:16px; \
							 	    border-color: red; \
								    max-width:30px; \
								    max-height:30px; \
								    min-width:30px; \
								    min-height:30px; }");

	_sec_B_RFrame = new QFrame(_main_R_Frame);
	_sec_B_RFrame->setGeometry(0, 75, 200, 25);
	_sec_B_RFrame->setStyleSheet("background-color: black");
	//---
}

ControlPanel::~ControlPanel() {
	//if (_backgroundColor != nullptr) {
	//	delete _backgroundColor;
	//	_backgroundColor = nullptr;
	//}
}