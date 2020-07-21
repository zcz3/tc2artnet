
#include "ltcreceiver.h"


LTCReceiver::LTCReceiver(QObject *parent)
  :QObject(parent)
  ,_input_devices(QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
{
  for(auto &device : _input_devices)
    _input_names.append(device.deviceName().trimmed());
}


LTCReceiver::~LTCReceiver()
{
  stop();
}


void LTCReceiver::setDevice(int index)
{
  if(index == -1 && index != _current_device)
  {
    // No device selected
    stop();
    _current_device = -1;
  }

  if(_input_devices.size() > 0 &&
     index >= 0 &&
     index < _input_devices.size() &&
     index != _current_device)
  {
    stop();
    _current_device = index;
  }
}


void LTCReceiver::start()
{
  stop();

  if(_input_devices.size() == 0)
  {
    statusChanged(true, "no audio devices");
  }
  else if(_current_device < 0 || _current_device >= _input_devices.size())
  {
    statusChanged(true, "no audio device selected");
  }
  else
  {
    QAudioDeviceInfo &device = _input_devices[_current_device];

    QAudioFormat format;
    format.setCodec("audio/pcm");
    format.setSampleRate(LTC_DECODER_RATE);
    format.setSampleSize(LTC_DECODER_BITS);
    format.setSampleType(QAudioFormat::UnSignedInt);
    format.setChannelCount(1);

    if(!device.isFormatSupported(format))
    {
      statusChanged(true, "audio device incompatible");
      return;
    }

    _input = new QAudioInput(device, format);
    _decoder = new LTCDecoder;

    connect(_decoder, &LTCDecoder::newFrame, this, &LTCReceiver::newFrame);
    connect(_input, &QAudioInput::stateChanged, this, &LTCReceiver::handleStateChanged);
    _input->start(_decoder);
  }
}


void LTCReceiver::stop()
{
  if(_input)
  {
    _input->stop();
    _input->deleteLater();
    _input = nullptr;
  }

  if(_decoder)
  {
    delete _decoder;
    _decoder = nullptr;
  }

  statusChanged(true, "stopped");
}


void LTCReceiver::handleStateChanged(QAudio::State state)
{
  switch(state)
  {
    case QAudio::ActiveState:
      statusChanged(false, "listening");
      break;

    default:
      statusChanged(true, "unable to open device");
      break;
  }
}

