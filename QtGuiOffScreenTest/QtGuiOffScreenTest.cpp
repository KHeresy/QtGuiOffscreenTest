#include "QtGuiOffScreenTest.h"

#include <chrono>

#include <QtTestUI.h>
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include <QOpenGLPaintDevice>
#include <QGraphicsSceneMouseEvent>
#include <QScreen>

class CGraphicsPixmapItemExt : public QGraphicsPixmapItem
{
public:
	QGraphicsScene&	m_Scene;
	float			m_fScale;

public:
	CGraphicsPixmapItemExt(QGraphicsScene& rScene, QGraphicsItem *parent = Q_NULLPTR) : QGraphicsPixmapItem(parent), m_Scene(rScene)
	{
		setAcceptHoverEvents(true);
		m_fScale = 1.0f;
	}

protected:
	template<typename TPointType, typename TPointTypeSrc>
	TPointType calcPoint(const TPointTypeSrc& rPoint)
	{
		return TPointType(rPoint.x() / m_fScale, rPoint.y() / m_fScale);
	}

	QGraphicsSceneMouseEvent* generateEvent(const QGraphicsSceneMouseEvent* pEvent)
	{
		QGraphicsSceneMouseEvent* pNewEvent = new QGraphicsSceneMouseEvent(pEvent->type());
		pNewEvent->setWidget(nullptr);

		QPoint vPos = calcPoint<QPoint>(pEvent->pos());
		pNewEvent->setPos(vPos);
		pNewEvent->setButtonDownPos(pEvent->button(), vPos);
		pNewEvent->setButtonDownScenePos(pEvent->button(), vPos);
		pNewEvent->setButtonDownScreenPos(pEvent->button(), vPos);
		pNewEvent->setScenePos(calcPoint<QPointF>(pEvent->scenePos()));
		pNewEvent->setScreenPos(calcPoint<QPoint>(pEvent->scenePos()));

		pNewEvent->setLastPos(calcPoint<QPointF>(pEvent->lastPos()));
		pNewEvent->setLastScenePos(calcPoint<QPointF>(pEvent->lastScenePos()));
		pNewEvent->setLastScreenPos(calcPoint<QPoint>(pEvent->lastScreenPos()));

		pNewEvent->setButtons(pEvent->buttons());
		pNewEvent->setButton(pEvent->button());
		pNewEvent->setModifiers(0);
		pNewEvent->setAccepted(false);

		return pNewEvent;
	}

	virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* pEvent) Q_DECL_OVERRIDE
	{
		QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseMove);
		mouseEvent.setWidget(nullptr);
		mouseEvent.setPos(calcPoint<QPointF>(pEvent->pos()));
		mouseEvent.setScenePos(calcPoint<QPointF>(pEvent->scenePos()));
		mouseEvent.setScreenPos(calcPoint<QPoint>(pEvent->screenPos()));
		mouseEvent.setLastPos(calcPoint<QPointF>(pEvent->lastPos()));
		mouseEvent.setLastScenePos(calcPoint<QPointF>(pEvent->lastScenePos()));
		mouseEvent.setLastScreenPos(calcPoint<QPoint>(pEvent->lastScreenPos()));

		mouseEvent.setButtons(Qt::NoButton);
		mouseEvent.setButton(Qt::NoButton);
		mouseEvent.setModifiers(0);
		mouseEvent.setAccepted(false);
		QApplication::sendEvent(&m_Scene, &mouseEvent);
	}

	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* pEvent) Q_DECL_OVERRIDE
	{
		QApplication::sendEvent(&m_Scene, generateEvent(pEvent));
	}

	virtual void mousePressEvent(QGraphicsSceneMouseEvent* pEvent) Q_DECL_OVERRIDE
	{
		QApplication::sendEvent(&m_Scene, generateEvent(pEvent));
	}
	
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent) Q_DECL_OVERRIDE
	{
		QApplication::sendEvent(&m_Scene, generateEvent(pEvent));
	}
};

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
		m_Scene2.addWidget(pTestUI);

		CGraphicsPixmapItemExt* pPixmap = new CGraphicsPixmapItemExt(m_Scene2);
		m_OffscreenUI = QPixmap(fScale * pTestUI->width(), fScale * pTestUI->height());
		pPixmap->setPixmap(m_OffscreenUI);
		pPixmap->m_fScale = fScale;
		m_Scene2a.addItem(pPixmap);
		//QGraphicsPixmapItem* pPixmap = m_Scene2a.addPixmap(m_OffscreenUI);
		ui.graphicsView2->setScene(&m_Scene2a);
		ui.graphicsView2->scale(1.0f/fScale, 1.0f / fScale);

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
