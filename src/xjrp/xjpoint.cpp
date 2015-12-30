#include "xjPoint.h"
#include <QDebug>

namespace XJRP {

xjPoint::xjPoint ()
    : m_x (NAN), m_y (NAN), m_z (NAN)
{}

xjPoint::xjPoint(qreal x, qreal y)
    : m_x (x), m_y (y), m_z (NAN)
{}

xjPoint::xjPoint (qreal x, qreal y, qreal z)
    : m_x (x), m_y (y), m_z (z)
{}

xjPoint::xjPoint (const xjPoint &p)
    : m_x (p.m_x), m_y (p.m_y), m_z (p.m_z)
{}

void xjPoint::setValue (qreal x, qreal y, qreal z)
{
    m_x = x;
    m_y = y;
    m_z = z;
}

void xjPoint::setValue(const xjPoint &p)
{
    m_x = p.m_x;
    m_y = p.m_y;
    m_z = p.m_z;
}

void xjPoint::setX (qreal x)
{
    m_x = x;
}

void xjPoint::setY (qreal y)
{
    m_y = y;
}

void xjPoint::setZ (qreal z)
{
    m_z = z;
}

#ifdef use_color
void xjPoint::setColor(int r,int g,int b,int a)
{
    this->m_color=QColor(r,g,b,a);
}
QColor xjPoint::color() const
{
    return m_color;
}
#endif

qreal xjPoint::length () const
{
    return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
}

qreal xjPoint::x () const
{
    return m_x;
}

qreal xjPoint::y () const
{
    return m_y;
}

qreal xjPoint::z () const
{
    return m_z;
}

xjPoint &xjPoint::operator += (const xjPoint &p)
{
    m_x += p.m_x;
    m_y += p.m_y;
    m_z += p.m_z;
    return *this;
}

xjPoint &xjPoint::operator -= (const xjPoint &p)
{
    m_x -= p.m_x;
    m_y -= p.m_y;
    m_z -= p.m_z;
    return *this;
}

xjPoint &xjPoint::operator *= (const qreal scale)
{
    m_x *= scale;
    m_y *= scale;
    m_z *= scale;
    return *this;
}

xjPoint &xjPoint::operator *= (const xjPoint &p)
{
    m_x *= p.m_x;
    m_y *= p.m_y;
    m_z *= p.m_z;
    return *this;
}

xjPoint &xjPoint::operator /= (const qreal scale)
{
    if (!fuzzyIsNull (scale))
    {
        m_x /= scale;
        m_y /= scale;
        m_z /= scale;
    }
    return *this;
}

xjPoint &xjPoint::operator /= (const xjPoint &p)
{
    if (!fuzzyIsNull (p.m_x) && !fuzzyIsNull (p.m_y) && !fuzzyIsNull (p.m_z))
    {
        m_x /= p.m_x;
        m_y /= p.m_y;
        m_z /= p.m_z;
    }
    return *this;
}

bool xjPoint::isValid () const
{
    bool valid = std::isfinite (m_x) && std::isfinite (m_y) && std::isfinite (m_z);
    return valid;
}

bool xjPoint::isZero () const
{
    bool isZero = (fuzzyIsNull (m_x) && fuzzyIsNull (m_y) && fuzzyIsNull (m_z));
    return isZero;
}

const QString xjPoint::toString () const
{
    QString line (QString ("(%1,%2,%3)").arg ((double) m_x).arg ((double) m_y).arg ((double) m_z));
    return line;
}

const xjPoint xjPoint::fromString (const QString &line)
{
    QString _line (line.trimmed ());
    xjPoint p;
    if (_line.startsWith ('(') && _line.endsWith (')'))
    {
        _line.remove ('(');
        _line.remove (')');
        QStringList list (_line.split (','));
        if (list.size () == 3)
        {
            p.setValue (list.at (0).toDouble (),
                        list.at (1).toDouble (),
                        list.at (2).toDouble ());
        }
    }
    return p;
}

const xjPoint xjPoint::zero()
{
    return xjPoint (0.0, 0.0, 0.0);
}

}  //结束xjrp

bool operator == (const XJRP::xjPoint &p, const XJRP::xjPoint &q)
{
    return (fuzzyIsNull (p.x () - q.x ()) && fuzzyIsNull (p.y () - q.y ()) && fuzzyIsNull (p.z () - q.z ()));
}

bool operator != (const XJRP::xjPoint &p, const XJRP::xjPoint &q)
{
    return ! operator == (p, q);
}

const XJRP::xjPoint operator+ (const XJRP::xjPoint &p, const XJRP::xjPoint &q)
{
    XJRP::xjPoint t (p);
    t.operator += (q);
    return t;
}

const XJRP::xjPoint operator- (const XJRP::xjPoint &p, const XJRP::xjPoint &q)
{
    XJRP::xjPoint t (p);
    t.operator -= (q);
    return t;
}

const XJRP::xjPoint operator* (const XJRP::xjPoint &p, const qreal scale)
{
    XJRP::xjPoint t (p);
    t.operator *=(scale);
    return t;
}

const XJRP::xjPoint operator* (const XJRP::xjPoint &p, const XJRP::xjPoint &q)
{
    XJRP::xjPoint t (p);
    t.operator *=(q);
    return t;
}

const XJRP::xjPoint operator/ (const XJRP::xjPoint &p, const qreal scale)
{
    XJRP::xjPoint t (p);
    t.operator /= (scale);
    return t;
}

const XJRP::xjPoint operator/ (const XJRP::xjPoint &p, const XJRP::xjPoint &q)
{
    XJRP::xjPoint t (p);
    t.operator /= (q);
    return t;
}

QDebug operator << (QDebug dbg, const XJRP::xjPoint &xjPoint)
{
    dbg.noquote () << xjPoint.toString ();
    return dbg.quote ();
}

QDataStream &operator << (QDataStream &stream, const XJRP::xjPoint &p)
{
    stream << p.x();
    stream << p.y();
    stream << p.z();
    return stream;
}

QDataStream &operator >> (QDataStream &stream, XJRP::xjPoint &p)
{
    qreal x,y,z;
    stream >> x;
    stream >> y;
    stream >> z;
    p.setValue(x,y,z);
    return stream;
}


