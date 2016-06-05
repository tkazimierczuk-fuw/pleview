#ifndef _COLORMAP_SLIDER_H_
#define _COLORMAP_SLIDER_H_

#include <QWidget>
#include <QMap>
#include <QtGui>
#include <QGradient>
#include <QBrush>
#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>

#include "colorbutton.h"
#include "colormap.h"

#include <iostream>
#include <memory>


/**
 * @file
 * This file defines widgets which visualise and allow to manipulate a ColorMap.
 */


/**
 * @brief The ColorMapSlider is a widget which manipulates the colors of the underlying ColorMap
 *
 * This widget does not depend on or influence the limit values of the ColorMap - just the color
 * at given fraction of the range.
 */
class ColorMapSlider : public QWidget
{
    Q_OBJECT

public:
    ColorMapSlider(std::shared_ptr<ColorMap> colormap = std::make_shared<ColorMap>()) : ColorMapSlider(0, colormap) {}

    ColorMapSlider(QWidget *parent, std::shared_ptr<ColorMap> colormap) : QWidget(parent), model(colormap) {
        stopWidth = 5;
        setFocusPolicy(Qt::ClickFocus);
        mouseOffset = 0; // initialization not necessary
        selected = -1;
        connect(colormap.get(), SIGNAL(valueChanged()), this, SLOT(checkSelection()));
        connect(colormap.get(), SIGNAL(valueChanged()), this, SLOT(update()));
    }

    virtual ~ColorMapSlider() = default;

    QColor selectedColor();

    double selectedStop(); //! Returns fraction of the currently selected stop or a silent NaN if no stop is selected

    ColorMap currentMap();

signals:
    void selectionChanged(); //! Signal is emitted when the selected stop is moved or (de)selected


public slots:
    void addStop(double val, const QColor &color);
    bool selectStop(double val);
    void moveStop(double val);
    void removeStop();
    void changeSelectedColor(const QColor &color);


protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void paintEvent (QPaintEvent *e) override;

private slots:
    void checkSelection(); //! Verify if the selected stop still exists

private:
    double mapMouse(const QPoint &pos) const;
    double mapValue(double value) const;
    std::shared_ptr<ColorMap> model;
    double stopWidth;
    double mouseOffset;
    double selected;

};



/**
 * @brief The ColorMapConfig class
 *
 * ColorMapConfig incorporates ColorMapSlider to manipulate the colors and additionally controls
 * to access the limit values.
 *
 * ColorMapConfig has two modes: basic and full. In the basic mode it displays a ColorMapSlider plus buttons
 * to control the limits. The full mode additionally displays controls to more precisely edit the stop currently
 * selected in the slider.
 */
class ColorMapConfig : public QWidget {
    Q_OBJECT
public:
    ColorMapConfig(QWidget *parent, std::shared_ptr<ColorMap> colormap, bool full = true);
    ColorMapConfig(std::shared_ptr<ColorMap> colormap, bool full = true) : ColorMapConfig(0, colormap, full) {}

    ColorMap currentColorMap() const;

    QSize sizeHint() const override { return minimumSizeHint(); }

public slots:
    void cancel();

private:
    void setupUi();

private slots:
    void updateDialog(); //! slot called when the map has changed

    //! Displays a dialog with more editing options (particularly useful if ColorMapConfig works in basic mode)
    void displayDialog();

private:
    std::shared_ptr<ColorMap> model;
    ColorMap original;
    ColorMapSlider * slider;
    QPushButton *removeButton;

    QGroupBox *group;
    ColorButton *buttonStop, *buttonMin, *buttonMax;
    QLineEdit *percentEdit, *valueEdit, *minEdit, *maxEdit;
    QCheckBox * cbAuto;
    double _min, _max;
    bool full;
};

#endif
