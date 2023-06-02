#include "engine.h"
#include "utils.h"
#include "3rdparty/fftreal/fftreal_wrapper.h" // For FFTLengthPowerOfTwo

#include <math.h>

#include <QAudioSource>
#include <QAudioSink>
#include <QCoreApplication>
#include <QFile>
#include <QMetaObject>
#include <QSet>
#include <QThread>

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const qint64 BufferDurationUs = 10 * 100000000;

int frames = 120;
int fps = 1000 / frames;

//-----------------------------------------------------------------------------
// Constructor and destructor
//-----------------------------------------------------------------------------

Engine::Engine(QObject *parent)
    :   QObject(parent)
    ,   m_mode(QAudioDevice::Input)
    ,   m_state(QAudio::StoppedState)
    ,   m_devices(new QMediaDevices(this))
    ,   m_availableAudioInputDevices(m_devices->audioInputs())
    ,   m_audioInputDevice(m_devices->defaultAudioInput())
    ,   m_audioInput(nullptr)
    ,   m_audioInputIODevice(nullptr)
    ,   m_recordPosition(0)
    ,   m_bufferPosition(0)
    ,   m_bufferLength(0)
    ,   m_dataLength(0)
    ,   m_spectrumBufferLength(0)
    ,   m_spectrumPosition(0)
{
    connect(&m_spectrumAnalyser, QOverload<const FrequencySpectrum&>::of(&SpectrumAnalyser::spectrumChanged),
            this, QOverload<const FrequencySpectrum&>::of(&Engine::spectrum_change));

    QStringList arguments = QCoreApplication::instance()->arguments();
    for (int i = 0; i < arguments.count(); ++i)
    {
        if (arguments.at(i) == QStringLiteral("--"))
        {
            break;
        }
    }

    initialize();

    m_notifyTimer = new QTimer(this);
    m_notifyTimer->setInterval(fps);
    connect(m_notifyTimer, &QTimer::timeout, this, &Engine::audioNotify);

    connect(m_devices, &QMediaDevices::audioInputsChanged,
            this, &Engine::audioInputDevicesChanged);
}

Engine::~Engine() = default;

//-----------------------------------------------------------------------------
// Public slots
//-----------------------------------------------------------------------------


void Engine::startRecording()
{
    if (m_audioInput)
    {
        if (QAudioDevice::Input == m_mode && QAudio::SuspendedState == m_state)
        {
            m_audioInput->resume();
        }
        else
        {
            m_spectrumAnalyser.cancelCalculation();
            emit spectrumChanged(FrequencySpectrum());

            m_buffer.fill(0);
            m_mode = QAudioDevice::Input;

            m_dataLength = 0;
            m_audioInputIODevice = m_audioInput->start();
            connect(m_audioInputIODevice, &QIODevice::readyRead,
                    this, &Engine::audioDataReady);
        }
        m_notifyTimer->start();
    }
}

void Engine::suspend()
{
    if (QAudio::ActiveState == m_state || QAudio::IdleState == m_state)
    {
        switch (m_mode)
        {
        case QAudioDevice::Input:
            m_audioInput->suspend();
            break;
        default:
            break;
        }
        m_notifyTimer->stop();
    }
    stopRecording();
}

void Engine::setAudioInputDevice(const QAudioDevice &device)
{
    if (device.id() != m_audioInputDevice.id())
    {
        m_audioInputDevice = device;
        stopRecording();
        reset();
        initialize();
        startRecording();
    }
}

void Engine::FPSchanged(const int i)
{
    frames = i;
    fps = 1000 / frames;
    m_notifyTimer->setInterval(fps);
}

//-----------------------------------------------------------------------------
// Private slots
//-----------------------------------------------------------------------------

void Engine::audioNotify()
{
    switch (m_mode)
    {
        case QAudioDevice::Input:
            if (m_dataLength >= m_spectrumBufferLength)
            {
                const qint64 spectrumPosition = m_dataLength - m_spectrumBufferLength;
                calculateSpectrum(spectrumPosition);
            }
            emit bufferChanged(m_dataLength, m_buffer);
            break;
        default:
            break;
    }
}

void Engine::audioDataReady()
{
    Q_ASSERT(0 == m_bufferPosition);
    const qint64 bytesReady = m_audioInput->bytesAvailable();
    const qint64 bytesSpace = m_buffer.size() - m_dataLength;
    const qint64 bytesToRead = qMin(bytesReady, bytesSpace);

    const qint64 bytesRead = m_audioInputIODevice->read(
                                       m_buffer.data() + m_dataLength,
                                       bytesToRead);

    if (bytesRead)
    {
        m_dataLength += bytesRead;
    }

    if (m_buffer.size() == m_dataLength)
    {
        stopRecording();
    }
}

void Engine::spectrum_change(const FrequencySpectrum &spectrum)
{
    emit spectrumChanged(spectrum);
}

void Engine::audioInputDevicesChanged()
{
    m_availableAudioInputDevices.clear();
    m_availableAudioInputDevices = m_devices->audioInputs();

    emit devicesChanged();
}

QAudioDevice Engine::audioInputDeviceSelected()
{
    return m_audioInputDevice;
}

//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void Engine::resetAudioDevices()
{
    delete m_audioInput;
    m_audioInput = nullptr;
    m_audioInputIODevice = nullptr;
    m_spectrumPosition = 0;
}

void Engine::reset()
{
    stopRecording();
    setFormat(QAudioFormat());
    m_buffer.clear();
    m_bufferPosition = 0;
    m_bufferLength = 0;
    m_dataLength = 0;
    resetAudioDevices();
}

bool Engine::initialize()
{
    bool result = false;

    QAudioFormat format = m_format;

    if (selectFormat())
    {
        if (m_format != format)
        {
            resetAudioDevices();
            m_bufferLength = m_format.bytesForDuration(BufferDurationUs);
            m_buffer.resize(m_bufferLength);
            m_buffer.fill(0);
            emit bufferChanged(0, m_buffer);
            m_audioInput = new QAudioSource(m_audioInputDevice, m_format, this);
            result = true;
        }
    }

    return result;
}

bool Engine::selectFormat()
{
    bool foundSupportedFormat = false;

    int minSampleRate = m_audioInputDevice.minimumSampleRate();
    int maxSampleRate = m_audioInputDevice.maximumSampleRate();
    int minChannelCount = m_audioInputDevice.minimumChannelCount();
    int maxChannelCount = m_audioInputDevice.maximumChannelCount();

    QAudioFormat format;
    format.setSampleFormat(QAudioFormat::Int16);
    format.setSampleRate(qBound(minSampleRate, 48000, maxSampleRate));
    format.setChannelCount(qBound(minChannelCount, 2, maxChannelCount));

    const bool inputSupport = m_audioInputDevice.isFormatSupported(format);
    if (inputSupport)
    {
        foundSupportedFormat = true;
    }

    setFormat(format);

    return foundSupportedFormat;
}

void Engine::stopRecording()
{
    if (m_audioInput)
    {
        m_audioInput->stop();
        QCoreApplication::instance()->processEvents();
        m_audioInput->disconnect();
    }
    m_audioInputIODevice = nullptr;
    m_notifyTimer->stop();
}

void Engine::calculateSpectrum(qint64 position)
{
    Q_ASSERT(position + m_spectrumBufferLength <= m_bufferPosition + m_dataLength);
    Q_ASSERT(0 == m_spectrumBufferLength % 2); // constraint of FFT algorithm

    if (m_spectrumAnalyser.isReady())
    {
        m_spectrumBuffer = QByteArray::fromRawData(m_buffer.constData() + position - m_bufferPosition,
                                                   m_spectrumBufferLength);
        m_spectrumPosition = position;
        m_spectrumAnalyser.calculate(m_spectrumBuffer, m_format);
    }
}

void Engine::setFormat(const QAudioFormat &format)
{
    m_format = format;
    m_spectrumBufferLength = PowerOfTwo<FFTLengthPowerOfTwo>::Result * format.bytesPerFrame();
}
