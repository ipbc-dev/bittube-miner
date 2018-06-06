#pragma once

#include <QWidget>
#include <QGroupBox>
#include <QTextEdit>

class StatsPanel : public QWidget {
	Q_OBJECT

	public:
		explicit StatsPanel(QWidget* parent = 0);
		virtual ~StatsPanel();

	private:
		QPalette * _backgroundColor = nullptr;

		QGroupBox* _main_C_Group = nullptr;
		QTextEdit* _outputText = nullptr;


};