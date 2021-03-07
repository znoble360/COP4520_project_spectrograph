#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "Spectrum.h"
#include "Engine.h"
#include <QDialog>
#include <QAudioDeviceInfo>

QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
class QSlider;
class QSpinBox;
class QGridLayout;
QT_END_NAMESPACE

/**
 * Dialog used to control settings such as the audio input / output device
 * and the windowing function.
 */
    class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(const QList<QAudioDeviceInfo>& availableOutputDevices,
                   QWidget* parent = 0);
    ~SettingsDialog();

    WindowFunction windowFunction() const { return m_windowFunction; }
    const QAudioDeviceInfo& outputDevice() const { return m_outputDevice; }

private slots:
    void windowFunctionChanged(int index);
    void outputDeviceChanged(int index);

private:
    WindowFunction   m_windowFunction;
    QAudioDeviceInfo m_outputDevice;
    Engine* m_parentEngine;

    QComboBox* m_outputDeviceComboBox;
    QComboBox* m_windowFunctionComboBox;
};

#endif // SETTINGSDIALOG_H
