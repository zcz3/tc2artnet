
#include "mididev.h"

#include <Windows.h>

#define MIDI_PRIV static_cast<midi_dev_private *>(_private)


namespace {
struct midi_dev_private {
  QStringList device_list {};
  bool open {false};
  HMIDIIN handle;
};

void CALLBACK midi_in_callback(
   HMIDIIN   hMidiIn,
   UINT      wMsg,
   DWORD_PTR dwInstance,
   DWORD_PTR dwParam1,
   DWORD_PTR dwParam2)
{
  MIDIDev *d = (MIDIDev *)dwInstance;

  if(wMsg == MIM_DATA)
  {
    int time_ms = (int)dwParam2;
    int status = dwParam1 & 0xFF;
    int data1 = (dwParam1 >> 8) & 0x7F;
    int data2 = (dwParam2 >> 16) & 0x7F;

    d->midiMessage(time_ms, status, data1, data2);
  }
}
}


MIDIDev::MIDIDev(QObject *parent)
  :QObject(parent)
{
  _private = new midi_dev_private;
  midi_dev_private *p = MIDI_PRIV;

  MIDIINCAPSW dev;

  for(uint i = 0; i < midiInGetNumDevs(); i++)
  {
    memset(&dev, 0, sizeof(dev));

    if(midiInGetDevCapsW(i, &dev, sizeof(dev)) != MMSYSERR_NOERROR)
    {
      p->device_list.clear();
      break;
    }

    QString name = QString::fromWCharArray(dev.szPname).trimmed();
    if(name.isEmpty())
      name = tr("Unknown device");

    p->device_list << name;
  }
}


MIDIDev::~MIDIDev()
{
  midi_dev_private *p = MIDI_PRIV;

  close();

  if(p)
    delete p;
}


const QStringList MIDIDev::deviceList()
{
  midi_dev_private *p = MIDI_PRIV;

  return p->device_list;
}


bool MIDIDev::open(int device)
{
  midi_dev_private *p = MIDI_PRIV;

  close();

  if(p->device_list.size() == 0 || device < 0 || device >= p->device_list.size())
    return false;

  QString dev_name = p->device_list[device];

  // The list of MIDI devices might have changed since startup,
  // so iterate it again and find the correct device ID.
  // TODO: Deal with multiple devices with the same name.

  MIDIINCAPSW dev;
  uint n_devs = midiInGetNumDevs();
  uint i = 0;

  for(; i < n_devs; i++)
  {
    memset(&dev, 0, sizeof(dev));

    if(midiInGetDevCapsW(i, &dev, sizeof(dev)) != MMSYSERR_NOERROR)
      return false;

    QString name = QString::fromWCharArray(dev.szPname).trimmed();
    if(name.isEmpty())
      name = tr("Unknown device");

    if(name == dev_name)
      break;
  }

  if(i == n_devs)
    return false;

  MMRESULT result = midiInOpen(
        &(p->handle),
        i,
        (DWORD_PTR)&midi_in_callback,
        (DWORD_PTR )this,
        CALLBACK_FUNCTION);

  if(result != MMSYSERR_NOERROR)
    return false;

  result = midiInStart(p->handle);

  if(result != MMSYSERR_NOERROR)
  {
    midiInClose(p->handle);
    return false;
  }

  p->open = true;

  return true;
}


void MIDIDev::close()
{
  midi_dev_private *p = MIDI_PRIV;

  if(p->open)
  {
    midiInStop(p->handle);
    midiInClose(p->handle);
    p->open = false;
  }
}
