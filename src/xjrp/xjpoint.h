#ifndef XJPoint_H
#define XJPoint_H

#define use_color

#include "xjmath.hpp"
#include <QDebug>
#include <QDataStream>
#ifdef use_color
#include <QColor>
#endif
namespace XJRP
{

class xjPoint
{
public:
    xjPoint ();
    xjPoint (qreal x, qreal y);
    xjPoint (qreal x, qreal y, qreal z);
    xjPoint (const xjPoint &p);

    void setValue (qreal x, qreal y, qreal z);
    void setValue (const xjPoint &p);
    void setX (qreal x);
    void setY (qreal y);
    void setZ (qreal z);
#ifdef use_color
    void setColor(int r,int g,int b,int a=255);
    QColor color() const;
#endif
    qreal length () const;
    qreal x() const;
    qreal y() const;
    qreal z() const;

    xjPoint &operator+= (const xjPoint &p);
    xjPoint &operator-= (const xjPoint &p);
    xjPoint &operator*= (const qreal scale);
    xjPoint &operator*= (const xjPoint &p);
    xjPoint &operator/= (const qreal scale);
    xjPoint &operator/= (const xjPoint &p);

    bool isValid () const;
    bool isZero () const;

    const QString toString () const;
    static const xjPoint fromString (const QString &line);

    static const xjPoint zero ();

private:
    qreal m_x, m_y, m_z;
#ifdef use_color
    QColor m_color;
#endif
};

}

bool operator== (const XJRP::xjPoint &p, const XJRP::xjPoint &q);
bool operator!= (const XJRP::xjPoint &p, const XJRP::xjPoint &q);

const XJRP::xjPoint operator+ (const XJRP::xjPoint &p, const XJRP::xjPoint &q);
const XJRP::xjPoint operator- (const XJRP::xjPoint &p, const XJRP::xjPoint &q);
const XJRP::xjPoint operator* (const XJRP::xjPoint &p, const qreal scale);
const XJRP::xjPoint operator* (const XJRP::xjPoint &p, const XJRP::xjPoint &q);
const XJRP::xjPoint operator/ (const XJRP::xjPoint &p, const qreal scale);
const XJRP::xjPoint operator/ (const XJRP::xjPoint &p, const XJRP::xjPoint &q);

QDebug operator << (QDebug dbg, const XJRP::xjPoint &xjPoint);

QDataStream &operator << (QDataStream &stream, const XJRP::xjPoint &p);
QDataStream &operator >> (QDataStream &stream, XJRP::xjPoint &p);

Q_DECLARE_METATYPE (XJRP::xjPoint)

#endif // xjPoint_H
