#include "engine.h"
#include "mainwidget.h"
#include "qcombobox.h"
#include "qspinbox.h"
#include "spectrograph.h"
#include "scene.h"

#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyle>
#include <QMenu>
#include <QFileDialog>
#include <QTimerEvent>
#include <QMessageBox>
#include <iostream>

// Lower bound of first band in the spectrum
const qreal  SpectrumLowFreq        = 20.0; // Hz

// Upper band of last band in the spectrum
const qreal  SpectrumHighFreq       = 20000.0; // Hz

extern int klatki;
int SpectrumNumBands = 128;

bool _2D = true;

std::vector<QString> gradient{"#FF0000 #FF00FF #00FF00 #00FFFF #0000FF",
                                "#0000FF #00FFFF #00FF00 #FF00FF #FF0000"};

static const char* color[] {
                        QT_TRANSLATE_NOOP("Color", "Red"),
                        QT_TRANSLATE_NOOP("Color", "Magenta"),
                        QT_TRANSLATE_NOOP("Color", "Green"),
                        QT_TRANSLATE_NOOP("Color", "Cyan"),
                        QT_TRANSLATE_NOOP("Color", "Blue"),
                        };

MainWidget::MainWidget(QWidget *parent)
    :   QWidget(parent)
    ,   m_engine(new Engine(this))
    ,   m_spectrograph(new Spectrograph(this))
{
    m_spectrograph->setParams(SpectrumNumBands, SpectrumLowFreq, SpectrumHighFreq);

    setWindowTitle(tr("Audio Spectrum"));

    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, "#3f3f3f");
    this->setAutoFillBackground(true);
    this->setPalette(pal);

    m_currentDevice = 0;
    createUi2D();
    m_engine->startRecording();
    connect(m_engine, QOverload<const FrequencySpectrum&>::of(&Engine::spectrumChanged),
            this, QOverload<const FrequencySpectrum&>::of(&MainWidget::spectrumChanged));
    connectUi2D();
    connect(m_engine, &Engine::devicesChanged,
            this, &MainWidget::inputDevicesChanged);
}

MainWidget::~MainWidget() = default;


//-----------------------------------------------------------------------------
// Public slots
//-----------------------------------------------------------------------------

void MainWidget::spectrumChanged(const FrequencySpectrum &spectrum)
{
    if (_2D)
    {
        m_spectrograph->spectrumChanged(spectrum);
    }
    else
    {
        m_scene->spectrumChanged(spectrum);
    }
}

void MainWidget::barsChanged(const int bars)
{
    SpectrumNumBands = bars;
    m_spectrograph->setParams(SpectrumNumBands, SpectrumLowFreq, SpectrumHighFreq);
}

void MainWidget::FPSchanged(const int fps)
{
    m_engine->FPSchanged(fps);
}

void MainWidget::gradientChanged(const int index)
{
    m_spectrograph->setGradient(gradient[index]);
}

void MainWidget::colorChanged(const int index)
{
    m_scene->setColor(index);
}

void MainWidget::deviceChanged(const int index)
{
    m_currentDevice = index;
    const QList<QAudioDevice> device = m_engine->availableAudioInputDevices();
    m_engine->setAudioInputDevice(device[index]);
}

void MainWidget::inputDevicesChanged()
{
    m_InputDevices->clear();
    for (const QAudioDevice &device : m_engine->availableAudioInputDevices())
        m_InputDevices->addItem(device.description(),
                                QVariant::fromValue(device));
    m_InputDevices->setCurrentText(m_engine->audioInputDeviceSelected().description());
}

//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void MainWidget::createUi2D()
{
    m_plButton = new QPushButton(this);
    m_usButton = new QPushButton(this);
    m_3DswitchButton = new QPushButton(this);
    m_FPScount = new QSpinBox(this);
    m_Bars = new QSpinBox(this);
    m_Gradient = new QComboBox(this);
    m_InputDevices = new QComboBox(this);

    QHBoxLayout* windowLayout = new QHBoxLayout(this);

    // Spectrograph
    windowLayout->addWidget(m_spectrograph);

    // Button panel
    const QSize LanguageButtonSize(40, 20);
    const QSize SmallerButtonSize(30, 30);
    const QSize BiggerButtonSize(140, 30);

    QFile styleFile(":pro/style.qss");
    styleFile.open(QFile::ReadOnly);
    QString style(styleFile.readAll());
    styleFile.close();

    QPixmap pixmap_pl(":/images/pl_flag.png");
    pixmap_pl = pixmap_pl.scaled(LanguageButtonSize.width(), LanguageButtonSize.height(), Qt::IgnoreAspectRatio);
    m_plButton->setIcon(QIcon(pixmap_pl));
    m_plButton->setIconSize(LanguageButtonSize);
    m_plButton->setEnabled(true);
    m_plButton->setFixedSize(LanguageButtonSize);

    QPixmap pixmap_us(":/images/us_flag.png");
    pixmap_us = pixmap_us.scaled(LanguageButtonSize.width(), LanguageButtonSize.height(), Qt::IgnoreAspectRatio);
    m_usButton->setIcon(QIcon(pixmap_us));
    m_usButton->setIconSize(LanguageButtonSize);
    m_usButton->setEnabled(true);
    m_usButton->setFixedSize(LanguageButtonSize);

    m_3DswitchButton->setText("3D");
    m_3DswitchButton->setStyleSheet(style);
    m_3DswitchButton->setEnabled(true);
    m_3DswitchButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_3DswitchButton->setMinimumSize(SmallerButtonSize);

    m_FPScount->setStyleSheet(style);
    m_FPScount->setEnabled(true);
    m_FPScount->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_FPScount->setMinimumSize(SmallerButtonSize);
    m_FPScount->setRange(1, 120);
    m_FPScount->setSuffix(tr(" fps"));
    m_FPScount->setValue(klatki);

    m_InputDevices->setEnabled(true);
    m_InputDevices->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_InputDevices->setStyleSheet(style);
    m_InputDevices->setMinimumSize(BiggerButtonSize);

    m_Gradient->setStyleSheet(style);
    m_Gradient->setEnabled(true);
    m_Gradient->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_Gradient->setMinimumSize(BiggerButtonSize);

    m_Bars->setStyleSheet(style);
    m_Bars->setEnabled(true);
    m_Bars->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_Bars->setMinimumSize(BiggerButtonSize);
    m_Bars->setRange(32, 1024);
    m_Bars->setSuffix(tr(" bars"));
    m_Bars->setValue(SpectrumNumBands);

    // 1st Line
    std::unique_ptr<QHBoxLayout> buttonPanelLayout1(new QHBoxLayout);
    buttonPanelLayout1->setAlignment(Qt::AlignRight);
    buttonPanelLayout1->setSpacing(10);
    buttonPanelLayout1->addWidget(m_plButton);
    buttonPanelLayout1->addWidget(m_usButton);

    QWidget *buttonPanel1 = new QWidget(this);
    buttonPanel1->setContentsMargins(0, 0, 0, 5);
    buttonPanel1->setLayout(buttonPanelLayout1.release());

    // 2nd Line
    std::unique_ptr<QHBoxLayout> buttonPanelLayout2(new QHBoxLayout);
    buttonPanelLayout2->addWidget(m_3DswitchButton);
    buttonPanelLayout2->addWidget(m_FPScount);

    QWidget *buttonPanel2 = new QWidget(this);
    buttonPanel2->setContentsMargins(0, 0, 0, 5);
    buttonPanel2->setLayout(buttonPanelLayout2.release());

    // 3th Line
    std::unique_ptr<QHBoxLayout> buttonPanelLayout3(new QHBoxLayout);
    for (const QAudioDevice &device : m_engine->availableAudioInputDevices())
    {
        m_InputDevices->addItem(device.description(),
                                       QVariant::fromValue(device));
    }
    m_InputDevices->setCurrentIndex(m_currentDevice);
    buttonPanelLayout3->addWidget(m_InputDevices);

    QWidget *buttonPanel3 = new QWidget(this);
    buttonPanel3->setContentsMargins(0, 0, 0, 5);
    buttonPanel3->setLayout(buttonPanelLayout3.release());

    // 4th Line
    std::unique_ptr<QHBoxLayout> buttonPanelLayout4(new QHBoxLayout);
    for (int i = 1; i <= gradient.size(); ++i)
    {
        QString name = "Gradient " + QString::number(i);
        m_Gradient->addItem(name);
    }
    buttonPanelLayout4->addWidget(m_Gradient);

    QWidget *buttonPanel4 = new QWidget(this);
    buttonPanel4->setContentsMargins(0, 0, 0, -8);
    buttonPanel4->setLayout(buttonPanelLayout4.release());

    // 5th Line
    std::unique_ptr<QHBoxLayout> buttonPanelLayout5(new QHBoxLayout);
    buttonPanelLayout5->addWidget(m_Bars);

    QWidget *buttonPanel5 = new QWidget(this);
    buttonPanel5->setContentsMargins(0, 0, -8, 0);
    buttonPanel5->setLayout(buttonPanelLayout5.release());

    // Combine
    std::unique_ptr<QVBoxLayout> Buttons(new QVBoxLayout);
    Buttons->addWidget(buttonPanel1);
    Buttons->addWidget(buttonPanel2);
    Buttons->addWidget(buttonPanel3);
    Buttons->addWidget(buttonPanel4);
    Buttons->addWidget(buttonPanel5);
    Buttons->setAlignment(Qt::AlignHCenter);
    Buttons->setAlignment(Qt::AlignTop);

    QWidget *buttonPanel = new QWidget(this);
    buttonPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    buttonPanel->setLayout(Buttons.release());

    windowLayout->addWidget(buttonPanel);

    setLayout(windowLayout);
}

void MainWidget::createUi3D()
{
    m_plButton = new QPushButton(this);
    m_usButton = new QPushButton(this);
    m_2DswitchButton = new QPushButton(this);
    m_FPScount = new QSpinBox(this);
    m_Color = new QComboBox(this);
    m_InputDevices = new QComboBox(this);

    QHBoxLayout* windowLayout = new QHBoxLayout(this);

    // 3D scene
    container = QWidget::createWindowContainer(view);
    container->setMinimumSize(QSize(800, 400)); // w, h
    windowLayout->addWidget(container);

    m_scene->setParams(SpectrumLowFreq, SpectrumHighFreq);

    // Button panel
    const QSize LanguageButtonSize(40, 20);
    const QSize SmallerButtonSize(30, 30);
    const QSize BiggerButtonSize(140, 30);

    QFile styleFile(":pro/style.qss");
    styleFile.open(QFile::ReadOnly);
    QString style(styleFile.readAll());
    styleFile.close();

    QPixmap pixmap_pl(":/images/pl_flag.png");
    pixmap_pl = pixmap_pl.scaled(LanguageButtonSize.width(), LanguageButtonSize.height(), Qt::IgnoreAspectRatio);
    m_plButton->setIcon(QIcon(pixmap_pl));
    m_plButton->setIconSize(LanguageButtonSize);
    m_plButton->setEnabled(true);
    m_plButton->setFixedSize(LanguageButtonSize);

    QPixmap pixmap_us(":/images/us_flag.png");
    pixmap_us = pixmap_us.scaled(LanguageButtonSize.width(), LanguageButtonSize.height(), Qt::IgnoreAspectRatio);
    m_usButton->setIcon(QIcon(pixmap_us));
    m_usButton->setIconSize(LanguageButtonSize);
    m_usButton->setEnabled(true);
    m_usButton->setFixedSize(LanguageButtonSize);

    m_2DswitchButton->setText("2D");
    m_2DswitchButton->setStyleSheet(style);
    m_2DswitchButton->setEnabled(true);
    m_2DswitchButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_2DswitchButton->setMinimumSize(SmallerButtonSize);

    m_FPScount->setStyleSheet(style);
    m_FPScount->setEnabled(true);
    m_FPScount->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_FPScount->setMinimumSize(SmallerButtonSize);
    m_FPScount->setRange(1, 120);
    m_FPScount->setSuffix(tr(" fps"));
    m_FPScount->setValue(klatki);

    m_InputDevices->setEnabled(true);
    m_InputDevices->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_InputDevices->setStyleSheet(style);
    m_InputDevices->setMinimumSize(BiggerButtonSize);

    m_Color->setStyleSheet(style);
    m_Color->setEnabled(true);
    m_Color->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_Color->setMinimumSize(BiggerButtonSize);

    // 1st Line
    std::unique_ptr<QHBoxLayout> buttonPanelLayout1(new QHBoxLayout);
    buttonPanelLayout1->setAlignment(Qt::AlignRight);
    buttonPanelLayout1->setSpacing(10);
    buttonPanelLayout1->addWidget(m_plButton);
    buttonPanelLayout1->addWidget(m_usButton);

    QWidget *buttonPanel1 = new QWidget(this);
    buttonPanel1->setContentsMargins(0, 0, 0, 5);
    buttonPanel1->setLayout(buttonPanelLayout1.release());

    // 2nd Line
    std::unique_ptr<QHBoxLayout> buttonPanelLayout2(new QHBoxLayout);
    buttonPanelLayout2->addWidget(m_2DswitchButton);
    buttonPanelLayout2->addWidget(m_FPScount);

    QWidget *buttonPanel2 = new QWidget(this);
    buttonPanel2->setContentsMargins(0, 0, 0, 5);
    buttonPanel2->setLayout(buttonPanelLayout2.release());

    // 3th Line
    std::unique_ptr<QHBoxLayout> buttonPanelLayout3(new QHBoxLayout);
    for (const QAudioDevice &device : m_engine->availableAudioInputDevices())
    {
        m_InputDevices->addItem(device.description(),
                                QVariant::fromValue(device));
    }
    m_InputDevices->setCurrentIndex(m_currentDevice);
    buttonPanelLayout3->addWidget(m_InputDevices);

    QWidget *buttonPanel3 = new QWidget(this);
    buttonPanel3->setContentsMargins(0, 0, 0, 5);
    buttonPanel3->setLayout(buttonPanelLayout3.release());

    // 4th Line
    std::unique_ptr<QHBoxLayout> buttonPanelLayout4(new QHBoxLayout);
    for (const char* colorName : color)
    {
        m_Color->addItem(QCoreApplication::translate("Color", colorName));
    }
    buttonPanelLayout4->addWidget(m_Color);

    QWidget *buttonPanel4 = new QWidget(this);
    buttonPanel4->setContentsMargins(0, 0, 0, -8);
    buttonPanel4->setLayout(buttonPanelLayout4.release());

    // Combine
    std::unique_ptr<QVBoxLayout> Buttons(new QVBoxLayout);
    Buttons->addWidget(buttonPanel1);
    Buttons->addWidget(buttonPanel2);
    Buttons->addWidget(buttonPanel3);
    Buttons->addWidget(buttonPanel4);
    Buttons->setAlignment(Qt::AlignHCenter);
    Buttons->setAlignment(Qt::AlignTop);

    QWidget *buttonPanel = new QWidget(this);
    buttonPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    buttonPanel->setLayout(Buttons.release());

    windowLayout->addWidget(buttonPanel);

    setLayout(windowLayout);
}

void MainWidget::connectUi2D()
{
    connect(m_plButton, &QPushButton::clicked,
            this, &MainWidget::switchLanguage);

    connect(m_usButton, &QPushButton::clicked,
            this, &MainWidget::switchLanguage);

    connect(m_3DswitchButton, &QPushButton::clicked,
            this, &MainWidget::switch3D);

    connect(m_FPScount, &QSpinBox::valueChanged,
            this, &MainWidget::FPSchanged);

    connect(m_Bars, &QSpinBox::valueChanged,
            this, &MainWidget::barsChanged);

    connect(m_Gradient, &QComboBox::currentIndexChanged,
            this, &MainWidget::gradientChanged);

    connect(m_InputDevices, &QComboBox::activated,
            this, &MainWidget::deviceChanged);
}

void MainWidget::connectUi3D()
{
    connect(m_plButton, &QPushButton::clicked,
            this, &MainWidget::switchLanguage);

    connect(m_usButton, &QPushButton::clicked,
            this, &MainWidget::switchLanguage);

    connect(m_2DswitchButton, &QPushButton::clicked,
            this, &MainWidget::switch2D);

    connect(m_FPScount, &QSpinBox::valueChanged,
            this, &MainWidget::FPSchanged);

    connect(m_Color, &QComboBox::currentIndexChanged,
            this, &MainWidget::colorChanged);

    connect(m_InputDevices, &QComboBox::activated,
            this, &MainWidget::deviceChanged);
}

void MainWidget::clearLayout(QLayout *layout)
{
    if (layout == NULL)
    {
        return;
    }

    QLayoutItem *item;
    while((item = layout->takeAt(0)))
    {
        if (item->layout())
        {
            clearLayout(item->layout());
            delete item->layout();
        }
        if (item->widget())
        {
            delete item->widget();
        }
        delete item;
    }
    delete layout;
}

void MainWidget::switchLanguage()
{
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());

    if (clickedButton == m_plButton)
    {
        m_translator = new QTranslator();

        if(m_translator->load(":pro/AudioSpectrum_pl_PL.qm"))
        {
            qApp->installTranslator(m_translator);
        }
    }
    else
    {
        qApp->removeTranslator(m_translator);
    }

    setWindowTitle(tr("Audio Spectrum"));

    if (_2D)
    {
        m_Bars->setSuffix(tr(" bars"));
    }
    else
    {
        int curr_idx = m_Color->currentIndex();
        m_Color->clear();
        for (const char* colorName : color)
        {
            m_Color->addItem(QCoreApplication::translate("Color", colorName));
        }
        m_Color->setCurrentIndex(curr_idx);
    }
}

void MainWidget::switch3D()
{
    m_engine->suspend();
    m_engine->stopRecording();

    clearLayout(layout());

    disconnect(m_plButton, nullptr, nullptr, nullptr);
    disconnect(m_usButton, nullptr, nullptr, nullptr);
    disconnect(m_3DswitchButton, nullptr, nullptr, nullptr);
    disconnect(m_FPScount, nullptr, nullptr, nullptr);
    disconnect(m_Bars, nullptr, nullptr, nullptr);
    disconnect(m_Gradient, nullptr, nullptr, nullptr);

    _2D = !_2D;

    view = new Qt3DExtras::Qt3DWindow();
    m_scene = new Scene(view);

    createUi3D();
    connectUi3D();
    m_engine->startRecording();
}

void MainWidget::switch2D()
{
    m_scene->pause();
    m_engine->suspend();
    m_engine->stopRecording();

    clearLayout(layout());

    disconnect(m_plButton, nullptr, nullptr, nullptr);
    disconnect(m_usButton, nullptr, nullptr, nullptr);
    disconnect(m_2DswitchButton, nullptr, nullptr, nullptr);
    disconnect(m_FPScount, nullptr, nullptr, nullptr);
    disconnect(m_Color, nullptr, nullptr, nullptr);

    _2D = !_2D;

    m_spectrograph = new Spectrograph(this);
    m_spectrograph->setParams(SpectrumNumBands, SpectrumLowFreq, SpectrumHighFreq);

    createUi2D();
    connectUi2D();
    m_engine->startRecording();
}
