#include "scene.h"
#include "utils.h"

Scene::Scene(Qt3DExtras::Qt3DWindow *view)
    :   m_root(view)
{
    m_paused = false;
    for (int i = 0; i < halos_2D; ++i)
    {
        m_amp_prev[i] = 1.0;
        m_amp[i] = 1.0;
    }

    // Root entity
    m_rootEntity = new Qt3DCore::QEntity();

    m_root->defaultFrameGraph()->setClearColor("black");

    // Camera
    m_cameraEntity = m_root->camera();

    m_cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    m_cameraEntity->setPosition(QVector3D(10.0f, 0.0, 0.0));
    m_cameraEntity->setUpVector(QVector3D(1, 0, 1));
    m_cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    // Light
    m_lightEntity = new Qt3DCore::QEntity(m_rootEntity);
    m_light = new Qt3DRender::QPointLight(m_lightEntity);
    m_light->setColor("red");
    m_light->setIntensity(2);
    m_lightEntity->addComponent(m_light);
    m_lightTransform = new Qt3DCore::QTransform(m_lightEntity);
    m_lightTransform->setTranslation(m_cameraEntity->position());
    m_lightEntity->addComponent(m_lightTransform);

    // Set root object of the scene
    view->setRootEntity(m_rootEntity);

    double radius = 3;
    double angle_increment_2D = 170 / halos_2D;
    double angle_increment_3D = 360 / halos_3D;

    // Create sphere 10x24
    for (int i = 0; i < halos_2D; ++i)
    {
        for (int j = 0; j < halos_3D; ++j)
        {
            int angle_2D = i * angle_increment_2D + 10;
            int angle_3D = j * angle_increment_3D;
            double radians_2D = qDegreesToRadians(angle_2D);
            double radians_3D = qDegreesToRadians(angle_3D);

            float pos_x = radius * sin(radians_3D) * sin(radians_2D);
            float pos_y = radius * cos(radians_3D) * sin(radians_2D);
            float pos_z = radius * cos(radians_2D);

            m_sphere[i][j] = new Sphere(m_rootEntity, QVector3D(pos_x, pos_y, pos_z));
        }
    }
}

Scene::~Scene() = default;

void Scene::pause()
{
    m_paused = !m_paused;
}

void Scene::setParams(qreal lowFreq, qreal highFreq)
{
    Q_ASSERT(highFreq > lowFreq);
    m_lowFreq = lowFreq;
    m_highFreq = highFreq;
    updateSphere();
}

void Scene::setColor(const int index)
{
    switch(index)
    {
    // Red
    case 0:
        m_light->setColor("red");
        break;

    // Magenta
    case 1:
        m_light->setColor("magenta");
        break;

    // Green
    case 2:
        m_light->setColor("green");
        break;

    // Cyan
    case 3:
        m_light->setColor("cyan");
        break;

    // Blue
    case 4:
        m_light->setColor("blue");
        break;

    default:
        break;
    }
}

void Scene::updateSphere()
{
    for (int i = 0; i < halos_2D; ++i)
    {
        for (int j = 0; j < halos_3D; ++j)
        {
            QVector3D vec = m_sphere[i][j]->getCoords();
            QVector3D new_vec = vec * m_amp[i];
            m_sphere[i][j]->translateSphere(new_vec);
        }
    }
}

void Scene::reset()
{
    m_spectrum.reset();
    spectrumChanged(m_spectrum);
}

int Scene::barIndex(qreal frequency, qreal lower, qreal higher) const
{
    Q_ASSERT(frequency >= m_lowFreq && frequency < m_highFreq);
    const int index = floor( (log_a(frequency, 100) - log_a(lower, 100)) / (log_a(higher, 100) - log_a(lower, 100)) * halos_2D );
    if (index < 0 || index > halos_2D)
    {
        Q_ASSERT(false);
    }
    return index;
}

void Scene::updateHalos()
{
    for (int i = 0; i < halos_2D; ++i)
    {
        m_amp_prev[i] = m_amp[i];
        m_amp[i] = 1.0;
    }

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
            int idx = barIndex(e.frequency, lower, 19000);
            m_amp[idx] = qMax(m_amp[idx], 1.0 + e.amplitude * bass_amp);
        }
    }
}

qreal Scene::measureAmp(qreal amplitude) const
{
    if (amplitude >= 0.3)
    {
        return (amplitude - 0.03) * bass_amp / (0.7 - 0.03);
    }

    return 0.0;
}

void Scene::smoothBars()
{
    // left sided
    for (int i = 1; i < halos_2D - 1; ++i)
    {
        m_amp[i] = m_amp[i - 1] * 0.2 + m_amp[i] * 0.6 + m_amp[i + 1] * 0.2;
    }

    //right sided
    for (int i = halos_2D - 2; i > 0; --i)
    {
        m_amp[i] = m_amp[i - 1] * 0.2 + m_amp[i] * 0.6 + m_amp[i + 1] * 0.2;
    }
}

void Scene::addDelay()
{
    for (int i = 0; i < halos_2D; ++i)
    {
        m_amp[i] = m_amp[i] + (m_amp_prev[i] - m_amp[i]) * 0.6;
    }
}

void Scene::spectrumChanged(const FrequencySpectrum &spectrum)
{
    if (m_paused == false)
    {
        m_spectrum = spectrum;
        updateHalos();
        smoothBars();
        addDelay();
        updateSphere();
    }
}
