#include "utils.h"

const qint16  PCMS16MaxValue     =  32767;
const quint16 PCMS16MaxAmplitude =  32768; // because minimum is -32768

qreal pcmToReal(qint16 pcm)
{
    return qreal(pcm) / PCMS16MaxAmplitude;
}

double log_a(double liczba, double podstawa)
{
    return log2(liczba) / log2(podstawa);
}
