#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>

namespace Ui {
	class MainWindow;
}


class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

private:
	QWidget* _window = nullptr;
	QVBoxLayout* mainLayout = nullptr;
	QImage* _mainBackgroundImg = nullptr;
	QPalette* _mainBackgroundPalette = nullptr;
	
	// Main section
	QHBoxLayout *contentLayout = nullptr;
	QWidget *contentWidget = nullptr;
	QHBoxLayout *innerContentLayout = nullptr;

	QTextEdit *outputText = nullptr;
	//---

	// Footer section
	QHBoxLayout *footerLayout = nullptr;
	QWidget *footerWidget = nullptr;
	QHBoxLayout *innerFooterLayout = nullptr;

	QPushButton* statsButton = nullptr;
	QPushButton* startButton = nullptr;
	//---
};