#ifndef LTCRECEIVER_H
#define LTCRECEIVER_H

#include <QObject>
#include <QAudioInput>
#include <QAudioDeviceInfo>

#include "timecode_frame.h"
#include "ltcdecoder.h"


class LTCReceiver: public QObject
{
Q_OBJECT

public:
  LTCReceiver(QObject *parent=nullptr);
  ~LTCReceiver();

  inline const QStringList deviceList() { return _input_names; }
  void setDevice(int index);
  void start();
  void stop();

  void handleStateChanged(QAudio::State state);

signals:
  void newFrame(TimecodeFrame frame);
  void statusChanged(bool error, QString message);

private:
  QAudioInput *_input {nullptr};
  LTCDecoder *_decoder {nullptr};

  QList<QAudioDeviceInfo> _input_devices;
  QStringList _input_names;
  int _current_device {-1};
};

#endif // LTCRECEIVER_H
