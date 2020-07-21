#ifndef TIMECODE_FRAME_H
#define TIMECODE_FRAME_H

#include <QMetaType>


// Values match Art-Net format
enum class TimecodeType {
  Film24  = 0,
  EBU25   = 1,
  DF30    = 2,
  SMPTE30 = 3
};


struct TimecodeFrame {
  int hours {0};
  int minutes {0};
  int seconds {0};
  int frames {0};
  TimecodeType type;
};

Q_DECLARE_METATYPE(TimecodeFrame)


inline int timecode_frames_per_sec(TimecodeFrame frame)
{
  switch(frame.type)
  {
    case TimecodeType::Film24:
      return 24;
    case TimecodeType::EBU25:
      return 25;
    default:
      return 30;
  }
}

#endif // TIMECODE_FRAME_H
