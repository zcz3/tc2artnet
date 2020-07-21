#ifndef TC2ARTNET_H
#define TC2ARTNET_H

#include <QObject>
#include <QApplication>
#include <QTimer>

#include "transmitter.h"
#include "ltcreceiver.h"
#include "mainwindow.h"


/**
 * The 'Controller' which glues together the UI, decoder,
 * tranmistter and settings.
 */
class TC2ArtNet : public QObject
{
  Q_OBJECT

public:
  TC2ArtNet(QObject *parent=nullptr);
  ~TC2ArtNet();

  int run(QApplication *app);

  void processFrame(TimecodeFrame frame);
  void resetTimeStamp();
  void rxTimeout();

private:
  void startup();
  void loadSettings();
  void saveSettings();

  Transmitter *_transmitter;
  LTCReceiver *_ltc_receiver;
  MainWindow *_mainwindow;

  QTimer _timeout {};
  bool _locked {false};
  TimecodeType _locked_type {TimecodeType::SMPTE30};

  // Settings
  int _audio_device_index {-1};
  bool _artnet_loopback {true};
  bool _artnet_external {false};
  uint32_t _artnet_ip {0};
  int _timestamp_adjust {0};
};

#endif // TC2ARTNET_H
