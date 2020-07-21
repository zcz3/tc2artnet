#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QTimer>

#include "timecode_frame.h"


class MainWindow : public QWidget
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent=nullptr);
  ~MainWindow();

  void setStatus(bool error, QString message);
  void setTimeStamp(TimecodeFrame frame);
  void setControls(
      QStringList audio_device_list,
      int audio_device_index,
      bool artnet_loopback,
      bool artnet_external,
      QString artnet_ip,
      int timestamp_adjust);

  virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
  void resetTimeStampClicked();
  void audioDeviceChanged(int index);
  void artnetLoopbackChanged(bool enabled);
  void artnetExternalChanged(bool enabled);
  void artnetIPChanged(QString ip);
  void timestampAdjustChanged(int frames);

private:
  void createWidgets();
  void connectSignals();

  QVBoxLayout *_vbox_outer {nullptr};

  // Upper window: dashboard
  QLabel *_label_status {nullptr};
  QLabel *_label_timestamp {nullptr};

  // Midst: audio input settings
  QGroupBox *_gbox_audio {nullptr};
  QVBoxLayout *_vbox_audio {nullptr};
  QLabel *_label_audio {nullptr};
  QComboBox *_combo_audio {nullptr};

  // Lower window: Art-Net output settings
  QGroupBox *_gbox_settings {nullptr};
  QVBoxLayout *_vbox_settings {nullptr};
  QCheckBox *_check_loopback {nullptr};
  QCheckBox *_check_external {nullptr};
  QLineEdit *_edit_external_ip {nullptr};
  QHBoxLayout *_hbox_adjust {nullptr};
  QLabel *_label_adjust {nullptr};
  QSpinBox *_spin_adjust {nullptr};

  QTimer _external_ip_timer {};
};
#endif // MAINWINDOW_H
