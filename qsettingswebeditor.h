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
	explicit QSettingsWebEditor(QObject* parent = NULL, int port = 8080);
	~QSettingsWebEditor();

protected:
	QString m_org;
	QString m_app;
	QHttpServer m_server;
	int m_portNumber;
	
private:
	QHttpResponse *m_resp;

public:
	QString generateSettingsPage(QString organization, QString application);
	void setPort(int port = 8080);

public slots:
	void onHttpRequest(QHttpRequest *req, QHttpResponse *resp);
	void onHttpRequestEnd();
	void onHttpRequestData(const QByteArray &data);

signals:
	//

};

#endif // QSETTINGSWEBEDITOR_H
