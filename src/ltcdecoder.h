#ifndef LTCDECODER_H
#define LTCDECODER_H

#include <QIODevice>

#include "timecode_frame.h"


constexpr int LTC_DECODER_RATE = 44100;
constexpr int LTC_DECODER_BITS = 8;


class LTCDecoder: public QIODevice
{
  Q_OBJECT

public:
  LTCDecoder(QObject *parent=nullptr);
  ~LTCDecoder();

  virtual inline qint64 readData(char *, qint64) override { return 0; }
  virtual qint64 writeData(const char *data, qint64 len) override;

signals:
  void newFrame(TimecodeFrame frame);

private:
  void *_private {nullptr};
};

#endif // LTCDECODER_H
