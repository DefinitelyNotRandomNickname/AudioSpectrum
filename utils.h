#ifndef UTILS_H
#define UTILS_H

#include <QtCore/qglobal.h>
#include <QDebug>

QT_FORWARD_DECLARE_CLASS(QAudioFormat)

//-----------------------------------------------------------------------------
// Miscellaneous utility functions
//-----------------------------------------------------------------------------

// Skala PCM od [-1.0, 1.0]
/*!
 * \brief Przeksztalca wartosc pcm do liczby rzeczywistej
 *
 * \param[in] pcm - wartosc pcm do przeksztalcenia
 * \param[out] qreal - wartosc rzeczywista przeksztalcenia
 */
qreal pcmToReal(qint16 pcm);

/*!
 * \brief Obliczenie logarytmu o zadanej podstawie i wartosci
 *
 * \param[in] liczba - wartosc do logarytmu
 * \param[in] podstawa - podstawa logarytmu
 * \param[out] double - wynik logarytmu
 */
double log_a(double liczba, double podstawa);

// Obliczanie potęgi liczby 2
template<int N> class PowerOfTwo
{
public:
    static const int Result = PowerOfTwo<N-1>::Result * 2;
};

template<> class PowerOfTwo<0>
{
public:
    static const int Result = 1;
};


//-----------------------------------------------------------------------------
// Debug output
//-----------------------------------------------------------------------------

class NullDebug
{
public:
    template <typename T>
    NullDebug& operator<<(const T&) {
        return *this;
    }
};


#define ENGINE_DEBUG qDebug()
#define SPECTRUMANALYSER_DEBUG qDebug()

#endif // UTILS_H
