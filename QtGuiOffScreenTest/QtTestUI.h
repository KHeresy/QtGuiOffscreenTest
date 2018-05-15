#pragma once

#include <QWidget>
#include <QDebug>

#include "ui_QtTestUI.h"

class QtTestUI : public QWidget
{
	Q_OBJECT

public:
	QtTestUI(QWidget *parent = Q_NULLPTR);
	~QtTestUI();

private:
	Ui::QtTestUI ui;
};
