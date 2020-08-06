#ifndef MIDIDEV_H
#define MIDIDEV_H

#include <QObject>


class MIDIDev: public QObject
{
Q_OBJECT

public:
  MIDIDev(QObject *parent=nullptr);
  ~MIDIDev();

  const QStringList deviceList();
  bool open(int device);
  void close();

signals:
  void midiError(QString message);
  void midiMessage(int time_ms, int status, int data1, int data2);

private:
  void *_private {nullptr};
};

#endif
