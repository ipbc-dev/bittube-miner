#pragma once

#include <QFrame>
#include <QWidget>
//#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>

class ControlPanel : public QWidget {
	Q_OBJECT

	public:
		explicit ControlPanel(QWidget* parent = 0);
		virtual ~ControlPanel();

	private:
		QPalette* _backgroundColor = nullptr;

		// Left button panel
		QFrame* _main_L_Frame = nullptr;
		QFrame* _sec_T_LFrame = nullptr;
		QFrame* _sec_C_LFrame = nullptr;
		QFrame* _sec_B_LFrame = nullptr;

		QPushButton* _console_button = nullptr;
		QPushButton* _config_button = nullptr;
		//---

		// Center button panel
		QFrame* _main_C_Frame = nullptr;
		QFrame* _sec_T_CFrame = nullptr;
		QFrame* _sec_C_CFrame = nullptr;
		QFrame* _sec_B_CFrame = nullptr;

		QPushButton* _play_button = nullptr;
		//---

		// Right button panel
		QFrame* _main_R_Frame = nullptr;
		QFrame* _sec_T_RFrame = nullptr;
		QFrame* _sec_C_RFrame = nullptr;
		QFrame* _sec_B_RFrame = nullptr;

		QPushButton* _stats_button = nullptr;
		QPushButton* _result_button = nullptr;
		//---

		// windows system buttons panel
		QFrame* _win_L_CFrame = nullptr;
		QFrame* _win_R_RFrame = nullptr;

		QPushButton* _win_minimize_button = nullptr;
		QPushButton* _win_maximize_button = nullptr;
		QPushButton* _win_close_button = nullptr;
		//---

		//FIXME: delete 
		QGroupBox* _main_L_Group = nullptr;
		QGroupBox* _main_C_Group = nullptr;
		QGroupBox* _main_R_Group = nullptr;

		QGroupBox* _sec_T_LGroup = nullptr;
		QGroupBox* _sec_C_LGroup = nullptr;
		QGroupBox* _sec_B_LGroup = nullptr;

		QGroupBox* _sec_T_RGroup = nullptr;
		QGroupBox* _sec_C_RGroup = nullptr;
		QGroupBox* _sec_B_RGroup = nullptr;

		QPushButton* _play_CGroupButton = nullptr;

		QPushButton* _console_LGroupButton = nullptr;

		QPushButton* _stat_RGroupButton = nullptr;
		QPushButton* _conf_RGroupButton = nullptr;

};