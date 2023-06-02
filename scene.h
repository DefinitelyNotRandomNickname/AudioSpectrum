#ifndef SCENE_H
#define SCENE_H

#include "sphere.h"
#include "frequencyspectrum.h"

#include <QGuiApplication>

#include <Qt3DRender/qcamera.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qcameralens.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qsceneloader.h>
#include <Qt3DRender/qpointlight.h>

#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DExtras/qforwardrenderer.h>

#include <Qt3DExtras/qt3dwindow.h>

#define halos_2D 20 // horizontal
#define halos_3D 35 // vertical

#define bass_amp 0.3

class Sphere;

/*!
 * \brief Klasa Scene
 *
 * Klasa odpowiedzialna za stworzenie sceny 3D, stworzenie odpowiednich pasów sfer
 * w zależności od zdefiniowanych ich ilości.
 */
class Scene : public Qt3DExtras::Qt3DWindow
{
    Q_OBJECT

public:
    explicit Scene(Qt3DExtras::Qt3DWindow *view);
    ~Scene();

    /*!
     * \brief Przekazanie parametrów sfery
     *
     * \param[in] lowFreq - minimalna częstotliwość audio
     * \param[in] highFreq - maksymalna częstotliwość audio
     */
    void setParams(qreal lowFreq, qreal highFreq);

    /*!
     * \brief Ustawienie koloru światła
     *
     * \param[in] index - indeks koloru światła
     */
    void setColor(const int index);

    /*!
     * \brief Metoda odpowiedzialna za zatrzymanie aktualizowania sceny
     */
    void pause();

public slots:

    /*!
     * \brief Zresetowanie klasy
     */
    void reset();

    /*!
     * \brief Spektrum uległo zmianie
     *
     * \param[in] spectrum - nowe spektrum
     */
    void spectrumChanged(const FrequencySpectrum &spectrum);

private:

    /*!
     * \brief Metoda odpowiedzialna za zaktualizowanie pozycji sfer na scenie 3D
     */
    void updateSphere();

    /*!
     * \brief Metoda odpowiedzialna za obliczenia ideksu pasma sfer na podstawie częstotliwości
     *
     * \param[in] frequency - częstotliwość wejściowa
     * \param[in] lower - najniższa częstotliwość spektrum
     * \param[in] higher - najwyższa częstotliwość spektrum
     * \param[out] int - indeks pasma, do którego pada częstotliwość
     */
    int barIndex(qreal frequency, qreal lower, qreal higher) const;

    /*!
     * \brief Metoda odpowiedzialna za zaktualizowanie pasm sfery
     */
    void updateHalos();

    /*!
     * \brief Metoda odpowiedzialna za sprawdzenie amplitudy pasm spektrum i dobranie odpowiedniego wzmocnienia
     *
     * \param[in] amplitude - amplituda wejściowa
     * \param[out] qreal - wartość wzmocnienia amplitudy w zakresie [0.0, 1.0] * bass_amp
     */
    qreal measureAmp(qreal amplitude) const;

    /*!
     * \brief Wygładzenie pasm sfer
     */
    void smoothBars();

    /*!
     * \brief Dodaje opóźnienie rośnięcia i opadania pasów sfer w zależności od poprzednich ich wartości
     */
    void addDelay();

private:

    Qt3DExtras::Qt3DWindow*  m_root;
    Qt3DCore::QEntity*       m_rootEntity;
    Qt3DRender::QCamera*     m_cameraEntity;
    Qt3DCore::QEntity*       m_lightEntity;
    Qt3DRender::QPointLight* m_light;
    Qt3DCore::QTransform*    m_lightTransform;

    Sphere*                  m_sphere[halos_2D][halos_3D];
    qreal                    m_amp[halos_2D];
    qreal                    m_amp_prev[halos_2D];

    bool                     m_paused;
    qreal                    m_lowFreq;
    qreal                    m_highFreq;
    FrequencySpectrum        m_spectrum;
};

#endif // SCENE_H
