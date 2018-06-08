#pragma once

#include <QFrame>
#include <QWidget>

class ConfigPanel : public QWidget {
	Q_OBJECT

	public:
		explicit ConfigPanel(QWidget* parent = 0) : QWidget(parent) {};
		virtual ~ConfigPanel() {};

	private:
		QFrame * _main_T_Frame = nullptr;
		QFrame* _main_B_Frame = nullptr;

};