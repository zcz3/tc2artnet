
#include "transmitter.h"


namespace {
constexpr int ARTTIMECODE_LENGTH = 19;
constexpr int ARTNET_PORT = 0x1936;
}


Transmitter::Transmitter()
{
  _buffer.resize(ARTTIMECODE_LENGTH);
  _buffer.fill('\0');
  _buffer.replace(0, 7, "Art-Net");
  _buffer[8] = 0;     // OpCode lo
  _buffer[9] = 0x97;  // OpCode hi
  _buffer[10] = 0;    // ProtoVerHi
  _buffer[11] = 14;   // ProtoVerLo
}


Transmitter::~Transmitter()
{

}


/**
 * Transmits an Art-Net timecode packet.
 *
 * If 'localhost' is true, a packet is sent to 127.0.0.1.
 *
 * If 'external' is not 0, a packet is sent to that IP address.
 */
void Transmitter::transmit(bool localhost, ip4_addr external, TimecodeFrame &frame)
{
  if(!localhost && !external)
    return;

  static const QHostAddress LOCALHOST(QHostAddress::LocalHost);

  _buffer[14] = frame.frames;
  _buffer[15] = frame.seconds;
  _buffer[16] = frame.minutes;
  _buffer[17] = frame.hours;
  _buffer[18] = static_cast<char>(frame.type);

  if(localhost)
    _sock.writeDatagram(_buffer, LOCALHOST, ARTNET_PORT);

  if(external)
    _sock.writeDatagram(_buffer, QHostAddress(external), ARTNET_PORT);
}
