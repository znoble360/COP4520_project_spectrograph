#include "AudioFileStream.h"

#include <iostream>

AudioFileStream::AudioFileStream(Waveform* waveform, Spectrograph* spectrograph, QObject* parent) :
    QIODevice(parent),
    m_waveform(waveform),
    m_spectrograph(spectrograph),
    m_input(&m_data),
    m_output(&m_data),
    m_state(State::Stopped),
    m_peakVal(0),
    m_file(new QFile(this))
{
    setOpenMode(QIODevice::ReadOnly);

    isInited = false;
    isDecodingFinished = false;
}

bool AudioFileStream::init(const QAudioFormat& format)
{
    m_format = format;
    m_decoder.setAudioFormat(m_format);

    if (m_decoder.error() != QAudioDecoder::Error::NoError)
    {
        qDebug("AudioFileStream::init() ERROR: Audio decoder failed to set audio format.");
        return false;
    }

    connect(&m_decoder, &QAudioDecoder::bufferReady, this, &AudioFileStream::bufferReady);
    connect(&m_decoder, &QAudioDecoder::finished, this, &AudioFileStream::finished);

    // Initialize buffers
    if (!m_output.open(QIODevice::ReadOnly) || !m_input.open(QIODevice::WriteOnly))
    {
        return false;
    }

    m_peakVal = getPeakValue(m_format);

    if (m_peakVal == qreal(0))
    {
        qDebug("AudioFileStream::init() ERROR: Failed to get peak value, invalid audio format.");
        return false;
    }

    isInited = true;

    return true;
}

bool AudioFileStream::setFormat(const QAudioFormat& format)
{
    m_decoder.setAudioFormat(format);

    if (m_decoder.error() != QAudioDecoder::Error::NoError)
    {
        m_decoder.setAudioFormat(m_format);
        return false;
    }

    m_format = format;
    m_peakVal = getPeakValue(m_format);

    if (m_peakVal == qreal(0))
    {
        qDebug("AudioFileStream::setFormat() ERROR: Failed to get peak value, invalid audio format.");
        return false;
    }

    return true;
}

QFile* AudioFileStream::getFile()
{
    return m_file;
}

QAudioFormat AudioFileStream::getFormat()
{
    return m_format;
}

AudioFileStream::State AudioFileStream::getState()
{
    return m_state;
}

void AudioFileStream::drawChartSamples(int start, char* data)
{
    int sampleCount = m_waveform->getSampleCount();

    switch (m_format.sampleType())
    {
    case QAudioFormat::Float:
    case QAudioFormat::SignedInt:
        // For 32bit sample size
        if (m_format.sampleSize() == 32)
        {
            int* data_int = (int*)data;
            for (int s = start; s < sampleCount; ++s, ++data_int)
            {
                m_waveformBuffer[s].setY((int)*data_int / m_peakVal);
            }
        }
        // For 16bit sample size
        else if (m_format.sampleSize() == 16)
        {
            short* data_short = (short*)data;
            for (int s = start; s < sampleCount; ++s, ++data_short)
            {
                m_waveformBuffer[s].setY((short)*data_short / m_peakVal);
            }
        }
        // For 8bit sample size
        else if (m_format.sampleSize() == 8)
        {
            for (int s = start; s < sampleCount; ++s, ++data)
            {
                m_waveformBuffer[s].setY((char)*data / m_peakVal);
            }
        }
        break;
    case QAudioFormat::UnSignedInt:
        // For 32bit sample size
        if (m_format.sampleSize() == 32)
        {
            uint* data_int = (uint*)data;
            for (int s = start; s < sampleCount; ++s, ++data_int)
            {
                m_waveformBuffer[s].setY((uint)*data_int / m_peakVal);
            }
        }
        // For 16bit sample size
        else if (m_format.sampleSize() == 16)
        {
            ushort* data_short = (ushort*)data;
            for (int s = start; s < sampleCount; ++s, ++data_short)
            {
                m_waveformBuffer[s].setY((ushort)*data_short / m_peakVal);
            }
        }
        // For 8bit sample size
        else if (m_format.sampleSize() == 8)
        {
            for (int s = start; s < sampleCount; ++s, ++data)
            {
                m_waveformBuffer[s].setY((uchar)*data / m_peakVal);
            }
        }
        break;
    default:
        break;
    }
}

// AudioOutput device (like speaker) calls this function to get new audio data
qint64 AudioFileStream::readData(char* data, qint64 maxSize)
{
    memset(data, 0, maxSize);

    // If playing, read audio from m_output, else don't process any data
    if (m_state == State::Playing)
    {
        int sampleCount = m_waveform->getSampleCount();
        int resolution = m_format.sampleSize() / 8;

        m_output.read(data, maxSize);

        if (m_waveformBuffer.isEmpty())
        {
            m_waveformBuffer.reserve(sampleCount);
            for (int i = 0; i < sampleCount; ++i)
                m_waveformBuffer.append(QPointF(i, 0));
        }

        // Draw the available sample points to the chart
        int start = 0;
        const int availableSamples = int(maxSize) / resolution;
        if (availableSamples < sampleCount)
        {
            start = sampleCount - availableSamples;
            for (int s = 0; s < start; ++s)
            {
                m_waveformBuffer[s].setY(m_waveformBuffer.at(s + availableSamples).y());
            }
        }

        // Draw the rest of the chart's y-values based on sample-size data type
        drawChartSamples(start, data);

        m_waveform->getSeries()->replace(m_waveformBuffer);

        // Send read audio data via signal to output device.
        if (maxSize > 0)
        {
            QByteArray buff(data, maxSize);
            emit newData(buff);
        }

        // If at end of file
        if (m_output.atEnd())
        {
            stop();
        }
    }

    return maxSize;
}

qint64 AudioFileStream::writeData(const char* data, qint64 maxSize)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);

    return 0;
}

bool AudioFileStream::loadFile(const QString& filePath)
{
    if (m_peakVal == qreal(0) || !clear())
        return false;

    m_decoder.setSourceFilename(filePath);
    m_decoder.start();

    return true;
}

// Start playing the audio file
bool AudioFileStream::play(const QString& filePath)
{
    if (m_peakVal == qreal(0) || m_decoder.error() != QAudioDecoder::Error::NoError)
    {
        qDebug() << "AudioFileStream::play() ERROR: " << m_decoder.error();
        qDebug() << "AudioFileStream::play() Current value of m_format = " << m_format;
        return false;
    }

    if (m_state == State::Paused)
    {
        qDebug() << "AudioFileStream::play() Resuming audio " << filePath.toLatin1();

        m_state = State::Playing;
        emit stateChanged(m_state);
        return true;
    }

    m_state = State::Playing;
    emit stateChanged(m_state);

    return true;
}

// Pause the currently playing audio file
void AudioFileStream::pause()
{
    m_state = State::Paused;
    emit stateChanged(m_state);
}

// Stop playing audio file
void AudioFileStream::stop()
{
    m_file->close();
    clear();
    m_state = State::Stopped;
    emit stateChanged(m_state);
}

bool AudioFileStream::clear()
{
    m_decoder.stop();
    m_data.clear();
    m_waveformBuffer.clear();
    m_waveform->getSeries()->clear();

    m_output.close();
    m_input.close();

    if (!m_output.open(QIODevice::ReadOnly) || !m_input.open(QIODevice::WriteOnly))
    {
        return false;
    }

    isDecodingFinished = false;

    return true;
}

// Determines if reached the end of audio file
bool AudioFileStream::atEnd() const
{
    return m_output.size()
        && m_output.atEnd()
        && isDecodingFinished;
}

// QAudioDecoder Logic 
// This method is responsible for decoding the audio file and wrtiing audio data to stream buffer
// Only runs when decoder processed some audio data
void AudioFileStream::bufferReady() // SLOT
{
    const QAudioBuffer& buffer = m_decoder.read();

    const int length = buffer.byteCount();
    const char* data = buffer.constData<char>();

    m_input.write(data, length);
    m_spectrograph->getDataBuffer()->write(data, length);
}

// Runs when decoder finished decoding
void AudioFileStream::finished() // SLOT
{
    isDecodingFinished = true;

    // When audio decoding is finished we can start calculating and plotting the
    // DFT graph on a new thread.
    m_spectrograph->calculateSpectrum(m_format);
}

void AudioFileStream::cancelSpectrum()
{
    m_spectrograph->cancelCalculation();
}

qreal AudioFileStream::getPeakValue(const QAudioFormat& format)
{
    // Note: Only the most common sample formats are supported
    if (!format.isValid())
        return qreal(0);

    if (format.codec() != "audio/pcm")
        return qreal(0);

    switch (format.sampleType()) {
        case QAudioFormat::Unknown:
            break;
        case QAudioFormat::Float:
            if (format.sampleSize() != 32) // other sample formats are not supported
                return qreal(0);
            return qreal(1.00003);
        case QAudioFormat::SignedInt:
            if (format.sampleSize() == 32)
            {
            #ifdef Q_OS_WIN
                return qreal(INT_MAX);
            #endif
            #ifdef Q_OS_UNIX
                return qreal(SHRT_MAX);
            #endif
            }
            if (format.sampleSize() == 16)
            {
                return qreal(SHRT_MAX);
            }
            if (format.sampleSize() == 8)
            {
                return qreal(CHAR_MAX);
            }
            break;
        case QAudioFormat::UnSignedInt:
            if (format.sampleSize() == 32)
            {
                return qreal(UINT_MAX);
            }
            if (format.sampleSize() == 16)
            {
                return qreal(USHRT_MAX);
            }
            if (format.sampleSize() == 8)
            {
                return qreal(UCHAR_MAX);
            }
            break;
    }

    return qreal(0);
}
