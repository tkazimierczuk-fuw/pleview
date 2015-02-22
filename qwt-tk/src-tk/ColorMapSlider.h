#ifndef _COLORMAP_SLIDER_H_
#define _COLORMAP_SLIDER_H_

#include <QWidget>
#include <QMap>
#include <QtGui>
#include <QGradient>
#include <QBrush>
#include <QGroupBox>
#include <QLineEdit>

#include "colorbutton.h"
#include "ColorMap.h"

#include <iostream>

/**
*/


class ColorMapSlider : public QWidget
{
    Q_OBJECT

public:
    explicit ColorMapSlider(QWidget *parent = 0, const ColorMap &_map = ColorMap()) {
        QMap<double, QRgb> tmpMap = _map.rawMap();
        QMap<double, QRgb>::const_iterator it = tmpMap.begin();
        while(it != tmpMap.end()) {
            map.insert(it.key(), it.value());
            it++;
        }

        stopWidth = 5;
        setFocusPolicy(Qt::TabFocus);
        mouseOffset = 0; /* initialization not necessary */
        selected = -1;
    }
    virtual ~ColorMapSlider() {}
    QColor selectedColor();
    double selectedStop();
    ColorMap currentMap();

public slots:
    void addStop(double val, const QColor &color);
    bool selectStop(double val);
    void moveStop(double val);
    void removeStop();
    void changeSelectedColor(const QColor &color);

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    virtual void paintEvent (QPaintEvent *e);

private:
    double mapMouse(const QPoint &pos);
    double mapValue(double value);
    QMap<double, QColor> map;
    double stopWidth;
    double mouseOffset;
    double selected;

signals:
    void mapChanged();
};



class ColorMapConfig : public QWidget {
    Q_OBJECT
public:
    ColorMapConfig(QWidget *parent = 0, const ColorMap &initial = ColorMap());
    ColorMap currentColorMap() const;

public slots:
    void setColorMap(const ColorMap & map);
    void cancel();

private:
    void setupUi();
    double valueToPercent(double value) { return (value-_min)/(_max-_min)*100; }
    double percentToValue(double percent) { return _min+(_max-_min)*percent/100.; }

private slots:
    void limitEdited();
    void percentEdited();
    void valueEdited();
    void stopRemoved();
    void mapChanged();
    void setMinColor(const QColor &color);
    void setMaxColor(const QColor &color);

signals:
    void colorMapChanged(const ColorMap &);

private:
    ColorMap oryginal;
    ColorMapSlider * slider;
    QPushButton *removeButton;

    QGroupBox *group;
    ColorButton *buttonStop, *buttonMin, *buttonMax;
    QLineEdit *percentEdit, *valueEdit, *minEdit, *maxEdit;
    double _min, _max;
};

#endif
