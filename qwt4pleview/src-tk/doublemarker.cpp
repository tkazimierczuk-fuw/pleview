#include <QtGui>

#include "doublemarker.h"

#include "qwt_painter.h"
#include "qwt_scale_map.h"
#include "qwt_plot_marker.h"
#include "qwt_symbol.h"
#include "qwt_text.h"
#include "qwt_math.h"

class DoubleMarker::PrivateData
{
public:
    PrivateData():
        labelAlignment(Qt::AlignTop | Qt::AlignRight),
        labelOrientation(Qt::Horizontal),
        orientation(Qt::Vertical),
        spacing(2),
        x1Value(0.0),
        x2Value(0.0),
        brush(QBrush(QColor(200,200,200,128)))
    {}

    QwtText label;

    Qt::Alignment labelAlignment;
    Qt::Orientation labelOrientation;
    Qt::Orientation orientation;
    int spacing;

    QPen pen;
    QBrush brush;

    double x1Value, x2Value;
};

//! Sets alignment to Qt::AlignCenter, and style to NoLine
DoubleMarker::DoubleMarker(Qt::Orientation orientation):
    QwtPlotItem(QwtText("Marker"))
{
    d_data = new PrivateData;
    if(orientation == Qt::Horizontal) {
        d_data->orientation = Qt::Horizontal;
    }
    setZ(30.0);
}

//! Destructor
DoubleMarker::~DoubleMarker()
{
    delete d_data;
}

//! Set X Value
void DoubleMarker::setXValues(double x1, double x2)
{
    d_data->x1Value = x1;
    d_data->x2Value = x2;
}

/*!
  Draw the marker

  \param painter Painter
  \param xMap x Scale Map
  \param yMap y Scale Map
  \param canvasRect Contents rect of the canvas in painter coordinates
*/
void DoubleMarker::draw(QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    const QRectF &canvasRect) const
{
    if(d_data->orientation == Qt::Vertical) {
        const int x1 = xMap.transform(d_data->x1Value);
        const int x2 = xMap.transform(d_data->x2Value);
        drawAt(painter, canvasRect, x1, x2);
    }
    else {
        const int y1 = yMap.transform(d_data->x1Value);
        const int y2 = yMap.transform(d_data->x2Value);
        drawAt(painter, canvasRect, y1, y2);
    }
}

/*!
  Draw the marker at a specific position

  \param painter Painter
  \param canvasRect Contents rect of the canvas in painter coordinates
  \param pos Position of the marker in painter coordinates
*/
void DoubleMarker::drawAt(QPainter *painter,
    const QRectF &canvasRect, int x1, int x2) const
{

    if(d_data->orientation == Qt::Vertical) {
        // draw inside of the rectangle
        QwtPainter::fillRect(painter, QRect(x1, canvasRect.top(), x2-x1, canvasRect.height()), d_data->brush);

        // draw lines
        //painter->setPen(QwtPainter::scaledPen(d_data->pen)); - obsolete
        painter->setPen(d_data->pen);
        QwtPainter::drawLine(painter, x1, canvasRect.top(), x1, canvasRect.bottom());
        QwtPainter::drawLine(painter, x2, canvasRect.top(), x2, canvasRect.bottom());

        drawLabel(painter, canvasRect, x2);
    }
    else /* i.e. orientation == Horizontal */ {
        // draw inside of the rectangle
        QwtPainter::fillRect(painter, QRect(canvasRect.left(), x1, canvasRect.width(), x2-x1), d_data->brush);

        // draw lines
        //painter->setPen(QwtPainter::scaledPen(d_data->pen)); - obsolete
        painter->setPen(d_data->pen);
        QwtPainter::drawLine(painter, canvasRect.left(), x1, canvasRect.right(), x1);
        QwtPainter::drawLine(painter, canvasRect.left(), x2, canvasRect.right(), x2);

        drawLabel(painter, canvasRect, x1);
    }
}

void DoubleMarker::drawLabel(QPainter *painter, const QRectF &canvasRect, double x) const
{
    if (d_data->label.isEmpty())
        return;

    int align = d_data->labelAlignment;
    QPoint alignPos(x, x);

    QSize symbolOff(0, 0);

    if(d_data->orientation == Qt::Vertical) {
        if (d_data->labelAlignment & (int) Qt::AlignTop)
        {
            alignPos.setY(canvasRect.top());
            align &= ~Qt::AlignTop;
            align |= Qt::AlignBottom;
        }
        else if (d_data->labelAlignment & (int) Qt::AlignBottom)
        {
            // In HLine-style the x-position is pointless and
            // the alignment flags are relative to the canvas

            alignPos.setY(canvasRect.bottom() - 1);
            align &= ~Qt::AlignBottom;
            align |= Qt::AlignTop;
        }
        else
            alignPos.setY(canvasRect.center().y());
    }
    else
        alignPos.setX(canvasRect.left());

    int pw = d_data->pen.width();
    if ( pw == 0 )
        pw = 1;

    const int xSpacing = 0;
//        QwtPainter::metricsMap().screenToLayoutX(d_data->spacing); TODO
    const int ySpacing = 0;
//        QwtPainter::metricsMap().screenToLayoutY(d_data->spacing); TODO


    int xOff = qMax( (pw + 1) / 2, symbolOff.width() );
    int yOff = qMax( (pw + 1) / 2, symbolOff.height() );

    const QSize textSize = d_data->label.textSize(painter->font()).toSize();

    if ( align & Qt::AlignLeft )
    {
        alignPos.rx() -= xOff + xSpacing;
        if ( d_data->labelOrientation == Qt::Vertical )
            alignPos.rx() -= textSize.height();
        else
            alignPos.rx() -= textSize.width();
    }
    else if ( align & Qt::AlignRight )
    {
        alignPos.rx() += xOff + xSpacing;
    }
    else
    {
        if ( d_data->labelOrientation == Qt::Vertical )
            alignPos.rx() -= textSize.height() / 2;
        else
            alignPos.rx() -= textSize.width() / 2;
    }

    if (align & (int) Qt::AlignTop)
    {
        alignPos.ry() -= yOff + ySpacing;
        if ( d_data->labelOrientation != Qt::Vertical )
            alignPos.ry() -= textSize.height();
    }
    else if (align & (int) Qt::AlignBottom)
    {
        alignPos.ry() += yOff + ySpacing;
        if ( d_data->labelOrientation == Qt::Vertical )
            alignPos.ry() += textSize.width();
    }
    else
    {
        if ( d_data->labelOrientation == Qt::Vertical )
            alignPos.ry() += textSize.width() / 2;
        else
            alignPos.ry() -= textSize.height() / 2;
    }

    painter->translate(alignPos.x(), alignPos.y());
    if ( d_data->labelOrientation == Qt::Vertical )
        painter->rotate(-90.0);

    const QRect textRect(0, 0, textSize.width(), textSize.height());
    d_data->label.draw(painter, textRect);
}

/*!
  \brief Set the label
  \param label label text
  \sa label()
*/
void DoubleMarker::setLabel(const QwtText& label)
{
    if ( label != d_data->label )
    {
        d_data->label = label;
        itemChanged();
    }
}

/*!
  \return the label
  \sa setLabel()
*/
QwtText DoubleMarker::label() const
{
    return d_data->label;
}

void DoubleMarker::setBrush(const QBrush &b) {
    d_data->brush = b;
}

const QBrush & DoubleMarker::brush() const {
    return d_data->brush;
}

/*!
  \brief Set the alignment of the label

  In case of QwtPlotMarker::HLine the alignment is relative to the
  y position of the marker, but the horizontal flags correspond to the
  canvas rectangle. In case of QwtPlotMarker::VLine the alignment is
  relative to the x position of the marker, but the vertical flags
  correspond to the canvas rectangle.

  In all other styles the alignment is relative to the marker's position.

  \param align Alignment. A combination of AlignTop, AlignBottom,
    AlignLeft, AlignRight, AlignCenter, AlgnHCenter,
    AlignVCenter.
  \sa labelAlignment(), labelOrientation()
*/
void DoubleMarker::setLabelAlignment(Qt::Alignment align)
{
    if ( align != d_data->labelAlignment )
    {
        d_data->labelAlignment = align;
        itemChanged();
    }
}

/*!
  \return the label alignment
  \sa setLabelAlignment(), setLabelOrientation()
*/
Qt::Alignment DoubleMarker::labelAlignment() const
{
    return d_data->labelAlignment;
}

/*!
  \brief Set the orientation of the label

  When orientation is Qt::Vertical the label is rotated by 90.0 degrees
  ( from bottom to top ).

  \param orientation Orientation of the label

  \sa labelOrientation(), setLabelAlignment()
*/
void DoubleMarker::setLabelOrientation(Qt::Orientation orientation)
{
    if ( orientation != d_data->labelOrientation )
    {
        d_data->labelOrientation = orientation;
        itemChanged();
    }
}

/*!
  \return the label orientation
  \sa setLabelOrientation(), labelAlignment()
*/
Qt::Orientation DoubleMarker::labelOrientation() const
{
    return d_data->labelOrientation;
}

/*!
  \brief Set the spacing

  When the label is not centered on the marker position, the spacing
  is the distance between the position and the label.

  \param spacing Spacing
  \sa spacing(), setLabelAlignment()
*/
void DoubleMarker::setSpacing(int spacing)
{
    if ( spacing < 0 )
        spacing = 0;

    if ( spacing == d_data->spacing )
        return;

    d_data->spacing = spacing;
    itemChanged();
}

/*!
  \return the spacing
  \sa setSpacing()
*/
int DoubleMarker::spacing() const
{
    return d_data->spacing;
}
