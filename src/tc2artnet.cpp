
#include "tc2artnet.h"

#include <QSettings>

#include "timecode_frame.h"


TC2ArtNet::TC2ArtNet(QObject *parent)
  :QObject(parent)
{
  qRegisterMetaType<TimecodeFrame>("TimecodeFrame");

  _transmitter = new Transmitter;
  _ltc_receiver = new LTCReceiver;
  _mainwindow = new MainWindow;

  connect(_ltc_receiver, &LTCReceiver::statusChanged, _mainwindow, &MainWindow::setStatus);
  connect(_ltc_receiver, &LTCReceiver::statusChanged, [&](bool, QString) {
    _timeout.stop();
    _locked = false;
  });
  connect(_ltc_receiver, &LTCReceiver::newFrame, this, &TC2ArtNet::processFrame);

  _timeout.setInterval(1000);
  _timeout.setSingleShot(true);
  connect(&_timeout, &QTimer::timeout, this, &TC2ArtNet::rxTimeout);
}


TC2ArtNet::~TC2ArtNet()
{
  delete _mainwindow;
  delete _ltc_receiver;
  delete _transmitter;
}


/**
 * Shows the UI and enters the main app loop.
 * Returns app exit code.
 */
int TC2ArtNet::run(QApplication *app)
{
  QTimer::singleShot(0, this, &TC2ArtNet::startup);
  int r = app->exec();
  saveSettings();

  return r;
}


void TC2ArtNet::processFrame(TimecodeFrame frame)
{
  if(_timestamp_adjust)
  {
    frame.frames += _timestamp_adjust;

    int fps = timecode_frames_per_sec(frame);
    if(frame.frames >= fps)
    {
      frame.seconds += frame.frames / fps;
      frame.frames = frame.frames % fps;
    }
  }

  _transmitter->transmit(
    _artnet_loopback,
    _artnet_external ? _artnet_ip : 0,
        frame);
  _mainwindow->setTimeStamp(frame);
  _timeout.start();

  if(!_locked || frame.type != _locked_type)
  {
    _locked = true;
    _locked_type = frame.type;

    QString msg = "locked";
    switch(frame.type)
    {
      case TimecodeType::SMPTE30:
        msg += " 30fps";
        break;
      case TimecodeType::DF30:
        msg += " 30fps DF";
        break;
      case TimecodeType::EBU25:
        msg += " 25fps";
        break;
      case TimecodeType::Film24:
        msg += " 24fps";
        break;
    }

    _mainwindow->setStatus(false, msg);
  }
}


/**
 * If you double-click on the timestamp when there is no external
 * signal comming in, the clock resets to 00:00:00.00 and sends a
 * zero frame over Art-Net.
 */
void TC2ArtNet::resetTimeStamp()
{
  if(_locked)
    return;

  TimecodeFrame frame;
  frame.hours = frame.minutes = frame.seconds = frame.frames = 0;
  frame.type = _locked_type;

  _transmitter->transmit(
    _artnet_loopback,
    _artnet_external ? _artnet_ip : 0,
        frame);
  _mainwindow->setTimeStamp(frame);
}


void TC2ArtNet::rxTimeout()
{
  _mainwindow->setStatus(false, "listening");
  _locked = false;
}


/**
 * Late init - runs in main app loop.
 */
void TC2ArtNet::startup()
{
  loadSettings();
  _mainwindow->setControls(
        _ltc_receiver->deviceList(),
        _audio_device_index,
        _artnet_loopback,
        _artnet_external,
        QHostAddress(_artnet_ip).toString(),
        _timestamp_adjust);
  _mainwindow->show();

  connect(_mainwindow, &MainWindow::resetTimeStampClicked, this, &TC2ArtNet::resetTimeStamp);
  connect(_mainwindow, &MainWindow::audioDeviceChanged, [&](int index){
    _ltc_receiver->stop();
    _ltc_receiver->setDevice(index);
    _locked = false;
    _mainwindow->setStatus(true, "opening device");
    _ltc_receiver->start();
    _audio_device_index = index;
  });
  connect(_mainwindow, &MainWindow::artnetLoopbackChanged, [&](bool enabled){
    _artnet_loopback = enabled;
  });
  connect(_mainwindow, &MainWindow::artnetExternalChanged, [&](bool enabled){
    _artnet_external = enabled;
  });
  connect(_mainwindow, &MainWindow::artnetIPChanged, [&](QString ip){
    _artnet_ip = QHostAddress(ip).toIPv4Address();
  });
  connect(_mainwindow, &MainWindow::timestampAdjustChanged, [&](int frames){
    _timestamp_adjust = frames;
  });

  _ltc_receiver->setDevice(_audio_device_index);
  _ltc_receiver->start();
}


void TC2ArtNet::loadSettings()
{
  QSettings settings;

  QString audio_device_name = settings.value("audio_device_name", "").toString().trimmed();
  _audio_device_index = _ltc_receiver->deviceList().indexOf(audio_device_name);
  _artnet_loopback = settings.value("artnet_loopback", true).toBool();
  _artnet_external = settings.value("artnet_external", false).toBool();
  _artnet_ip = settings.value("artnet_ip", (10 << 24) + (99 << 16) + 10).toInt();
  _timestamp_adjust = settings.value("timestamp_adjust", 0).toInt();

  if(_timestamp_adjust < 0 || _timestamp_adjust > 20)
    _timestamp_adjust = 0;
}


void TC2ArtNet::saveSettings()
{
  QSettings settings;

  QString audio_device_name;
  if(_audio_device_index >= 0 && _audio_device_index < _ltc_receiver->deviceList().size())
    audio_device_name = _ltc_receiver->deviceList()[_audio_device_index];

  settings.setValue("audio_device_name", audio_device_name);
  settings.setValue("artnet_loopback", _artnet_loopback);
  settings.setValue("artnet_external", _artnet_external);
  settings.setValue("artnet_ip", _artnet_ip);
  settings.setValue("timestamp_adjust", _timestamp_adjust);
}
