
#include "mainwindow.h"

#include <QFont>
#include <QFontDatabase>
#include <QMouseEvent>


MainWindow::MainWindow(QWidget *parent)
  :QWidget(parent)
{
  setWindowTitle("TC2ArtNet");
  createWidgets();
  connectSignals();
}


MainWindow::~MainWindow()
{
  // Widgets get deleted automatically because
  // they are children (in QObject terminology).
}


void MainWindow::setStatus(bool error, QString message)
{
  QColor color = error ? Qt::red : Qt::black;
  if(!error && message.startsWith("locked"))
    color = Qt::darkGreen;

  QLabel *lab = _label_status;

  QPalette pal = lab->palette();
  pal.setColor(lab->foregroundRole(), color);
  lab->setPalette(pal);

  lab->setText(message);

  QString title = "TC2ArtNet - ";
  title += message;
  setWindowTitle(title);
}


void MainWindow::setTimeStamp(TimecodeFrame frame)
{
  QString ts = QString("%1:%2:%3.%4")
      .arg(frame.hours, 2, 10, QLatin1Char('0'))
      .arg(frame.minutes, 2, 10, QLatin1Char('0'))
      .arg(frame.seconds, 2, 10, QLatin1Char('0'))
      .arg(frame.frames, 2, 10, QLatin1Char('0'));

  _label_timestamp->setText(ts);
}


void MainWindow::setControls(
    bool ltc_enabled,
    QStringList audio_device_list,
    int audio_device_index,
    QStringList midi_device_list,
    int midi_device_index,
    bool artnet_loopback,
    bool artnet_external,
    QString artnet_ip,
    int timestamp_adjust)
{
  _combo_audio->addItem(tr("[Select device]"));
  _combo_audio->addItems(audio_device_list);

  if(audio_device_index >= 0 && audio_device_index < audio_device_list.size())
    _combo_audio->setCurrentIndex(audio_device_index + 1);
  else
    _combo_audio->setCurrentIndex(0);

  _combo_midi->addItem(tr("[Select device]"));
  _combo_midi->addItems(midi_device_list);

  if(midi_device_index >= 0 && midi_device_index < midi_device_list.size())
    _combo_midi->setCurrentIndex(midi_device_index + 1);
  else
    _combo_midi->setCurrentIndex(0);

  ltc_enabled ? _radio_ltc->setChecked(true) : _radio_mtc->setChecked(true);

  _check_loopback->setChecked(artnet_loopback);
  _check_external->setChecked(artnet_external);
  _edit_external_ip->setEnabled(artnet_external);
  _edit_external_ip->setText(artnet_ip);
  _spin_adjust->setValue(timestamp_adjust);

#ifndef ENABLE_MTC
  _radio_ltc->setChecked(true);
  _radio_ltc->hide();
  _radio_mtc->hide();
#endif
}


void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
  if(_label_timestamp->rect().contains(event->pos()))
    resetTimeStampClicked();
}


void MainWindow::createWidgets()
{
  Q_ASSERT(_vbox_outer == nullptr);

  _vbox_outer = new QVBoxLayout(this);

  // Upper window: dashboard

  const QFont tt = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  QString css_ts = QString(
      "padding: 10px;"
      "font-size: 30px;"
      "font-weight: bold;"
      "font-family: \"%1\";"
      "color: black;"
      "background-color: white;")
      .arg(tt.family());
  QString css_status = QString(
      "padding: 2px;"
      "font-size: 20px;"
      "font-weight: bold;"
      "font-family: \"%1\";"
      "background-color: white;")
      .arg(tt.family());

  _label_timestamp = new QLabel;
  _label_timestamp->setStyleSheet(css_ts);
  _label_timestamp->setAlignment(Qt::AlignCenter);
  _label_timestamp->setText("--:--:--.--");
  _vbox_outer->addWidget(_label_timestamp);

  _label_status = new QLabel;
  _label_status->setStyleSheet(css_status);
  _label_status->setAlignment(Qt::AlignCenter);
  _label_status->setText("starting up");
  _vbox_outer->addWidget(_label_status);

  // Midst: timecode input settings

  _gbox_input = new QGroupBox(tr("Timecode Input"));
  _vbox_outer->addWidget(_gbox_input);
  _vbox_input = new QVBoxLayout(_gbox_input);

  _hbox_input = new QHBoxLayout;
  _vbox_input->addLayout(_hbox_input);
  _radio_ltc = new QRadioButton("LTC");
  _radio_ltc->setChecked(true);
  _hbox_input->addWidget(_radio_ltc);
  _radio_mtc = new QRadioButton("MTC");
  _hbox_input->addWidget(_radio_mtc);

  _label_audio = new QLabel(tr("Audio device"));
  _vbox_input->addWidget(_label_audio);
  _combo_audio = new QComboBox;
  _vbox_input->addWidget(_combo_audio);

  _label_midi = new QLabel(tr("MIDI device"));
  _label_midi->setVisible(false);
  _vbox_input->addWidget(_label_midi);
  _combo_midi = new QComboBox;
  _combo_midi->setVisible(false);
  _vbox_input->addWidget(_combo_midi);

  // Lower window: settings

  _gbox_settings = new QGroupBox(tr("Art-Net Timecode Output"));
  _vbox_outer->addWidget(_gbox_settings);
  _vbox_settings = new QVBoxLayout(_gbox_settings);

  _check_loopback = new QCheckBox(tr("Send to applications on this PC (loopback)"));
  _check_loopback->setChecked(true);
  _vbox_settings->addWidget(_check_loopback);

  _check_external = new QCheckBox(tr("Send to external IP address:"));
  _vbox_settings->addWidget(_check_external);

  _edit_external_ip = new QLineEdit;
  _edit_external_ip->setContentsMargins(20, 0, 0, 0); // Approx align to checkbox label
  _edit_external_ip->setMaximumWidth(200);
  _edit_external_ip->setMaxLength(15);
  _edit_external_ip->setEnabled(false);
  _vbox_settings->addWidget(_edit_external_ip);

  _hbox_adjust = new QHBoxLayout;
  _vbox_settings->addLayout(_hbox_adjust);
  _label_adjust = new QLabel(tr("Timestamp adjust (frames):"));
  _hbox_adjust->addWidget(_label_adjust);
  _spin_adjust = new QSpinBox;
  _spin_adjust->setRange(0, 20);
  _hbox_adjust->addWidget(_spin_adjust);
  _hbox_adjust->addStretch(1);
}


void MainWindow::connectSignals()
{
  connect(_radio_ltc, &QRadioButton::toggled, [&](bool checked){
    // We have to hide everything first, otherwise layout gets screwed up
    _label_audio->setVisible(false);
    _combo_audio->setVisible(false);
    _label_midi->setVisible(false);
    _combo_midi->setVisible(false);

    _label_audio->setVisible(checked);
    _combo_audio->setVisible(checked);
    _label_midi->setVisible(!checked);
    _combo_midi->setVisible(!checked);
  });
  connect(_radio_ltc, &QRadioButton::toggled, this, &MainWindow::inputTypeChanged);

  connect(_combo_audio, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int index){
    audioDeviceChanged(--index);
  });

  connect(_combo_midi, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int index){
    midiDeviceChanged(--index);
  });

  connect(_check_loopback, &QCheckBox::toggled, this, &MainWindow::artnetLoopbackChanged);
  connect(_check_external, &QCheckBox::toggled, _edit_external_ip, &QLineEdit::setEnabled);
  connect(_check_external, &QCheckBox::toggled, this, &MainWindow::artnetExternalChanged);

  // Wait until user has stopped typing for 1s before accepting new IP address
  _external_ip_timer.setInterval(1000);
  _external_ip_timer.setSingleShot(true);
  connect(&_external_ip_timer, &QTimer::timeout, [&](){
    artnetIPChanged(_edit_external_ip->text().trimmed());
  });

  connect(_edit_external_ip, &QLineEdit::textEdited, [&](QString){
    _external_ip_timer.start();
  });

  connect(_edit_external_ip, &QLineEdit::editingFinished, [&](){
    _external_ip_timer.stop();
    artnetIPChanged(_edit_external_ip->text().trimmed());
  });

  connect(_spin_adjust, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
          this, &MainWindow::timestampAdjustChanged);
}
