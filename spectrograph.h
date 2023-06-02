#ifndef SPECTROGRAPH_H
#define SPECTROGRAPH_H

#include "frequencyspectrum.h"

#include <QWidget>

/*!
 * \brief Klasa Spectograph
 *
 * Widget odpowiedzialny za wizualizację transformaty Fouriera na wykresie częstotliwości.
 */
class Spectrograph : public QWidget
{
    Q_OBJECT

public:
    explicit Spectrograph(QWidget *parent = 0);
    ~Spectrograph();

    /*!
     * \brief Ustawienie parametrów początkowych
     *
     * \param[in] numBars - ilość prążków (barów) na spektografie
     * \param[in] lowFreq - najniższa częstotliwość
     * \param[in] highFreq - najwyższa częstotliwość
     */
    void setParams(int numBars, qreal lowFreq, qreal highFreq);

    /*!
     * \brief Rysowanie prążków spektografu
     *
     * Wypełnia widget kolorem czarnym, pobiera liczbe prążków, tworzy gradient, a następnie rysuje pojedynczo prążki za pomocą czteroboków
     */
    void paintEvent(QPaintEvent *event) override;

    /*!
     * \brief Ustawienie gradientu
     *
     * Zmiana gradientu na odpowiedni przesłany jako argument metody
     * \param[in] gradient - string ciągu wartości RGB w jednostkach hexadecymalnych
     */
    void setGradient(QString gradient);

public slots:

    /*!
     * \brief Resetuje spektograf
     */
    void reset();

    /*!
     * \brief Spektograf uległ zmianie
     *
     * Stare dane są zastępywane przez nowe, a następnie spektograf jest aktualizowany
     * \param[in] spectrum - nowy zestaw danych widma audio
     */
    void spectrumChanged(const FrequencySpectrum &spectrum);

private:

    /*!
     * \brief Wyliczenie indeksu prążka na podstawie częstotliwości
     *
     * \param[in] frequency - częstotliwość wejściowa, z której wyliczany jest indeks prążka
     * \param[out] indeks prążka
     */
    int barIndex(qreal frequency, qreal lower, qreal higher) const;

    /*!
     * \brief Wypełnienie pustych prążków
     */
    void fillBars();

    /*!
     * \brief Wygładza sąsiednie prążki
     */
    void smoothBars();

    /*!
     * \brief Dodaje opóźnienie rośnięcia i opadania prążków w zależności od poprzednich ich wartości
     */
    void addDelay();

    /*!
     * \brief Oblicz kolor w pozycji gradientu za pomocą interpolacji
     *
     * \param[in] value - wartość double pozycji w zakresie [0.0, 1.0]
     * \param[out] color - kolor w postaci RGBA
     */
    QColor getColor(double value);

    /*!
     * \brief Metoda interpolacji koloru
     *
     * \param[in] start - kolor po stronie lewej, początkowy
     * \param[in] end - kolor po stronie prawej, końcowy
     * \param[in] ratio - wartość intensywności koloru pierwszego w stosunku do drugiego
     * \param[out] color - wynik interpolacji w postaci RGBA
     */
    QColor interpolate(QColor start, QColor end, double ratio);

    /*!
     * \brief Wyznaczenie wartości na podstawie basów
     *
     * Metoda wyznaczający wartość w zakresie od [0.0, 1.0] na podstawie amplitudy dolnych pasm częstotliwości
     * \param[out] - wartość amplitudy basu [0.1, 0.6] w zakresie [0.0, 1.0]
     */
    double bass();

    /*!
     * \brief Zaktualizowanie wartości i indeksów prążków
     */
    void updateBars();

private:
    /*!
     * \brief Klasa Bar
     *
     * przedstawia prążek na spektografie
     */
    struct Bar {
        Bar() : value(0.0), clipped(false) { }
        qreal   value;
        bool    clipped;
    };

    QList<Bar>          m_bars;
    QList<Bar>          m_prev_bars;
    QList<QColor>       mGradientColors;
    qreal               m_lowFreq;
    qreal               m_highFreq;
    FrequencySpectrum   m_spectrum;
    QLinearGradient     m_gradient;
};

#endif // SPECTROGRAPH_H
