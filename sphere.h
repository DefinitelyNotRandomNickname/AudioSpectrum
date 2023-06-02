#ifndef SPHERE_H
#define SPHERE_H

#include <QWidget>
#include <QtCore/QObject>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QTextureMaterial>

/*!
 * \brief Klasa Sphere
 *
 * Odpowiedzialna za sferę w scenie 3D
 */
class Sphere : public QWidget
{
    Q_OBJECT

public:
    explicit Sphere(Qt3DCore::QEntity *rootEntity, QVector3D vec);
    ~Sphere();

    /*!
     * \brief Metoda odpowiedzialna za zwrócenie koordynatów początkowych 3D sfery
     *
     * \param[out] QVector3D - zwraca wektor 3D z koordynatami sfery
     */
    QVector3D getCoords();

    /*!
     * \brief Przesuwanie koordynatów sfery o zadany wektor
     *
     * \param[in] vec - wektor, o który przesuwamy sferę
     */
    void translateSphere(QVector3D vec);

private:

    Qt3DCore::QEntity*          m_rootEntity;
    Qt3DCore::QEntity*          m_sphereEntity;

    Qt3DExtras::QSphereMesh*    m_sphereMesh;
    Qt3DExtras::QPhongMaterial* m_sphereMaterial;
    Qt3DCore::QTransform*       m_sphereTransform;

    QVector3D                   m_coords;
};

#endif // SPHERE_H
