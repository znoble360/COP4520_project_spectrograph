#ifndef SPECTROGRAPHUI_H
#define SPECTROGRAPHUI_H

#include "SettingsDialog.h"
#include "AudioFileStream.h"
#include "Spectrograph.h"
#include "Waveform.h"

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtGui/QIcon>
#include <QCoreApplication>
#include <QStyle>
#include <QString>
#include <QFileDialog>
#include <QtMultimedia/QAudio>
#include <QtMultimedia/QAudioOutput>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QSlider>

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE


class SpectrographUI : public QMainWindow
{
    Q_OBJECT

public:
    SpectrographUI(QWidget *parent = Q_NULLPTR);

public slots:
    void stateChanged(AudioFileStream::State state);

private slots:
    void showFileDialog();
    void showSettingsDialog();
    void startPlayback();
    void pausePlayback();
    void volumeChanged(int value);
    void toggleVolumeMute();

private:
    void createLayouts();
    void createActions();
    void createMenus();
    void updateChartTitle();
    void connectUI();
	void showWarningDialog(QString msg, QString informativeMsg = "");
    bool setAudioOutputDevice(const QAudioDeviceInfo& device);
    void quitApplication();

    Waveform* m_waveform;
    Spectrograph* m_spectrograph;
    AudioFileStream* m_device = nullptr;
    QAudioOutput* m_audioOutput = nullptr;
    QAudioDeviceInfo m_deviceInfo;

    SettingsDialog* m_settingsDialog;
    QMenu* m_fileMenu;
    QAction* m_openFileAct;
    QAction* m_exitAct;
    QPushButton* m_openWavFileButton;
    QIcon m_wavFileIcon;
    QPushButton* m_pauseButton;
    QIcon m_pauseIcon;
    QPushButton* m_playButton;
    QIcon m_playIcon;
    QPushButton* m_settingsButton;
    QIcon m_settingsIcon;
    QSlider* m_volumeSlider;
    QIcon m_volumeOnIcon;
    QIcon m_volumeMutedIcon;
    QPushButton* m_volumeMuteButton;
    QString m_currentFilePath;

    bool m_volumeMuted;
};

#endif // SPECTROGRAPHUI_H