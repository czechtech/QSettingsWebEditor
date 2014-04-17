#include "qsettingswebeditor.h"

QSettingsWebEditor::QSettingsWebEditor(QObject* parent)
	: QObject(parent), settings("CzechTech", "QSettingsWebEditor")
{
	// Load Port Number Setting:
	if(!settings.contains("Port")) {
		qDebug() << "Establishing QSettings";
		settings.setValue("Port", 8080);
	}
	portNumber = settings.value("Port", 8080).toInt();
	
	// Signal/Slot Connections:
	connect(&settings, SIGNAL(settingChanged(QString)),
	             this, SLOT(onSettingChanged(QString)));
	connect(&server, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
	           this, SLOT(onHttpRequest(QHttpRequest*, QHttpResponse*)));
	
	// Start Listening:
	qDebug() << "Listening on port #" << portNumber;
	server.listen(portNumber);
}


QSettingsWebEditor::~QSettingsWebEditor()
{
	server.close();
}


// Public Slot:
void QSettingsWebEditor::onSettingChanged(QString key)
{
	if(key != "Port") return;

	int p = settings.value("Port", 8080).toInt();
	if(portNumber != p) {
		// TODO: server needs to be deleted and started over
		server.listen(p);
		portNumber = p;
		qDebug() << "Now listening on port #" << portNumber;
	}
}


// Public Slot:
void QSettingsWebEditor::onHttpRequest(QHttpRequest *req, QHttpResponse *resp)
{
	connect(req, SIGNAL(end()),
	       this, SLOT(onHttpRequestEnd()));
	connect(req, SIGNAL(data(const QByteArray&)),
	       this, SLOT(onHttpRequestData(const QByteArray&)));
	
	QString path = req->path();

	// Requests To Ignore:
	if(path.contains("favicon.ico", Qt::CaseInsensitive)) {
		return;
	}

	QString html;

	QString org = path.section("/", 1, 1);
	QString app = path.section("/", 2, 2);
	if(app.contains(".") && path.count("/") < 3) { // TODO: Better URL checking
		app = "";
	}
	// TODO: Check for valid qsettings?
	if( !org.isEmpty() && !app.isEmpty() ) {
		
		if(req->method() == QHttpRequest::HTTP_POST) {
			// TODO: wait for the data to be processed
		}
		
		html = generateSettingsPage(org, app);
	}
	else {
		html = generateIndexPage();
	}
	
	resp->setHeader("Content-Length", QString::number(html.length()));
	resp->writeHead(200); // everything is OK
	resp->write(html.toUtf8());
	resp->end();
}


// Public Slot:
void QSettingsWebEditor::onHttpRequestEnd()
{
	//qDebug() << "onHttpRequestEnd()";
}


// Public Slot:
void QSettingsWebEditor::onHttpRequestData(const QByteArray &reqData)
{
	// TODO: Wait until QHttpRequest::end() signal to guarantee all data has been received
	
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
	// TODO: Security hole if a qSettings key is "organization" or "application"
	if(org.length() == 0 || app.length() == 0) {
		qDebug() << "onHttpRequestData could not parse organization and application from request data";
		qDebug() << reqData;
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
			s.setValue(key, map.value(key));
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


// Private:
QString QSettingsWebEditor::generateIndexPage()
{
	QDomDocument doc("html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\"");
	QDomElement html = doc.createElement("html");
	html.setAttribute("xmlns", "http://www.w3.org/1999/xhtml");
	html.setAttribute("xml:lang", "en");
	doc.appendChild(html);

	// Head
	QDomElement head  = doc.createElement("head");
	QDomElement title = doc.createElement("title");
	title.appendChild(doc.createTextNode("QSettingsWebEditor"));
	html.appendChild(head);
	head.appendChild(title);

	// Body
	QDomElement body = doc.createElement("body");
	html.appendChild(body);
	QDomElement h1 = doc.createElement("h1");
	body.appendChild(h1);
	
	QString helpText = "URL Path must be of the form organization/application/"; // TODO: Make this more informative
	
	h1.appendChild(doc.createTextNode(helpText));

	// TODO: present a form for the user to type in the OrganizationName & ApplicationName
	// TODO: present a list of links to all the accessible OrganizationName/ApplicationName

	return doc.toString();}


// Private:
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
		} else {
			// TODO: Handle Other QVariant Types
			val.appendChild(doc.createTextNode( settings.value(k).toString() ));
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
