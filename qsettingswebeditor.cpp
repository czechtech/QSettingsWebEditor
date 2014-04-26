#include "qsettingswebeditor.h"

#include <QtXml>

QSettingsWebEditor::QSettingsWebEditor(QObject* parent, int port)
	: QObject(parent)
{
	m_portNumber = port;
	
	m_org = QCoreApplication::organizationName();
	m_app = QCoreApplication::applicationName();
	if(m_org == "") m_org = QCoreApplication::organizationDomain(); // For Mac - http://qt-project.org/doc/qt-4.8/qcoreapplication.html#organizationName-prop
	
	connect(&m_server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)), this, SLOT(onHttpRequest(QHttpRequest*, QHttpResponse*)));
	
	// Start Listening:
	m_server.listen(m_portNumber);
	// TODO: Check that this is successful
	qDebug() << "QSettingsWebEditor: Listening on port #" << m_portNumber;
}


QSettingsWebEditor::~QSettingsWebEditor()
{
	m_server.close();
}


// Public:
void QSettingsWebEditor::setPort(int port)
{
	if(m_portNumber != port) {
		// TODO: server needs to be deleted and started over
		m_server.listen(port);
		m_portNumber = port;
		qDebug() << "QSettingsWebEditor: Now listening on port #" << m_portNumber;
	}
}


// Public Slot:
void QSettingsWebEditor::onHttpRequest(QHttpRequest *req, QHttpResponse *resp)
{
	// URL's To Ignore:
	if(req->path().contains("favicon.ico", Qt::CaseInsensitive)) {
		resp->setHeader("Content-Length", "0");
		resp->writeHead(404); // Not Found
		resp->end();
		return;
	}
	
	if(req->method() == QHttpRequest::HTTP_POST) {
		connect(req, SIGNAL(data(const QByteArray&)), this, SLOT(onHttpRequestData(const QByteArray&)));
	}

	connect(req, SIGNAL(end()), this, SLOT(onHttpRequestEnd()));
	
	m_resp = resp;
}


// Public Slot:
void QSettingsWebEditor::onHttpRequestEnd()
{
	if(!m_resp) {
		qDebug() << "QSettingsWebEditor: HTTP Request Ended With Null Response Pointer";
		return;
	}
	
	QString html = generateSettingsPage(m_org, m_app);

	m_resp->setHeader("Content-Length", QString::number(html.length()));
	m_resp->writeHead(200); // OK
	m_resp->write(html.toUtf8());
	m_resp->end();
	
	m_resp = 0; // QHttpServer manages the deletion
}


// Public Slot:
void QSettingsWebEditor::onHttpRequestData(const QByteArray &reqData)
{
	// TODO: Wait until QHttpRequest::end() signal to guarantee all data has been received?
	
	QString data(reqData);
	
	if(!data.contains("organization=") || !data.contains("application=")) {
		// TODO: this is not a bullet-proof test for valid request data
		qDebug() << "onHttpRequestData called without specifying organization and application in request data";
		return;
	}
	
	// Split Data Into key,value Pairs
	QStringList dataList = data.split("&");
	QMap<QString, QString> map;
	for(int i = 0; i < dataList.length(); i++) {
		QString str = dataList[i];
		int l = str.indexOf("=");
		int r = str.length() - l - 1;
		QString key = QUrl::fromPercentEncoding(str.left(l).toAscii());
		QString value = QUrl::fromPercentEncoding(str.right(r).toAscii());
		if(l > 1) {
			map[key] = value;
		}
		else {
			qDebug() << "Malformed key,value pair found " << str;
		}
	}
	
	// Parse Organization & Application
	QString org = map.take("organization");
	QString app = map.take("application");
	
	if(org != m_org || app != m_app) {
		qDebug() << "QSettingsWebEditor: Can only change settings of active application: " << m_org << m_app;
		return;
	}
	
	if(map.size() < 1) {
		qDebug() << "No settings passed to onHttpRequestData";
		return;
	}
	
	QSettings s(org, app);
	
	if(!s.isWritable()) {
		qDebug() << "Could not write new settings for " << org << "'s " << app;
		return;
	}

	// Update/Delete Settings
	QStringList keys = s.allKeys();
	int keyCount = keys.size();
	for(int i = 0; i < keyCount; i++) {
		QString key = keys[i];
		if(!map.contains(key)) {
			s.remove(key);
		}
		else if(s.value(key) != map.value(key)) {
			s.setValue(key, map.value(key)); // TODO: Handle Other QVariant Types
			map.remove(key);
		}
		else { // No change
			map.remove(key);
		}
	}
	
	// Add New Settings
	if(map.size() != 0) {
		qDebug() << "Found new settings: " << map;
		QMap<QString, QString>::const_iterator i = map.constBegin();
		while(i != map.constEnd()) {
			s.setValue(i.key(), i.value());
			i++;
		}
	}
}


// Public:
QString QSettingsWebEditor::generateSettingsPage(QString organization, QString application)
{
	QDomDocument doc("html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\"");
	QDomElement html = doc.createElement("html");
	html.setAttribute("xmlns", "http://www.w3.org/1999/xhtml");
	html.setAttribute("xml:lang", "en");
	doc.appendChild(html);

	// Head
	QDomElement head  = doc.createElement("head");
	QDomElement title = doc.createElement("title");
	title.appendChild(doc.createTextNode("QSettingsWebEditor: " + organization + "/" + application));
	html.appendChild(head);
	head.appendChild(title);

	// Body
	QDomElement body = doc.createElement("body");
	html.appendChild(body);
	QDomElement h1 = doc.createElement("h1");
	body.appendChild(h1);
	h1.appendChild(doc.createTextNode(organization + ":" + application));

	// Start Form
	QDomElement form = doc.createElement("form");
	form.setAttribute("action", "index.html");
	form.setAttribute("method", "post");
	body.appendChild(form);
	
	// Table
	QDomElement table = doc.createElement("table");
	table.setAttribute("border", "2");
	form.appendChild(table);
	QDomElement hrow  = doc.createElement("tr");
	QDomElement hset  = doc.createElement("th");
	QDomElement hval  = doc.createElement("th");
	table.appendChild(hrow);
	hrow.appendChild(hset);
	hrow.appendChild(hval);
	hset.appendChild(doc.createTextNode("Setting"));
	hval.appendChild(doc.createTextNode("Value"));

	// Rows
	QSettings s(organization, application);
	QStringList keys = s.allKeys();
	int keyCount = keys.size();
	QDomElement h2 = doc.createElement("h2");
	h2.appendChild( doc.createTextNode(QString::number(keyCount) + " Setting(s)") );
	body.appendChild(h2);
	for(int j = 0; j < keyCount; j++) {
		QDomElement r   = doc.createElement("tr");
		QDomElement set = doc.createElement("td");
		QDomElement val = doc.createElement("td");
		table.appendChild(r); r.appendChild(set); r.appendChild(val);

		QString k = keys[j];
		set.appendChild(doc.createTextNode(k));
		QVariant v = s.value(k);
		
		QVariant::Type t = v.type();
		if(t == QVariant::String || t == QVariant::UInt || t == QVariant::Int) {
			QDomElement i = doc.createElement("input");
			val.appendChild(i);
			i.setAttribute("type","text");
			i.setAttribute("name", k);
			i.setAttribute("value", v.toString() );
		} else if(t == QVariant::Bool) {
			QDomElement iFalse = doc.createElement("input");
			QDomElement iTrue  = doc.createElement("input");
			val.appendChild(iTrue);
			val.appendChild(iFalse);
			iTrue.setAttribute("type","radio");
			iTrue.setAttribute("name", k);
			iTrue.setAttribute("value", "true");
			if(v.toBool()) { iTrue.setAttribute("checked", "checked"); }
			iFalse.setAttribute("type","radio");
			iFalse.setAttribute("name", k);
			iFalse.setAttribute("value", "false");
			if(!v.toBool()) { iTrue.setAttribute("checked", "checked"); }
		} else {
			// TODO: Handle Other QVariant Types
			val.appendChild(doc.createTextNode( s.value(k).toString() ));
			qDebug() << "Could not present QVariant " << v;
		}
	}

	QDomElement p = doc.createElement("p");
	form.appendChild(p); 

	// Organization & Application
	QDomElement org = doc.createElement("input");
	QDomElement app = doc.createElement("input");
	org.setAttribute("type","hidden");
	app.setAttribute("type","hidden");
	org.setAttribute("name","organization");
	app.setAttribute("name","application");
	org.setAttribute("value",organization);
	app.setAttribute("value",application);
	p.appendChild(org); p.appendChild(app);

	// Submit Button
	QDomElement submit = doc.createElement("input");
	submit.setAttribute("type", "submit");
	submit.setAttribute("value", "Update");
	p.appendChild(submit);

	// TODO: Form for Adding a New Setting?

	// TODO: Form for Deleting a Setting?


	return doc.toString();
}
