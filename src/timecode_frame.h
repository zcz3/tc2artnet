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


inline void timecode_frame_reset(TimecodeFrame &frame)
{
  frame.hours = 0;
  frame.minutes = 0;
  frame.seconds = 0;
  frame.frames = 0;
}


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


inline TimecodeFrame timecode_frame_add(TimecodeFrame frame, int frames)
{
  frame.frames += frames;

  int fps = timecode_frames_per_sec(frame);

  if(frame.frames >= fps)
  {
    frame.seconds += frame.frames / fps;
    frame.frames = frame.frames % fps;
  }

  return frame;
}


#endif // TIMECODE_FRAME_H
