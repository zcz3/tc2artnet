
#include "ltcdecoder.h"

#include <ltc.h>

#define GETLTC static_cast<LTCDecoder *>(_private)


LTCDecoder::LTCDecoder(QObject *parent)
  :QIODevice(parent)
{
  int apv = 1920;
  int queue_size = 32;
  _private = ltc_decoder_create(apv, queue_size);

  setOpenMode(QIODevice::WriteOnly);
}


LTCDecoder::~LTCDecoder()
{
  LTCDecoder *ltc = GETLTC;
  ltc_decoder_free(ltc);
  _private = nullptr;
}


qint64 LTCDecoder::writeData(const char *data, qint64 len)
{
  static int last_frame = 0;
  static int frame_rate_g = 0;
  static int frame_rate = 0;

  LTCDecoder *ltc = GETLTC;
  LTCFrameExt ltc_frame;
  SMPTETimecode ltc_stime;
  TimecodeFrame frame;

  ltc_decoder_write(ltc, (ltcsnd_sample_t *)data, len, 0);

  while(ltc_decoder_read(ltc, &ltc_frame))
  {
    if(ltc_frame.reverse)
      continue;

    ltc_frame_to_time(&ltc_stime, &ltc_frame.ltc, 0);

    frame.hours = ltc_stime.hours;
    frame.minutes = ltc_stime.mins;
    frame.seconds = ltc_stime.secs;
    frame.frames = ltc_stime.frame;
    frame.type = TimecodeType::SMPTE30;

    if(frame.frames == 0 &&
       (last_frame == 29 || last_frame == 24 || last_frame == 23))
    {
      // Could last_frame be the frame rate? It could very well could very well be.

      int r = last_frame + 1;

      if(r == frame_rate_g) // "frame rate guess"
      {
        // Same rate twice in a row, accept it
        frame_rate = r;
        frame_rate_g = 0;
      }
      else if(r != frame_rate)
      {
        // Possible change of frame rate, or first run
        if(r > frame_rate || frame_rate == 0)
        {
          frame_rate = r;
          frame_rate_g = 0;
        }
        else
        {
          frame_rate_g = r;
        }
      }
      else
      {
        frame_rate_g = 0;
      }
    }

    last_frame = frame.frames;

    if(ltc_frame.ltc.dfbit)
      frame.type = TimecodeType::DF30;
    else if(frame_rate == 25)
      frame.type = TimecodeType::EBU25;
    else if(frame_rate == 24)
      frame.type = TimecodeType::Film24;

    newFrame(frame);
  }

  return len;
}
