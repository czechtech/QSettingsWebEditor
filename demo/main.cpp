#include <iostream>
#include <csignal>
#include <QCoreApplication>
#include "../qsettingswebeditor.h"

using namespace std;

QCoreApplication *a;

class PathParser: public QSettingsWebEditor // Must derive from QSettingsWebEditor to have access to m_org & m_app
{
    Q_OBJECT
public slots:
	void onRequest(QHttpRequest* req, QHttpResponse*) {
		QString path = req->path();
		if(path.count("/") > 1) {
			QString org = path.section("/", 1, 1);
			QString app = path.section("/", 2, 2);
			if( !org.isEmpty() && !app.isEmpty() ) {
				m_org = org;
				m_app = app;
				return;
			}
		}
		// TODO: Provide links to all accessible settings pages?
		m_org = "CzechTech"; m_app = "QSettingsWebEditor";
	}
	void onSettingsChanged(QStringList keys)
	{
		if(!keys.join(" ").contains("Port")) return;
		QSettings s("CzechTech", "QSettingsWebEditor");
		setPort(s.value("Port").toInt());
	}
public:
	explicit PathParser() : QSettingsWebEditor(NULL, 8080) {
		connect(&m_server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)), this, SLOT(onRequest(QHttpRequest*, QHttpResponse*)));
	}
};
// Public Slot:


void signalHandler(int) { cerr << "Terminating\n"; a->exit(0); }


int main(int argc, char *argv[])
{
  a = new QCoreApplication(argc, argv);
  signal(SIGINT,  signalHandler); signal(SIGTERM, signalHandler);

  QCoreApplication::setOrganizationName("CzechTech");
  QCoreApplication::setApplicationName("QSettingsWebEditor");

  QSharedSettings s;
  PathParser qswe;

  QObject::connect(&s, SIGNAL(settingsChanged(QStringList)), &qswe, SLOT(onSettingsChanged(QStringList)));

  return a->exec();
}

#include "main.moc" //http://stackoverflow.com/questions/5854626/qt-signals-and-slots-error-undefined-reference-to-vtable-for
