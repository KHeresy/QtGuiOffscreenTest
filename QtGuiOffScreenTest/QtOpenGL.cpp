#include <array>
#include <chrono>

#include "QtOpenGL.h"

#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QOpenGLPaintDevice>
#include <QPainter>

QtOpenGL::QtOpenGL(QObject *parent)
{
	m_fScale = 1.0f;
	setMouseTracking(true);

	QObject::connect(&m_Scene, &QGraphicsScene::changed, [this](const QList<QRectF>& vRect) {
		auto tpNow = std::chrono::high_resolution_clock::now();

		makeCurrent();
		m_pFBO->bind();
		QOpenGLPaintDevice device(m_pFBO->size());
		//device.setDevicePixelRatio(0.5);
		QPainter painter(&device);
		m_Scene.render(&painter);
		m_pFBO->release();

		setTexture(m_pFBO->texture());
		update();

		qDebug() << "OpenGL UI rendering time: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - tpNow).count() << "ms";
	});
}

void QtOpenGL::mouseMoveEvent(QMouseEvent * pEvent)
{
	QPoint pos = pEvent->pos();
	pos.setX(m_vSize.width() * pos.x() / width() / m_fScale);
	pos.setY(m_vSize.height() * pos.y() / height() / m_fScale);
	QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseMove);
	mouseEvent.setWidget(NULL);
	mouseEvent.setPos(pos);
	mouseEvent.setScenePos(pos);
	mouseEvent.setScreenPos(pos);
	mouseEvent.setLastPos(m_lastMousePos);
	mouseEvent.setLastScenePos(m_lastMousePos);
	mouseEvent.setLastScreenPos(m_lastMousePos);

	mouseEvent.setButtons(pEvent->buttons());
	mouseEvent.setButton(Qt::NoButton);
	mouseEvent.setModifiers(0);
	mouseEvent.setAccepted(false);
	m_lastMousePos = pos;

	QApplication::sendEvent(&m_Scene, &mouseEvent);
}

void QtOpenGL::mousePressEvent(QMouseEvent * event)
{
	Qt::MouseButton button = Qt::LeftButton;
	m_lastMouseButtons |= button;

	QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMousePress);
	mouseEvent.setWidget(NULL);
	mouseEvent.setPos(m_lastMousePos);
	mouseEvent.setButtonDownPos(button, m_lastMousePos);
	mouseEvent.setButtonDownScenePos(button, m_lastMousePos);
	mouseEvent.setButtonDownScreenPos(button, m_lastMousePos);
	mouseEvent.setScenePos(m_lastMousePos);
	mouseEvent.setScreenPos(m_lastMousePos);
	mouseEvent.setLastPos(m_lastMousePos);
	mouseEvent.setLastScenePos(m_lastMousePos);
	mouseEvent.setLastScreenPos(m_lastMousePos);
	mouseEvent.setButtons(m_lastMouseButtons);
	mouseEvent.setButton(button);
	mouseEvent.setModifiers(0);
	mouseEvent.setAccepted(false);

	QApplication::sendEvent(&m_Scene, &mouseEvent);
}

void QtOpenGL::mouseReleaseEvent(QMouseEvent * event)
{
	Qt::MouseButton button = Qt::LeftButton;
	m_lastMouseButtons &= ~button;

	QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseRelease);
	mouseEvent.setWidget(NULL);
	mouseEvent.setPos(m_lastMousePos);
	mouseEvent.setScenePos(m_lastMousePos);
	mouseEvent.setScreenPos(m_lastMousePos);
	mouseEvent.setLastPos(m_lastMousePos);
	mouseEvent.setLastScenePos(m_lastMousePos);
	mouseEvent.setLastScreenPos(m_lastMousePos);
	mouseEvent.setButtons(m_lastMouseButtons);
	mouseEvent.setButton(button);
	mouseEvent.setModifiers(0);
	mouseEvent.setAccepted(false);

	QApplication::sendEvent(&m_Scene, &mouseEvent);
}

void QtOpenGL::setWidget(QWidget * pWidget)
{
	m_Scene.addWidget(pWidget);
	m_vSize.setWidth(m_fScale * pWidget->width());
	m_vSize.setHeight(m_fScale * pWidget->height());
}

void QtOpenGL::initializeGL()
{
	initializeOpenGLFunctions();

	m_pLogger = new QOpenGLDebugLogger(this);
	connect(m_pLogger, &QOpenGLDebugLogger::messageLogged, [](QOpenGLDebugMessage message) {
		auto s = message.message();
		auto st = message.severity();
		//qDebug() << message;
	});

	if (m_pLogger->initialize())
	{
		m_pLogger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
		m_pLogger->enableMessages();
	}

	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// Populate a vertex buffer
	GLsizei uVertextDataSize = 5 * sizeof(float);

	std::array<float, 20> aVertexArray = {
		-1, -1, 0, 0, 0,
		-1, 1, 0, 0, 1,
		1, 1, 0, 1, 1,
		1, -1, 0, 1, 0
	};

	std::array<uint16_t, 6> aIndexArray = {
		0, 1, 2,
		0, 2, 3
	};

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &m_glTexture);

	glGenBuffers(1, &m_glVertBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_glVertBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aVertexArray.size(), aVertexArray.data(), GL_STATIC_DRAW);

	glVertexPointer(3, GL_FLOAT, uVertextDataSize, 0);
	glTexCoordPointer(2, GL_FLOAT, uVertextDataSize, (void*)(3 * sizeof(float)));

	glGenBuffers(1, &m_glIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * aIndexArray.size(), aIndexArray.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_pFBO = new QOpenGLFramebufferObject(m_vSize);

}

void QtOpenGL::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float m_fPixelRatio = devicePixelRatioF();
	glViewport(0, 0, m_fPixelRatio * width(), m_fPixelRatio * height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1, 1, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, m_glTexture);

	glBindBuffer(GL_ARRAY_BUFFER, m_glVertBuffer);
	glVertexPointer(3, GL_FLOAT, 5 * sizeof(float), 0);
	glTexCoordPointer(2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIndexBuffer);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}
