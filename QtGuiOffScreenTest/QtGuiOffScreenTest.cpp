#include "QtGuiOffScreenTest.h"

#include <chrono>

#include <QtTestUI.h>
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include <QOpenGLPaintDevice>
#include <QScreen>

QtGuiOffScreenTest::QtGuiOffScreenTest(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	float fScale = 2.0f;

	// UI in QGraphicsScene
	{
		QtTestUI *pTestUI = new  QtTestUI();
		pTestUI->move(0, 0);

		m_Scene1.addWidget(pTestUI);
		ui.graphicsView1->setScene(&m_Scene1);
	}

	// draw UI with QGraphicsScene
	{
		QtTestUI *pTestUI = new  QtTestUI();
		pTestUI->move(0, 0);

		m_OffscreenUI = QPixmap(fScale * pTestUI->width(), fScale * pTestUI->height());
		QGraphicsPixmapItem* pPixmap = m_Scene2a.addPixmap(m_OffscreenUI);
		ui.graphicsView2->setScene(&m_Scene2a);
		ui.graphicsView2->scale(1.0f/fScale, 1.0f / fScale);
		m_Scene2.addWidget(pTestUI);

		QObject::connect(&m_Scene2, &QGraphicsScene::changed, [this, pPixmap](const QList<QRectF>& vRect) {
			auto tpNow = std::chrono::high_resolution_clock::now();

			QPainter qPainter(&m_OffscreenUI);
			m_Scene2.render(&qPainter);
			qPainter.end();
			pPixmap->setPixmap(m_OffscreenUI);

			qDebug() << "UI rendering time: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - tpNow).count() << "ms";
		});
	}

	// draw UI with QGraphicsScene as OpenGL texture
	{
		QtTestUI *pTestUI = new  QtTestUI();
		pTestUI->move(0, 0);

		ui.openGLWidget->m_fScale = fScale;
		// *96 / pTestUI->logicalDpiY();
		ui.openGLWidget->setWidget(pTestUI);
	}
}
