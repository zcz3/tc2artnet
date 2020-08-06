#ifndef MTCRECEIVER_H
#define MTCRECEIVER_H

#include <QObject>

#include "timecode_frame.h"
#include "mididev.h"


class MTCReceiver: public QObject
{
Q_OBJECT

public:
  MTCReceiver(QObject *parent=nullptr);
  ~MTCReceiver();

  inline const QStringList deviceList() { return _midi_dev.deviceList(); }
  void setDevice(int index);
  void start();
  void stop();

  // Private - link to MIDI device
  void _processMidiMessage(int time_ms, int status, int data1, int data2);

signals:
  void newFrame(TimecodeFrame frame);
  void statusChanged(bool error, QString message);

private:
  void reset_qf_vars();

  MIDIDev _midi_dev {};
  int _current_device {-1};

  int _last_qf_num;
  int _last_qf_time;
  uint8_t _qf_bits[8];
  TimecodeFrame _last_frame;
};


#endif
