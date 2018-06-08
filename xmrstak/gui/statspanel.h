#pragma once

#include <QFrame>
#include <QWidget>


#include <QVBoxLayout>
#include <QGroupBox>
#include <QTextEdit>

class StatsPanel : public QWidget {
	Q_OBJECT

	public:
		explicit StatsPanel(QWidget* parent = 0);
		virtual ~StatsPanel();

	private:
		QFrame * _main_T_Frame = nullptr;
		QFrame* _main_B_Frame = nullptr;

		QTextEdit* _outputText = nullptr;

};