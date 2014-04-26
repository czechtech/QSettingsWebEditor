QSettingsWebEditor
==================

Web Access to QSettings

Projects that use an Arduino, Raspberry Pi, Beagle Board, etc. often have a couple settings that might need to be configured once or twice in the device's lifetime. Adding extra hardware (LCD, buttons, keypad, etc.) and code for rare adjustments is expensive in time and money. Most of these embed-able computers have an ethernet port and the device is already on the network.

This is a test project to explore creating a web application for adjusting an application's settings.

In a way, this is a web version of the [SettingsEditor](http://qt-project.org/doc/qt-4.8/tools-settingseditor.html).

Installation
------------

QSettingsWebEditor relies heavily on [QHttpServer by Nikhil Marathe](https://github.com/nikhilm/qhttpserver/).

    wget https://github.com/nikhilm/qhttpserver/archive/master.zip
    gunzip master.zip
    cd qhttpserver-master
    qmake -r PREFIX=/usr
    make && su -c 'make install'
    cd ..
    rm master.zip

Download the source and compile it.

    wget https://github.com/czechtech/qsettingswebeditor/archive/master.zip
    gunzip master.zip
    cd qsettingswebeditor-master
    qmake && make && su -c 'make install'

Usage
-----

QSettingsWebEditor is designed to be a instantiated within the main class of the application or alongside the QCoreApplication.

	int main(int argc, char *argv[])
	{
		QCoreApplication a(argc, argv);
		...
		QSettingsWebEditor(8080);
		...
		QCoreApplication::setOrganizationName("CzechTech");
		QCoreApplication::setApplicationName("QSettingsWebEditor");
		...
		return a.exec();
	}

Be sure to set the QCoreApplication::organizationName and QCoreApplication::applicationName as these determine which settings will be accessible.

While the QSettingsWebEditor exists, it will handle the web ui for the user to change settings.

Notes
-----

Usually only an admin account can listen on port 80 of a computer.  So, don't set QSettingsWebEditor's port number to the standard web server port number 80 unless it is running as root.

QSettingsWebEditor is ideally designed for an application that utilized the QSharedSettings library.  See [QSharedSettings](https://github.com/czechtech/libqsharedsettings/) for more information about this.

Demo
----

Requires the QSharedSettings library.

    wget https://github.com/czechtech/libqsharedsettings/archive/master.zip
    gunzip master.zip
    cd libqsharedsettings-master
    qmake && make && su -c 'make install'
    cd ..
    rm master.zip

Compile

	qmake && make

Launch the QSettingsWebEditor program:

    LD_LIBRARY_PATH=../:$LD_LIBRARY_PATH ./qsettingswebeditor

This needs to be continually running.

Now, on another screen or window or computer or etc, direct a web browser to the ip of the computer running QSettingsWebEditor

    http://localhost:8080

Add to the URL the organization and application name of any Qt program that uses QSettings. Example:

    http://localhost:8080/czechtech/qsettingswebeditor/

You will see the settings associated with the QSettingsWebEditor (the port it listens on).  You can now make any changes. The URL path must be of the form organization/application/  Without the last /, many web browsers will assume "application" is the web page, and when the form is submitted, it will take you to organization/index.html

To quit QSettingsWebEditor, use CTRL-C to interrupt it.  If there's a "Segmentation Fault" upon exiting, this is a [known issue](https://github.com/nikhilm/qhttpserver/issues) with the QHttpServer library.

The user account that launches QSettingsWebEditor determines what application settings are accessible.

Typically, an application only reads its settings when started, and saves them when it closes. Any application's settings you change will likely need to be closed and restarted for the settings to take effect.  Hopefully upon exit the application does not overwrite your changes.

If a bad port number is set for QSettingsWebEditor, edit or delete the file ~/.config/CzechTech/QSettingsWebEditor.conf

**NOTE: QSettingsWebEditor is NOT secure! DO NOT use it on any production machine**

To Do
-----

- Design the Demo code to not depend on QSharedSettings,
- Consider puttings QSettingsWebEditor in its own thread,
- Better utilize and display [QSettings Fallback](http://qt-project.org/doc/qt-4.8/qsettings.html#fallback-mechanism),
- Maintain an open connection with the webbrowser so Settings can be updated when QSharedSettings signals, and
- Many many many "TODO" notes within the code.

The code functions well, but it could use some enhancements and bullet-proofing.

Uses
----

- QProPresentText

If you use this in a project drop, a line to have your project linked here.
