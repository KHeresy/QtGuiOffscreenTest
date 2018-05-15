#include "QtGuiOffScreenTest.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QSurfaceFormat glFormat;
	glFormat.setVersion(4, 1);
	glFormat.setProfile(QSurfaceFormat::CompatibilityProfile);
	glFormat.setSwapInterval(0);
	QSurfaceFormat::setDefaultFormat(glFormat);

	QApplication a(argc, argv);
	QtGuiOffScreenTest w;
	w.show();
	
	return a.exec();
}
