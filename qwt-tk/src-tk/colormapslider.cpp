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

#include "colormapslider.h"

double ColorMapSlider::mapMouse(const QPoint &pos) {
    double ret = (double) pos.x() / width();
    if(ret < 0)
        ret = 0;
    else if(ret > 1)
        ret = 1;
    return ret;
}

double ColorMapSlider::mapValue(double value) {
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
    QColor color = map.value(candidate, Qt::green);

    color = QColorDialog::getColor(color, this);
    if(color.isValid())
        map.insert(candidate, color);
    update();
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
    map.insert(val, color);
    selected = val;
    emit(mapChanged());
    update();
}

bool ColorMapSlider::selectStop(double val) {
    selected = -1;
    QMap<double, QColor>::const_iterator it;
    it = map.lowerBound(val);
    if(it != map.constEnd())
        selected = it.key();
    it = map.upperBound(val);
    if(it != map.constBegin() && abs(it.key() - val)< abs(val - selected))
        selected = it.key();
    const int selectRange = 20;
    if(abs(mapValue(val) - mapValue(selected)) > selectRange)
        selected = -1;
    emit(mapChanged());
    update();
    return true; // why return a value?
}

void ColorMapSlider::moveStop(double val) {
    if(val <= 0 || val >= 1) {
        removeStop();
        return;
    }

    QColor color = map.take(selected);
    map.insert(val, color);
    selected = val;
    emit(mapChanged());
    update();
}

void ColorMapSlider::removeStop() {
    map.remove(selected);
    selected = -1;
    emit(mapChanged());
    update();
}


void ColorMapSlider::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRect area = contentsRect();
    QLinearGradient gradient(area.bottomLeft(), area.bottomRight());
    gradient.setColorAt(0., Qt::white);
    gradient.setColorAt(1, Qt::black);

    QMap<double, QColor>::const_iterator it = map.constBegin();
    while (it != map.constEnd()) {
        gradient.setColorAt(it.key(), it.value());
        it++;
    }

    const int margin = 5;
    QBrush brush(gradient);
    painter.fillRect(area.adjusted(stopWidth /2, margin, -stopWidth/2, height()), brush);

    it = map.constBegin();
    while (it != map.constEnd()) {
        QRect posRect(mapValue(it.key()) - stopWidth /2, margin, stopWidth, height());
        if(it.key() == selected)
            posRect.adjust(0, -margin, 0, 0);
        painter.fillRect(posRect, Qt::black);
        const int stripWidth = 1;
        const int border = (stopWidth - stripWidth) / 2;
        posRect.adjust(border, 0, -border, 0);
        painter.fillRect(posRect, it.value());
        it++;
    }

    painter.end();
}


void ColorMapSlider::changeSelectedColor(const QColor &color) {
    if(selected >= 0 && selected <= 1)
        map.insert(selected, color);
    emit(mapChanged());
    update();
}

QColor ColorMapSlider::selectedColor() {
    return map.value(selected, Qt::black);
}

double ColorMapSlider::selectedStop() {
    if(selected >= 0 && selected <= 1)
        return selected;
    else
        return qSNaN();
}


ColorMap ColorMapSlider::currentMap() {
    QColor color0 = (map.find(0.) != map.end()) ? map[0.] : Qt::black;
    QColor color1 = (map.find(1.) != map.end()) ? map[1.] : Qt::white;
    ColorMap map2(0, 1, color0, color1);

    QMap<double, QColor>::iterator it = map.begin();
    while (it != map.end()) {
      map2.addColorStop(it.key(), it.value());
      it++;
    }
    return map2;
}


void ColorMapSlider::setColorMap(const ColorMap &_map) {
    map.clear();
    QMap<double, QRgb> tmpMap = _map.rawMap();
    QMap<double, QRgb>::const_iterator it = tmpMap.begin();
    while(it != tmpMap.end()) {
        map.insert(it.key(), it.value());
        it++;
    }

    stopWidth = 5;
    selected = -1;
    mapChanged();
}


void ColorMapConfig::setupUi() {
    QGridLayout * gridLayout = new QGridLayout(this);

    buttonMin = new ColorButton();
    buttonMin->setAutoDefault(false);
    gridLayout->addWidget(buttonMin, 0, 0, 1, 1);

    /* slider is already instantiated */
    slider->setMinimumWidth(100);
    gridLayout->addWidget(slider, 0, 1, 2, 1);

    buttonMax = new ColorButton();
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
    cbAuto->setChecked(oryginal.autoscaling());
    connect(cbAuto, SIGNAL(toggled(bool)), this, SLOT(limitEdited()));

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
        formLayout->addRow("&Color:", buttonStop);
        valueEdit = new QLineEdit();
        validator = new QDoubleValidator(_min, _max, 10, this);
        valueEdit->setValidator(validator);
        formLayout->addRow("&Value:", valueEdit);

        percentEdit = new QLineEdit("0");
        validator = new QDoubleValidator(0., 100., 10, this);
        percentEdit->setValidator(validator);
        formLayout->addRow("Percentage:", percentEdit);

        removeButton = new QPushButton();
        removeButton->setIcon(QIcon(":/icons/actions/editdelete.svg"));
        removeButton->setToolTip("Remove current color stop");

        formLayout->addRow(removeButton);

        connect(removeButton, SIGNAL(clicked()), this, SLOT(stopRemoved()));
        connect(buttonStop, SIGNAL(valueChanged(QColor)), slider, SLOT(changeSelectedColor(QColor)));
        connect(percentEdit, SIGNAL(editingFinished()), this, SLOT(percentEdited()));
        connect(valueEdit, SIGNAL(editingFinished()), this, SLOT(valueEdited()));
    }

    this->setLayout(gridLayout);
}

void ColorMapConfig::mapChanged() {
    ColorMap map = currentColorMap();
    buttonMin->setColor(map.color(map.min()));
    buttonMax->setColor(map.color(map.max()));

    if(full) {
        if(!qIsNaN(slider->selectedStop())) {
            group->setEnabled(true);
            buttonStop->setColor(slider->selectedColor());
            double percent = 100*slider->selectedStop();
            percentEdit->setText(QString("%1").arg(percent));
            valueEdit->setText(QString("%1").arg(percentToValue(percent)));
        } else {
            group->setEnabled(false);
        }
    }
    emit colorMapChanged(map);
}


void ColorMapConfig::setMinColor(const QColor &color) {
    slider->addStop(0, color);
}


void ColorMapConfig::setMaxColor(const QColor &color) {
    slider->addStop(1, color);
}


void ColorMapConfig::percentEdited() {
    double pos = 0.01 * percentEdit->text().toDouble();
    slider->moveStop(pos);
}


void ColorMapConfig::valueEdited() {
    double pos = valueEdit->text().toDouble();
    pos = (pos - _min) / (_max - _min);
    slider->moveStop(pos);
}


void ColorMapConfig::stopRemoved() {
    slider->removeStop();
}


void ColorMapConfig::limitEdited() {
    bool autoMode = cbAuto->isChecked();
    minEdit->setDisabled(autoMode);
    maxEdit->setDisabled(autoMode);

    ColorMap map = slider->currentMap();
    if(autoMode)
        map.setAutoscaling(true);
    else {
        _min = minEdit->text().toDouble();
        _max = maxEdit->text().toDouble();
        if(full && group->isEnabled())
            valueEdit->setText(QString::number(percentToValue(percentEdit->text().toDouble())));
        map.setRange(_min, _max);
    }
    emit colorMapChanged(map);
}




ColorMapConfig::ColorMapConfig(QWidget * parent, const ColorMap &initial, bool full)
        : QWidget(parent)
{
    this->full = full;
    _min = initial.min(); _max = initial.max();
    oryginal = initial;
    slider = new ColorMapSlider(this, initial);

    setupUi();

    connect(slider, SIGNAL(mapChanged()), this, SLOT(mapChanged()));
    connect(buttonMin, SIGNAL(valueChanged(QColor)), this, SLOT(setMinColor(QColor)));
    connect(buttonMax, SIGNAL(valueChanged(QColor)), this, SLOT(setMaxColor(QColor)));
    connect(minEdit, SIGNAL(editingFinished()), this, SLOT(limitEdited()));
    connect(maxEdit, SIGNAL(editingFinished()), this, SLOT(limitEdited()));
    this->mapChanged();

    limitEdited(); // can set min and max control disabled if autoscaling is checked
}

void ColorMapConfig::cancel() {
    emit colorMapChanged(oryginal);
}

void ColorMapConfig::setColorMap(const ColorMap &map) {
    bool tmp = slider->blockSignals(true);
    slider->setColorMap(map);
    slider->blockSignals(tmp);
    tmp = minEdit->blockSignals(true);
    minEdit->setText(QString::number(map.min()));
    minEdit->blockSignals(tmp);
    tmp = maxEdit->blockSignals(true);
    maxEdit->setText(QString::number(map.max()));
    maxEdit->blockSignals(tmp);
    tmp = cbAuto->blockSignals(true);
    cbAuto->setChecked(map.autoscaling());
    cbAuto->blockSignals(tmp);
    mapChanged();
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

    ColorMapConfig * fullConfig = new ColorMapConfig(this, currentColorMap());
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
