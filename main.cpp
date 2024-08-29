#include "ImageProcessing.h"

#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	//load qss
	//QString appDirPath = QCoreApplication::applicationDirPath();
	QFile qss(QCoreApplication::applicationDirPath() + "/Resources/qss/style.qss");
	QString qssPath = qss.fileName();
	if (qss.open(QFile::ReadOnly)) {
		qDebug("load qss success");
		QString style = QLatin1String(qss.readAll());
		a.setStyleSheet(style);
		qss.close();
	}
	else {
		qDebug("load qss failed");
	}

	ImageProcessing w;
	w.show();
	return a.exec();
}
