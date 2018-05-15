#pragma once

#include <QOpenGLFunctions_4_1_Compatibility>
#include <QOpenGLWidget>
#include <QOpenGLDebugLogger>
#include <QOpenGLFramebufferObject>
#include <QGraphicsScene>

class QtOpenGL : public QOpenGLWidget, public QOpenGLFunctions_4_1_Compatibility
{
	Q_OBJECT

public:
	QtOpenGL(QObject *parent);

	virtual void initializeGL() Q_DECL_OVERRIDE;
	virtual void paintGL() Q_DECL_OVERRIDE;

	virtual void  mouseMoveEvent(QMouseEvent* pEvent);
	virtual void  mousePressEvent(QMouseEvent* pEvent);
	virtual void  mouseReleaseEvent(QMouseEvent* pEvent);

	void setTexture(GLuint uId)
	{
		m_glTexture = uId;
	}

	void setWidget(QWidget* pWidget);

public:
	QOpenGLDebugLogger*	m_pLogger;
	QGraphicsScene		m_Scene;

	QPoint				m_lastMousePos;
	Qt::MouseButtons	m_lastMouseButtons;

	GLuint	m_glVertBuffer;
	GLuint	m_glIndexBuffer;
	GLuint	m_glTexture;

	float	m_fScale;
	QSize	m_vSize;
	QOpenGLFramebufferObject*	m_pFBO;
};
