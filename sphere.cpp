#include "sphere.h"

Sphere::Sphere(Qt3DCore::QEntity *rootEntity, QVector3D vec)
    :   m_rootEntity(rootEntity)
    ,   m_coords(vec)
{
    // Sphere shape data
    m_sphereMesh = new Qt3DExtras::QSphereMesh();
    m_sphereMesh->setRings(3);
    m_sphereMesh->setSlices(3);
    m_sphereMesh->setRadius(0.03f);

    // Sphere mesh transform
    m_sphereTransform = new Qt3DCore::QTransform();

    //m_sphereTransform->setScale(0.3f);
    m_sphereTransform->setTranslation(vec);

    m_sphereMaterial = new Qt3DExtras::QPhongMaterial();
    m_sphereMaterial->setDiffuse(Qt::white);

    // Sphere
    m_sphereEntity = new Qt3DCore::QEntity(m_rootEntity);
    m_sphereEntity->addComponent(m_sphereMesh);
    m_sphereEntity->addComponent(m_sphereMaterial);
    m_sphereEntity->addComponent(m_sphereTransform);

    m_sphereEntity->setEnabled(true);
}

Sphere::~Sphere() = default;

QVector3D Sphere::getCoords()
{
    return m_coords;
}

void Sphere::translateSphere(QVector3D vec)
{
    m_sphereTransform->setTranslation(vec);
}
