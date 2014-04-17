#include <iostream>
#include <csignal>
#include <QCoreApplication>
#include "qsettingswebeditor.h"

using namespace std;

QCoreApplication *a;

void signalHandler(int signum) { cerr << "Terminating\n"; a->exit(0); }

int main(int argc, char *argv[])
{
  a = new QCoreApplication(argc, argv);
  signal(SIGINT,  signalHandler); signal(SIGTERM, signalHandler);

  QSettingsWebEditor qswe;

  return a->exec();
}

//#include "test.moc" //http://stackoverflow.com/questions/5854626/qt-signals-and-slots-error-undefined-reference-to-vtable-for