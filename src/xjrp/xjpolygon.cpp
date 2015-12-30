#include "xjPolygon.h"
#include <algorithm>

namespace XJRP {


void xjPolygon::setType (xjPolygon::PolygonType type)
{
    m_type = type;
}

xjPolygon::PolygonType xjPolygon::type () const
{
    return m_type;
}

void xjPolygon::setLaserPower(const qreal power)
{
    m_laserPower = power;
}

double xjPolygon::laserPower() const
{
    return m_laserPower;
}

void xjPolygon::setScanSpeed(const qreal speed)
{
    m_scanSpeed = speed;
}

double xjPolygon::scanSpeed() const
{
    return m_scanSpeed;
}

void xjPolygon::simplify()
{
    for (int i = count () - 2; i > 0; --i)
    {
        if (count () < 3)
            break;

        xjPoint prevDelta ( (*this)[i] - (*this)[i-1]);
        xjPoint nextDelta ( (*this)[i+1] - (*this)[i]);

        if (nextDelta.isZero ())
        {
            removeAt (i);
            continue;
        }

        qreal prevDeltaLength (prevDelta.length ());
        qreal prevDeltaSlopeX (prevDelta.x () / prevDeltaLength);
        qreal prevDeltaSlopeY (prevDelta.y () / prevDeltaLength);
        qreal prevDeltaSlopeZ (prevDelta.z () / prevDeltaLength);

        qreal nextDeltaLength (nextDelta.length ());
        qreal nextDeltaSlopeX (nextDelta.x () / nextDeltaLength);
        qreal nextDeltaSlopeY (nextDelta.y () / nextDeltaLength);
        qreal nextDeltaSlopeZ (nextDelta.z () / nextDeltaLength);

        if (fuzzyIsNull (prevDeltaSlopeX - nextDeltaSlopeX) &&
            fuzzyIsNull (prevDeltaSlopeY - nextDeltaSlopeY) &&
            fuzzyIsNull (prevDeltaSlopeZ - nextDeltaSlopeZ))
        {
            removeAt (i);
        }
    }
}

xjPolygon xjPolygon::simplified() const
{
    xjPolygon other (*this);
    other.simplify ();
    return other;
}

void xjPolygon::reverse ()
{
    std::reverse(this->begin(), this->end());
}

xjPolygon xjPolygon::reversed () const
{
    xjPolygon other (*this);
    other.reverse();
    return other;
}

void xjPolygon::close ()
{
    if (!isClosed())
    {
        this->push_back (this->front());
    }
}

xjPolygon xjPolygon::closed () const
{
    xjPolygon other (*this);
    other.close();
    return other;
}

bool xjPolygon::isClosed () const
{
    // 空路径亦是闭合的
    bool isClosed (true);
    if (!isEmpty ())
        isClosed = (this->front() == this->back());
    return isClosed;
}

const Boundary xjPolygon::boundary() const
{
    Boundary boundary;
    for (const xjPoint &point : *this)
    {
        boundary.refer (point);
    }
    return boundary;
}

const QString xjPolygon::toString () const
{
    QString line;
    line.append ('[');

    QString typeString;
    switch (type ())
    {
    default:
    case xjPolygon::Contour :
        typeString.append ('C');
        break;
    case xjPolygon::Infill :
        typeString.append ('I');
        break;
    case xjPolygon::Support :
        typeString.append ('S');
        break;
    }
    line.append (typeString);

    for (const xjPoint &point : *this)
    {
        line.append (point.toString ());
    }
    line.append (']');
    return line;
}

const xjPolygon xjPolygon::fromString (const QString &line)
{
    QString _line (line.trimmed ());
    xjPolygon Polygon;

    if (_line.startsWith ('[') && _line.endsWith (']'))
    {
        _line.remove ('[');
        _line.remove (']');
        _line = _line.trimmed ();

        // C(...,...,...)(...,...,...)

        if (_line.size () > 0)
        {
            char c (_line.at (0).toUpper ().toLatin1 ());
            _line.remove (0, 1);
            PolygonType type;
            switch (c)
            {
            default:
            case 'C':
                type = PolygonType::Contour;
                break;
            case 'I':
                type = PolygonType::Infill;
                break;
            case 'S':
                type = PolygonType::Support;
                break;
            }
            Polygon.setType (type);
        }

        if (_line.startsWith ('(') && _line.endsWith (')'))
        {
            _line.remove (0, 1);
            _line.remove (_line.size () - 1, 1);
            QStringList list (_line.split (")("));
            for (const QString &string : list)
            {
                QString _string (string);
                _string.prepend ('(');
                _string.append (')');
                XJRP::xjPoint p (xjPoint::fromString (_string));
                if (p.isValid ())
                    Polygon.append (p);
            }
        }
    }
    return Polygon;
}

}

QDebug operator << (QDebug dbg, const XJRP::xjPolygon &xjPolygon)
{
    dbg.noquote () << xjPolygon.toString ();
    return dbg.quote ();
}

QDataStream &operator << (QDataStream &stream, const XJRP::xjPolygon &xjPolygon)
{
    stream << xjPolygon.type();
    stream << xjPolygon.laserPower();
    stream << xjPolygon.scanSpeed();
    stream << *((QList<XJRP::xjPoint>*)&xjPolygon);
    return stream;
}

QDataStream &operator >> (QDataStream &stream, XJRP::xjPolygon &Polygon)
{
    int type(0);
    double laserPower(0.0), scanSpeed(0.0);
    stream >> type;
    stream >> laserPower;
    stream >> scanSpeed;
    stream >> *((QList<XJRP::xjPoint>*)&Polygon);
    Polygon.setType((XJRP::xjPolygon::PolygonType)type);
    Polygon.setLaserPower(laserPower);
    Polygon.setScanSpeed(scanSpeed);
    return stream;
}
