#pragma once

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
		QPalette * _backgroundColor = nullptr;
		
		QVBoxLayout* _mainlayout = nullptr;
		QGroupBox* _main_C_Group = nullptr;
		QTextEdit* _outputText = nullptr;
};