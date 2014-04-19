QSettingsWebEditor
==================

Web Application for Editing QSettings

Projects that use an Arduino, Raspberry Pi, Beagle Board, etc. often have a couple settings that might need to be configured once or twice in the device's lifetime. Adding extra hardware (LCD, buttons, keypad, etc.) and code for rare adjustments is expensive in time and money. Most of these embed-able computers have an ethernet port and the device is already on the network.

This is a test project to explore creating a web application for adjusting a device's settings.

In a way, this is a web version of the [SettingsEditor](http://qt-project.org/doc/qt-4.8/tools-settingseditor.html).

**NOTE: QSettingsWebEditor is NOT secure! DO NOT use it on any production machine**

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

Requires the QSharedSettings library.

    wget https://github.com/czechtech/libqsharedsettings/archive/master.zip
    gunzip master.zip
    cd libqsharedsettings-master
    qmake && make && su -c 'make install'
    cd ..
    rm master.zip

Download the source and compile it.

    wget https://github.com/czechtech/qsettingswebeditor/archive/master.zip
    gunzip master.zip
    cd qsettingswebeditor-master
    qmake && make && su -c 'make install'

Usage
-----

Launch the QSettingsWebEditor program:

    ./qsettingswebeditor

This needs to be continually running.

Now, on another screen or window or computer or etc, direct a web browser to the ip of the computer running QSettingsWebEditor

    http://localhost:8080

You should see the message "URL Path must be of the form organization/application/"

Now, add to the URL the organization and application name of any Qt program that uses QSettings. Example:

    http://localhost:8080/czechtech/qsettingswebeditor/

You will see the settings associated with the QSettingsWebEditor (the port it listens on).  You can now make any changes.

To quit QSettingsWebEditor, use CTRL-C to interrupt it.  If there's a "Segmentation Fault" upon exiting, this is a [known issue](https://github.com/nikhilm/qhttpserver/issues) with the QHttpServer library.

Because QSettingsWebEditor utilizes the QSharedSettings library (instead of just QSettings), the changes can occur immediately.  Any other application's settings you change will likely need to be closed and restarted for the settings to take effect.  See [QSharedSettings](https://github.com/czechtech/qsharedsettings/) for more information about this.

Notes
-----

The URL path must be of the form organization/application/  Without the last /, many web browsers will assume "application" is the web page, and when the form is submitted, it will take you to organization/index.html

Usually only an admin account can open port 80 on a computer.  So, don't set QSettingsWebEditor's port number to the standard web server port number 80 unless it is running as root.  If a bad port number is chosen, edit or delete the file ~/.config/CzechTech/QSettingsWebEditor.conf

Often times the web page renders quicker than the settings can be updated. You may find you have to refresh the page to see an update shown in the table.

The user account that launches QSettingsWebEditor determines what application settings are accessible.

To Do
-----

- Restructure code to be usable as a library with a test app,
- Add a constructor which accepts an "organization" and "application" to limit access (ignores path),
- Better utilize and display [QSettings Fallback](http://qt-project.org/doc/qt-4.8/qsettings.html#fallback-mechanism),
- Better utilize the a-synchronicity of QHttpServer,
- Add security mechanisms,
- Maintain an open connection with the webbrowser so Settings can be updated when QSharedSettings signals,
- Method for launching this as a background service,
- Allow code to compile and function without qsharedsettings library, and
- Many many many "TODO" notes within the code.

The code functions well, but it could use some enhancements and bullet-proofing.

Uses
----

- ProPresentText

If you use this in a project drop, a line to have your project linked here.
