#pragma once

#include <QWidget>

class ConfigPanel : public QWidget {
	Q_OBJECT

public:
	explicit ConfigPanel(QWidget* parent = 0) : QWidget(parent) {};
	virtual ~ConfigPanel() {};


};