#pragma once

#include <QFrame>
#include <QWidget>


#include <QVBoxLayout>
#include <QGroupBox>
#include <QTextEdit>


class ConsolePanel : public QWidget {
	Q_OBJECT

	public:
		explicit ConsolePanel(QWidget* parent = 0);
		virtual ~ConsolePanel();

	private:
		QFrame* _main_T_Frame = nullptr;
		QFrame* _main_B_Frame = nullptr;

		QTextEdit* _outputText = nullptr;
	
};