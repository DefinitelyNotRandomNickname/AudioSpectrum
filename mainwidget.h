#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "qt3dwindow.h"
#include "qtranslator.h"
#include <QAudioDevice>
#include <QIcon>
#include <QWidget>

class Engine;
class FrequencySpectrum;
class Spectrograph;
class Scene;

class QAction;
class QLabel;
class QMenu;
class QPushButton;
class QSpinBox;
class QComboBox;

/*!
 * \brief Main widget of the application. It is responsible for UI, Engine and changing between scenes.
 */
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

public slots:

    /*!
     * \brief Spectrum has changed
     *
     * \param[in] spectrum - new spectrum
     */
    void spectrumChanged(const FrequencySpectrum &spectrum);

    /*!
     * \brief Bars amount has changed
     *
     * \param[in] bars - new bars amount
     */
    void barsChanged(const int bars);

    /*!
     * \brief FPS amount has changed
     *
     * \param[in] fps - new FPS amount
     */
    void FPSchanged(const int fps);

    /*!
     * \brief New gradient has been selected
     *
     * \param[in] index - index of selected gradient
     */
    void gradientChanged(const int index);

    /*!
     * \brief New sphere color has been selected
     *
     * \param[in] index - index of selected color
     */
    void colorChanged(const int index);

    /*!
     * \brief New audio device has been selected
     *
     * \param[in] index - indeks wybranego urządzenia audio
     */
    void deviceChanged(const int index);

    /*!
     * \brief Audio device list has been refreshed
     */
    void inputDevicesChanged();

private:
    /*!
     * \brief Creates 2D UI
     */
    void createUi2D();

    /*!
     * \brief Creates 3D UI
     */
    void createUi3D();

    /*!
     * \brief Connects 2D signals
     */
    void connectUi2D();

    /*!
     * \brief Connects 3D signals
     */
    void connectUi3D();

    /*!
     * \brief Clears layout
     *
     * \param[in] layout - layout to be cleared
     */
    void clearLayout(QLayout *layout);

    void switchLanguage();

    /*!
     * \brief Switches from 3D to 2D
     */
    void switch2D();

    /*!
     * \brief Switched from 2D to 3D
     */
    void switch3D();

private:
    QTranslator*            m_translator;

    Engine*                 m_engine;
    int                     m_currentDevice;

    Spectrograph*           m_spectrograph;
    Scene*                  m_scene;
    Qt3DExtras::Qt3DWindow* view;
    QWidget*                container;

    QPushButton*            m_plButton;
    QPushButton*            m_usButton;
    QPushButton*            m_3DswitchButton;
    QPushButton*            m_2DswitchButton;
    QSpinBox*               m_FPScount;
    QSpinBox*               m_Bars;
    QComboBox*              m_InputDevices;
    QComboBox*              m_Gradient;
    QComboBox*              m_Color;
};

#endif // MAINWIDGET_H
