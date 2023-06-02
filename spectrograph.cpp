#include "spectrograph.h"
#include "utils.h"

#include <QPainter>

#include <QtCore/qmath.h>
#include <cmath>

using namespace std;

Spectrograph::Spectrograph(QWidget *parent)
    :   QWidget(parent)
    ,   m_lowFreq(0.0)
    ,   m_highFreq(0.0)
    ,   m_gradient(0,0,rect().width(),0)
{
    setMinimumSize(800, 400); // w, h
}

Spectrograph::~Spectrograph() = default;

void Spectrograph::setParams(int numBars, qreal lowFreq, qreal highFreq)
{
    Q_ASSERT(numBars > 0);
    Q_ASSERT(highFreq > lowFreq);
    m_bars.resize(numBars);
    m_lowFreq = lowFreq;
    m_highFreq = highFreq;
    updateBars();
    m_gradient.setFinalStop(rect().width(),0);
    setGradient("#FF0000 #FF00FF #00FF00 #00FFFF #0000FF");
}

void Spectrograph::setGradient(QString gradient)
{
    QStringList gradients = gradient.split(" ");
    mGradientColors.clear();
    for(int i = 0, len = gradients.count(); i < len; ++i)
    {
        m_gradient.setColorAt((float)(i / (len - 1.0)), gradients.value(i));
        mGradientColors.push_back(gradients.value(i));
    }
}

double Spectrograph::bass()
{
    const int numBars = m_bars.count();

    for (int i = 0, len = numBars / 4; i < len; ++i)
    {
        if (m_bars[i].value >= 0.05)
        {
            return (m_bars[i].value - 0.05) / (0.7 - 0.05);
        }
    }

    return 0.0;
}

QColor Spectrograph::getColor(double value)
{
    // Asume mGradientColors.count()>1 and value=[0,1]
    double stepbase = 1.0/(mGradientColors.count()-1);
    int interval=mGradientColors.count()-1; // to fix 1<=0.99999999;

    // remove begin and end
    for (int i=1; i<mGradientColors.count();i++)
    {
        if(value<=i*stepbase)
        {
            interval=i;
            break;
        }
    }
    double percentage = (value-stepbase*(interval-1))/stepbase;
    QColor color(interpolate(mGradientColors[interval],mGradientColors[interval-1],percentage));
    return color;
}

QColor Spectrograph::interpolate(QColor start,QColor end,double ratio)
{
    int r = (int)(ratio*start.red() + (1-ratio)*end.red());
    int g = (int)(ratio*start.green() + (1-ratio)*end.green());
    int b = (int)(ratio*start.blue() + (1-ratio)*end.blue());
    return QColor::fromRgb(r,g,b);
}

void Spectrograph::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    const int numBars = m_bars.count();

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(m_gradient, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    for (int i = 0; i < numBars; ++i)
    {
        const qreal value = m_bars[i].value;
        painter.drawRect(QRect((i * rect().width() / (numBars - 1)), rect().height() - value * rect().height() - 1, 1, rect().height()));
    }

    double bass_amp = bass();
    if (bass_amp > 0.01)
    {
        QPen pen;
        int height = rect().height() / 4;
        int height_2 = height / 2;
        int width = ceil(numBars / 128.0);
        pen.setWidth(height);
        pen.setColor("red");
        for (int i = 0; i < numBars; i += width)
        {
            const qreal value = m_bars[i].value;
            for (int j = rect().height() - value * rect().height() - 1; j < rect().height(); j += height_2)
            {
                double x = i / (numBars - 1.0);
                QPointF centerPoint((int)(x * rect().width()), j);

                QRadialGradient radialGrad(centerPoint, height_2);
                QColor color = getColor(x).rgb();
                color.setAlpha((int)(15.0 * bass_amp));
                radialGrad.setColorAt(0.0, color);
                color.setAlpha(0);
                radialGrad.setColorAt(1.0, color);

                pen.setBrush(radialGrad);
                painter.setPen(pen);
                painter.drawPoint(centerPoint);
            }
        }
    }
}

void Spectrograph::reset()
{
    m_spectrum.reset();
    spectrumChanged(m_spectrum);
}

void Spectrograph::spectrumChanged(const FrequencySpectrum &spectrum)
{
    m_spectrum = spectrum;
    updateBars();
}

int Spectrograph::barIndex(qreal frequency, qreal lower, qreal higher) const
{
    Q_ASSERT(frequency >= m_lowFreq && frequency < m_highFreq);
    const int index = floor( (log_a(frequency, 100) - log_a(lower, 100)) / (log_a(higher, 100) - log_a(lower, 100)) * m_bars.count() );
    if (index < 0 || index >= m_bars.count())
    {
        Q_ASSERT(false);
    }

    return index;
}

void Spectrograph::fillBars()
{
    int since, to;
    int percentage = int(m_bars.count() * 0.1);
    for (int i = 0, lower = m_bars.count() / 4; i < lower; ++i)
    {
        if (m_bars[i].value != 0 && m_bars[i + 1].value == 0)
        {
            ++i;
            since = i;
            to = i;
            while (m_bars[i].value == 0 && (to - since) <= percentage)
            {
                ++i;
                to = i;
            }
            int len = to - since;

            for (int j = 0; j < len - 1; ++j)
            {
                int val = len - 1 - j;
                m_bars[since + j].value = (m_bars[since - 1].value * val / len) + (m_bars[to].value * (len - val) / len);
            }
            --i;
        }
    }
}

void Spectrograph::smoothBars()
{
    // left sided
    for (int i = 1; i < m_bars.count() - 1; ++i)
    {
        m_bars[i].value = (m_bars[i - 1].value + m_bars[i].value + m_bars[i + 1].value) / 3.0;
    }

    //right sided
    for (int i = m_bars.count() - 2; i > 0; --i)
    {
        m_bars[i].value = (m_bars[i - 1].value + m_bars[i].value + m_bars[i + 1].value) / 3.0;
    }
}

void Spectrograph::addDelay()
{
    for (int i = 0; i < m_bars.count(); ++i)
    {
        m_bars[i].value = m_bars[i].value + (m_prev_bars[i].value - m_bars[i].value) * 0.9;
    }
}

void Spectrograph::updateBars()
{
    m_prev_bars = m_bars;
    m_bars.fill(Bar());
    FrequencySpectrum::const_iterator i = m_spectrum.begin();
    const FrequencySpectrum::const_iterator end = m_spectrum.end();
    qreal lower = 0;
    for ( ; i != end; ++i)
    {
        const FrequencySpectrum::Element e = *i;
        if (e.frequency >= m_lowFreq && e.frequency < m_highFreq)
        {
            if (lower == 0)
            {
                lower = e.frequency;
            }
            Bar &bar = m_bars[barIndex(e.frequency, lower, 20000)];
            bar.value = qMax(bar.value, e.amplitude);
            bar.clipped |= e.clipped;
        }
    }
    fillBars();
    if (m_bars.count() >= 64)
    {
        smoothBars();
    }
    addDelay();

    update();
}
