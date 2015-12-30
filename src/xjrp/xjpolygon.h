#ifndef XJPolygon_H
#define XJPolygon_H

#include <QList>
#include "xjpoint.h"
#include "xjboundary.h"

namespace XJRP
{

class xjPolygon : public QList<xjPoint>
{
public:
    enum PolygonType
	{
		Contour,
		Infill,
		Support,
    };

    void setType (PolygonType type);
    PolygonType type () const;

    void setLaserPower (const qreal power);
    double laserPower() const;

    void setScanSpeed (const qreal speed);
    double scanSpeed () const;

    void simplify ();
    xjPolygon simplified () const;

    void reverse ();
    xjPolygon reversed () const;

    void close ();
    xjPolygon closed () const;
    bool isClosed () const;

    const Boundary boundary () const;

    const QString toString () const;
    static const xjPolygon fromString (const QString &line);
	
private:
    PolygonType m_type = Contour;
    qreal m_laserPower = -1.0;
    qreal m_scanSpeed = -1.0;
};

}

QDebug operator << (QDebug dbg, const XJRP::xjPolygon &xjPolygon);

QDataStream &operator << (QDataStream &stream, const XJRP::xjPolygon &xjPolygon);
QDataStream &operator >> (QDataStream &stream, XJRP::xjPolygon &xjPolygon);

Q_DECLARE_METATYPE (XJRP::xjPolygon)

#endif // xjPolygon_H
