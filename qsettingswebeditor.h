#ifndef QSETTINGSWEBEDITOR_H
#define QSETTINGSWEBEDITOR_H

#include <QObject>
#include <QCoreApplication>
#include <QtXml>
#include <QEventLoop>
#include <QUrl>
#include <qsharedsettings.h>
#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>

#include <QDebug>

using namespace std;

class QSettingsWebEditor : public QObject
{
	Q_OBJECT

public:
	explicit QSettingsWebEditor(QObject* parent = NULL);
	~QSettingsWebEditor();

protected:
	//
	
private:
	QSharedSettings settings;
	QHttpServer server;
	int portNumber;

	QString generateSettingsPage(QString organization, QString application);
	QString generateIndexPage();


public:
	//

public slots:
	void onSettingsChanged(QStringList keys);
	void onHttpRequest(QHttpRequest *req, QHttpResponse *resp);
	void onHttpRequestEnd();
	void onHttpRequestData(const QByteArray &data);

signals:
	//

};

#endif // QSETTINGSWEBEDITOR_H
