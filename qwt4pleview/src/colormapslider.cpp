#include <QWidget>
#include <QMap>
#include <QtGui>
#include <QGradient>
#include <QBrush>
#include <QColor>
#include <QGridLayout>
#include <QFormLayout>
#include <QColorDialog>
#include <QCheckBox>
#include <QToolButton>
#include <QDialog>
#include <QDialogButtonBox>

#include <iostream>
#include <limits>
#include <cmath>
using std::abs;

#include "colormapslider.h"

double ColorMapSlider::mapMouse(const QPoint &pos) const {
    double ret = (double) pos.x() / width();
    if(ret < 0)
        ret = 0;
    else if(ret > 1)
        ret = 1;
    return ret;
}

double ColorMapSlider::mapValue(double value) const {
    return width() * value;
}


/*!
   Mouse press event handler
   \param e Mouse event
*/
void ColorMapSlider::mousePressEvent(QMouseEvent *e) {
    double pos = mapMouse(e->pos());
    selectStop(pos);
    if(selected < 0)
        addStop(pos, Qt::green);
    update();
}

void ColorMapSlider::mouseDoubleClickEvent(QMouseEvent *e) {
    double pos = mapMouse(e->pos());
    selectStop(pos);
    double candidate = selected;
    if(selected < 0)
        candidate = pos;



    if(candidate < 0 || candidate > 1)
        return;
    QColor color = model->colorAtFraction(candidate);

    color = QColorDialog::getColor(color, this);
    if(color.isValid()) {
        auto raw = model->rawMap();
        raw.insert(candidate, color.rgba());
        model->setRawMap(raw);
    }
    update();
}


void ColorMapSlider::checkSelection() {
    if(selected >= 0 && !model->rawMap().contains(selected))
        selected = -1;
}


void ColorMapSlider::mouseMoveEvent(QMouseEvent *e)
{
    if(selected < 0)
        return;
    double pos = mapMouse(e->pos());
    moveStop(pos);
}

void ColorMapSlider::mouseReleaseEvent(QMouseEvent *e)
{
    if(selected < 0)
        return;
    double pos = mapMouse(e->pos());
    moveStop(pos);
}


void ColorMapSlider::keyPressEvent(QKeyEvent * event) {
    if(event->key() == Qt::Key_Delete) {
        removeStop();
        selected = -1;
    }
}


void ColorMapSlider::addStop(double val, const QColor &color) {
    if(val < 0)
        val = 0;
    else if(val > 1)
        val = 1;
    auto map = model->rawMap();
    map.insert(val, color.rgba());
    model->setRawMap(map);

    selected = val;
    emit(selectionChanged());
}


bool ColorMapSlider::selectStop(double val) {
    selected = -1;
    for(auto point : model->rawMap().keys()) {
        if( abs(point-val) < abs(val-selected) )
            selected = point;
    }

    const int selectRange = 20;
    if(abs(mapValue(val) - mapValue(selected)) > selectRange)
        selected = -1;
     return true; // why return a value?
}


void ColorMapSlider::moveStop(double val) {
    if(val <= 0 || val >= 1) {
        removeStop();
        return;
    }

    auto map = model->rawMap();
    QColor color = map.take(selected);
    map.insert(val, color.rgba());
    selected = val;
    model->setRawMap(map);


}


void ColorMapSlider::removeStop() {
    auto map = model->rawMap();
    map.remove(selected);
    selected = -1;
    model->setRawMap(map);
}


void ColorMapSlider::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRect area = contentsRect();
    QLinearGradient gradient(area.bottomLeft(), area.bottomRight());
    gradient.setColorAt(0., Qt::white);
    gradient.setColorAt(1, Qt::black);

    for(auto const & point : model->rawMap().toStdMap())
        gradient.setColorAt(point.first, point.second);

    const int margin = 5;
    QBrush brush(gradient);
    painter.fillRect(area.adjusted(stopWidth /2, margin, -stopWidth/2, height()), brush);


    for(auto const & point : model->rawMap().toStdMap()) {
        gradient.setColorAt(point.first, point.second);
        QRect posRect(mapValue(point.first) - stopWidth /2, margin, stopWidth, height());
        if(point.first == selected)
            posRect.adjust(0, -margin, 0, 0);
        painter.fillRect(posRect, Qt::black);
        const int stripWidth = 1;
        const int border = (stopWidth - stripWidth) / 2;
        posRect.adjust(border, 0, -border, 0);
        painter.fillRect(posRect, point.second);
    }

    painter.end();
}


void ColorMapSlider::changeSelectedColor(const QColor &color) {
    if(selected >= 0 && selected <= 1) {
        auto raw = model->rawMap();
        raw.insert(selected, color.rgba());
        model->setRawMap(raw);
    }
}

QColor ColorMapSlider::selectedColor() {
    return model->rawMap().value(selected, Qt::black);
}

double ColorMapSlider::selectedStop() {
    if(selected >= 0 && selected <= 1)
        return selected;
    else
        return qSNaN();
}


ColorMap ColorMapSlider::currentMap() {
    return *model;
}


void ColorMapConfig::setupUi() {
    QGridLayout * gridLayout = new QGridLayout(this);

    buttonMin = new ColorButton(model->colorAtFraction(0));
    buttonMin->setAutoDefault(false);
    gridLayout->addWidget(buttonMin, 0, 0, 1, 1);

    /* slider is already instantiated */
    slider->setMinimumWidth(100);
    gridLayout->addWidget(slider, 0, 1, 2, 1);

    buttonMax = new ColorButton(model->colorAtFraction(1));
    buttonMax->setAutoDefault(false);
    gridLayout->addWidget(buttonMax, 0, 2, 1, 1);

    minEdit = new QLineEdit(QString::number(_min));
    QDoubleValidator *validator = new QDoubleValidator(-qInf(), qInf(), 10, this);
    minEdit->setValidator(validator);
    gridLayout->addWidget(minEdit, 1, 0, 1, 1);

    maxEdit = new QLineEdit(QString::number(_max));
    minEdit->setValidator(validator);
    gridLayout->addWidget(maxEdit, 1, 2, 1, 1);

    QVBoxLayout * toolButtonLayout = new QVBoxLayout();

    cbAuto = new QCheckBox("Autoscale");
    cbAuto->setChecked(original.autoscaling());
    connect(cbAuto, &QCheckBox::toggled, [=](bool on){ model->setAutoscaling(on);} );

    if(!full) {
        QPushButton * tbDialog = new QPushButton("Advanced");
        connect(tbDialog, SIGNAL(clicked()), this, SLOT(displayDialog()));
        toolButtonLayout->addWidget(tbDialog);
        toolButtonLayout->addWidget(cbAuto);
        gridLayout->addLayout(toolButtonLayout, 0, 3, 2, 1);
    }
    else { // full == true
        gridLayout->addWidget(cbAuto, 2, 0, 1, 3);
        cbAuto->setText("Determine min and max automatically");

        group = new QGroupBox("Color stop");
        gridLayout->addWidget(group, 3, 0, 1, 3);
        QFormLayout *formLayout = new QFormLayout(group);

        buttonStop = new ColorButton();
        buttonStop->setAutoDefault(false);
        formLayout->addRow("Color:", buttonStop);
        valueEdit = new QLineEdit();
        validator = new QDoubleValidator(this);
        valueEdit->setValidator(validator);
        formLayout->addRow("Value:", valueEdit);

        percentEdit = new QLineEdit("0");
        validator = new QDoubleValidator(0., 100., 10, this);
        percentEdit->setValidator(validator);
        formLayout->addRow("Percentage:", percentEdit);

        removeButton = new QPushButton();
        removeButton->setIcon(QIcon(":/icons/actions/editdelete.svg"));
        removeButton->setToolTip("Remove current color stop");

        formLayout->addRow(removeButton);

        connect(removeButton, &QPushButton::clicked, [=](bool b){ slider->removeStop(); });
        connect(buttonStop, SIGNAL(valueChanged(QColor)), slider, SLOT(changeSelectedColor(QColor)));
        connect(valueEdit, &QLineEdit::editingFinished, [=]{ slider->moveStop(model->valueToFraction(valueEdit->text().toDouble())); });
        connect(percentEdit, &QLineEdit::editingFinished, [=]{ slider->moveStop(percentEdit->text().toDouble()/100); });
    }

    this->setLayout(gridLayout);
}


ColorMapConfig::ColorMapConfig(QWidget * parent, std::shared_ptr<ColorMap> colormap, bool full)
        : QWidget(parent)
{
    this->full = full;
    _min = colormap->min(); _max = colormap->max();
    original = *colormap;
    model = colormap;
    slider = new ColorMapSlider(this, colormap);

    setupUi();
    updateDialog();

    connect(buttonMin, &ColorButton::valueChanged, [=](const QColor &c){ model->addColorStop(0, c); } );
    connect(buttonMax, &ColorButton::valueChanged, [=](const QColor &c){ model->addColorStop(1, c); } );
    connect(minEdit, &QLineEdit::editingFinished, [=]{ model->setMin(minEdit->text().toDouble()); } );
    connect(maxEdit, &QLineEdit::editingFinished, [=]{ model->setMax(maxEdit->text().toDouble()); } );
    connect(colormap.get(), SIGNAL(valueChanged()), this, SLOT(updateDialog()));
    if(full)
        connect(slider, SIGNAL(selectionChanged()), this, SLOT(updateDialog()));

}


void ColorMapConfig::cancel() {
    *model = original;
}


void ColorMapConfig::updateDialog() {
    buttonMin->setColor(model->colorAtFraction(0));
    buttonMax->setColor(model->colorAtFraction(1));
    minEdit->setText(QString::number(model->min()));
    maxEdit->setText(QString::number(model->max()));
    cbAuto->setChecked(model->autoscaling());

    bool autoMode = cbAuto->isChecked();
    minEdit->setDisabled(autoMode);
    maxEdit->setDisabled(autoMode);

    if(full) {
        bool isStopSelected = !qIsNaN(slider->selectedStop());
        group->setEnabled(isStopSelected);
        if(isStopSelected) {
            valueEdit->setText(QString::number(model->fractionToValue(slider->selectedStop())));
            percentEdit->setText(QString::number(100*slider->selectedStop()));
            buttonStop->setColor(slider->selectedColor());
        }
    }

}


ColorMap ColorMapConfig::currentColorMap() const {
    ColorMap map = slider->currentMap();
    double vMin = minEdit->text().toDouble();
    double vMax = maxEdit->text().toDouble();
    map.setRange(vMin, vMax);
    map.setAutoscaling(cbAuto->isChecked());
    return map;
}


void ColorMapConfig::displayDialog() {
    QDialog * dialog = new QDialog(this);
    dialog->setWindowTitle("Configure color scale");
    QFormLayout * layout = new QFormLayout();

    ColorMapConfig * fullConfig = new ColorMapConfig(this, model);
    layout->addRow(fullConfig);

    QDialogButtonBox * buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    connect(buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), dialog, SLOT(reject()));
    layout->addRow(buttons);

    dialog->setLayout(layout);

    if(dialog->exec()) {
        //setColorMap(fullConfig->currentColorMap());
    }
    delete dialog;
}
