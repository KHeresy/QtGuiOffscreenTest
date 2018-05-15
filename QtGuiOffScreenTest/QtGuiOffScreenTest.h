#pragma once

#include <QtWidgets/QWidget>
#include "ui_QtGuiOffScreenTest.h"

#include <QGraphicsScene>

class QtGuiOffScreenTest : public QWidget
{
	Q_OBJECT

public:
	QtGuiOffScreenTest(QWidget *parent = Q_NULLPTR);

private:
	Ui::QtGuiOffScreenTestClass ui;

	QGraphicsScene	m_Scene1;
	QGraphicsScene	m_Scene2;
	QGraphicsScene	m_Scene2a;

	QPixmap			m_OffscreenUI;
};
