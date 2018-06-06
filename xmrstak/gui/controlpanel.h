#pragma once

#include <QWidget>
#include <QGroupBox>
#include <QPushButton>

class ControlPanel : public QWidget {
	Q_OBJECT

	public:
		explicit ControlPanel(QWidget* parent = 0);
		virtual ~ControlPanel();

	private:
		QPalette* _backgroundColor = nullptr;

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