#ifndef ENGINE_H
#define ENGINE_H

#include "spectrumanalyser.h"

#include <QAudioDevice>
#include <QAudioFormat>
#include <QBuffer>
#include <QByteArray>
#include <QDir>
#include <QList>
#include <QObject>
#include <QMediaDevices>
#include <QTimer>

class FrequencySpectrum;
QT_BEGIN_NAMESPACE
class QAudioSource;
class QAudioSink;
QT_END_NAMESPACE

/*!
 * \brief Engine Class
 *
 * Class is responsible for interaction with audio devices, sampling their audio and
 * computing FFT in real time.
 */
class Engine : public QObject
{
    Q_OBJECT

public:
    explicit Engine(QObject *parent = 0);
    ~Engine();

    /*!
     * \brief List of available input devices
     *
     * \param[out] m_availableAudioInputDevices - list of available input devices
     */
    const QList<QAudioDevice> &availableAudioInputDevices() const
                                    { return m_availableAudioInputDevices; }

    /*!
     * \brief Reset of the class
     *
     * Resets parameters and devices, and stops working subclasses.
     */
    void reset();

    /*!
     * \brief Returns length of the data
     *
     * \param[out] m_dataLength - length of the data
     */
    qint64 dataLength() const { return m_dataLength; }

public slots:

    /*!
     * \brief Start sampling audio
     */
    void startRecording();

    /*!
     * \brief Stops sampling audio
     */
    void stopRecording();

    /*!
     * \brief Stops taking data from a device
     */
    void suspend();

    /*!
     * \brief Sets selected device as a default input device
     *
     * \param[in] device - audio device
     */
    void setAudioInputDevice(const QAudioDevice &device);

    /*!
     * \brief FPS amount has changed
     *
     * \param[in] i - new amount of FPS
     */
    void FPSchanged(const int i);

    /*!
     * \brief New audio device has been selected
     */
    void audioInputDevicesChanged();

    /*!
     * \brief Gives selected audio device
     *
     * \param[out] QAudioDevice - selected audio device
     */
    QAudioDevice audioInputDeviceSelected();

signals:

    /*!
     * \brief Signals that spectrum has changed
     *
     * Sends signal for the other classes that spectrum has changed, because of the new FFT calculation has been completed.
     * \param[in] spectrum - new spectrum
     */
    void spectrumChanged(const FrequencySpectrum &spectrum);

    /*!
     * \brief Audio buffer has changed
     *
     * \param[in] length - length of buffer
     * \param[in] buffer - new audio buffer
     */
    void bufferChanged(qint64 length, const QByteArray &buffer);

    /*!
     * \brief List of the devices has changed
     */
    void devicesChanged();


private slots:

    /*!
     * \brief Setting timer for calling function in loop
     */
    void audioNotify();

    /*!
     * \brief Method for reading audio data
     */
    void audioDataReady();

    /*!
     * \brief Spectrum has changed
     *
     * Signals that FFT calculation has been completed.
     * \param[in] spectrum - new spectrum
     */
    void spectrum_change(const FrequencySpectrum &spectrum);

private:

    /*!
     * \brief Resets audio devices
     */
    void resetAudioDevices();

    /*!
     * \brief Initialize reading data
     *
     * \return If initializing has been completed successfully
     */
    bool initialize();

    /*!
     * \brief Checks if format is available
     *
     * \return If format is available
     */
    bool selectFormat();

    /*!
     * \brief Sets selected format
     *
     * \param If format has been selected successfully
     */
    void setFormat(const QAudioFormat &format);

    /*!
     * \brief Prepares data and starts FFT
     *
     * \param[in] position - possition of buffer
     */
    void calculateSpectrum(qint64 position);

private:

    QAudioDevice::Mode  m_mode;
    QAudio::State       m_state;
    QMediaDevices*      m_devices;

    QAudioFormat        m_format;

    QList<QAudioDevice> m_availableAudioInputDevices;
    QAudioDevice        m_audioInputDevice;
    QAudioSource*       m_audioInput;
    QIODevice*          m_audioInputIODevice;
    qint64              m_recordPosition;

    qint64              m_playPosition;

    QByteArray          m_buffer;
    qint64              m_bufferPosition;
    qint64              m_bufferLength;
    qint64              m_dataLength;

    int                 m_spectrumBufferLength;
    QByteArray          m_spectrumBuffer;
    SpectrumAnalyser    m_spectrumAnalyser;
    qint64              m_spectrumPosition;

    QTimer*             m_notifyTimer = nullptr;

};

#endif // ENGINE_H
