#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <cstdint>
#include <QUdpSocket>
#include <QByteArray>

#include "timecode_frame.h"


typedef uint32_t ip4_addr;


/**
 * Art-Net timecode transmitter.
 */
class Transmitter
{
public:
  Transmitter();
  ~Transmitter();

  void transmit(bool localhost, ip4_addr external, TimecodeFrame &frame);

private:
  QUdpSocket _sock;
  QByteArray _buffer;
};


#endif // TRANSMITTER_H
