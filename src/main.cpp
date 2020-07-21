
#include <QApplication>

#include "tc2artnet.h"


int main(int argc, char *argv[])
{
  QCoreApplication::setOrganizationName("zcz3");
  QCoreApplication::setApplicationName("TC2ArtNet");

  QApplication app(argc, argv);

  TC2ArtNet tc2artnet;
  return tc2artnet.run(&app);
}
