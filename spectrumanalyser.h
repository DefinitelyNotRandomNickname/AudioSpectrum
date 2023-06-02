#ifndef SPECTRUMANALYSER_H
#define SPECTRUMANALYSER_H

#include <QByteArray>
#include <QObject>
#include <QList>

#include "frequencyspectrum.h"
#include "3rdparty/fftreal/FFTRealFixLenParam.h"

QT_FORWARD_DECLARE_CLASS(QAudioFormat)
QT_FORWARD_DECLARE_CLASS(QThread)

// Zmienna odpowiedzialna za wzmocnienie amplitudy
const qreal SpectrumAnalyserMultiplier = 0.15;

class FFTRealWrapper;

class SpectrumAnalyserThreadPrivate;


/*!
 * \brief Klasa SpectrumAnalyserThread
 *
 * Klasa odpowiedzialna za obliczanie FFT
 */
class SpectrumAnalyserThread : public QObject
{
    Q_OBJECT

public:
    SpectrumAnalyserThread(QObject *parent);
    ~SpectrumAnalyserThread();

public slots:

    /*!
     * \brief Wywołanie obliczenia okna transformacji
     */
    void setWindowFunction();

    /*!
     * \brief Przygotowywanie danych do obliczeń i wywołanie FFT
     *
     * \param[in] buffer - bufon danych do transforamcji
     * \param[in] inputFrequency - częstotliwość wejściowa do transforamcji
     * \param[in] bytesPerSample - ilość byte'ów na próbkę do transformacji
     */
    void calculateSpectrum(const QByteArray &buffer,
                           int inputFrequency,
                           int bytesPerSample);

signals:
    /*!
     * \brief Sygnalizacja, że obliczenie FFT zostały zakończone
     *
     * \param[in] spectrum - obliczone spektrum
     */
    void calculationComplete(const FrequencySpectrum &spectrum);

private:
    /*!
     * \brief obliczenie okna transformacji
     */
    void calculateWindow();

private:

    FFTRealWrapper*                             m_fft;

    const int                                   m_numSamples;

    typedef FFTRealFixLenParam::DataType        DataType;
    QList<DataType>                             m_window;
    QList<DataType>                             m_input;
    QList<DataType>                             m_output;
    FrequencySpectrum                           m_spectrum;
    QThread*                                    m_thread;
};

/*!
 * \brief Klasa SpectrumAnalyser
 *
 * Klasa odpowiedzialna za obliczanie FFT.
 */
class SpectrumAnalyser : public QObject
{
    Q_OBJECT

public:
    SpectrumAnalyser(QObject *parent = 0);
    ~SpectrumAnalyser();

public:

    /*!
     * \brief Oblicz spektrum częstotliwości
     *
     * \param[in] buffer - bufor danych audio
     * \param[in] format - format danych audio
     */
    void calculate(const QByteArray &buffer, const QAudioFormat &format);

    /*!
     * \brief Sprawdza czy można dokonać kolejnej kalkulacji
     *
     * \return Czy można dokonać kolejnych kalkulacji
     */
    bool isReady() const;

    /*!
     * \brief Anuluj aktualną kalkulację
     */
    void cancelCalculation();

signals:

    /*!
     * \brief Spektrum uległo zmianie
     *
     * \param[in] spectrum - nowe spektrum
     */
    void spectrumChanged(const FrequencySpectrum &spectrum);

private slots:

    /*!
     * \brief Kalkulacja została zakończona
     *
     * \param[in] spectrum - obliczone spektrum
     */
    void calculationComplete(const FrequencySpectrum &spectrum);

private:

    SpectrumAnalyserThread*    m_thread;

    enum State {
        Idle,
        Busy,
        Cancelled
    };

    State              m_state;
};

#endif // SPECTRUMANALYSER_H

