#include <QApplication>
#include "MainWindow.h"
#include <QTranslator>
#include <QDir>
#include <QMutex>

#include "Options.h"
#include "Decoder.h"


Options options;
QMutex  FFTwInitMutex;

int main(int argc, char *argv[]) {

QApplication app(argc, argv);
QTranslator translator;
QString translatorPath = argv[0];
Decoder decoder;
int x;

  options.init();

  x = translatorPath.lastIndexOf(QString(QDir::separator()) + "wrx");
  if (x > 0) {
    translatorPath = translatorPath.left(x) + translatorPath.mid(x) + "_de.qm";
    translator.load(translatorPath);
    app.installTranslator(&translator);
  }

  MainWindow w;
  decoder.setMainWindow(&w);
  w.resize(options.WindowWH);
  w.move(options.WindowXY);
  if (options.WindowFormat == "maximized")
    w.showMaximized();
  else {
    if (options.WindowFormat == "fullscreen")
      w.showFullScreen();
    else
      w.show();
  }
  int ret = app.exec();
  options.write();
  options.clear();
  return ret;
}
