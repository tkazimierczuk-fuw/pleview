#include "advancedplot.h"
#include "qwt_scale_widget.h"
#include "qwt_scale_engine.h"

#include <QtWidgets>

class AxisConfigDialog : public QDialog {
    Q_OBJECT
public:
    AxisConfigDialog(AdvancedPlot *plot, int axisid) : QDialog(plot), plot(plot), axisid(axisid) {
        setWindowTitle("Configure axis");
        QwtInterval interval = plot->axisInterval(axisid);

        QFormLayout * layout = new QFormLayout();
        minEdit = new QLineEdit();
        minEdit->setText(QString::number(interval.minValue()));
        minEdit->setValidator(new QDoubleValidator(minEdit));
        layout->addRow("Minimum value", minEdit);

        maxEdit = new QLineEdit();
        maxEdit->setText(QString::number(interval.maxValue()));
        maxEdit->setValidator(new QDoubleValidator(maxEdit));
        layout->addRow("Maximum value", maxEdit);

        autoscaleCheckBox = new QCheckBox();
        connect(autoscaleCheckBox, SIGNAL(toggled(bool)), minEdit, SLOT(setDisabled(bool)));
        connect(autoscaleCheckBox, SIGNAL(toggled(bool)), maxEdit, SLOT(setDisabled(bool)));
        autoscaleCheckBox->setChecked(plot->axisAutoScale(axisid));
        layout->addRow("Autoscale", autoscaleCheckBox);

        buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel, Qt::Horizontal, this);
        buttons->button(QDialogButtonBox::Apply)->setDisabled(true);
        connect(minEdit, SIGNAL(textChanged(QString)), this, SLOT(setApplyEnabled()));
        connect(maxEdit, SIGNAL(textChanged(QString)), this, SLOT(setApplyEnabled()));
        connect(autoscaleCheckBox, SIGNAL(toggled(bool)), this, SLOT(setApplyEnabled()));
        connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
        connect(buttons->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
        connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
        layout->addRow(buttons);

        setLayout(layout);
    }

protected slots:
    void setApplyEnabled() {
        buttons->button(QDialogButtonBox::Apply)->setEnabled(true);
    }


    void apply() {
        if(autoscaleCheckBox->isChecked())
            plot->setAxisAutoScale(axisid);
        else
            plot->setAxisScale(axisid, minEdit->text().toDouble(), maxEdit->text().toDouble());
        plot->replot();
        buttons->button(QDialogButtonBox::Apply)->setEnabled(false);

    }


    void accept() override {
        apply();
        QDialog::accept();
    }


private:
    AdvancedPlot *plot;
    int axisid;
    QDialogButtonBox *buttons;
    QCheckBox *autoscaleCheckBox;
    QLineEdit *maxEdit, *minEdit;
};







AdvancedPlot::AdvancedPlot()
{
    createActions();

    replot_requested = false;
    connect(this, SIGNAL(internalWakeup()), this, SLOT(wakeUp()), Qt::QueuedConnection);

    //zoomer = new QwtPlotZoomer(this);
    //zoomer->setEnabled(false);

    axisWidget(QwtPlot::xBottom)->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(axisWidget(QwtPlot::xBottom), SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(displayHAxisCustomContextMenu(QPoint)));
    axisWidget(QwtPlot::yLeft)->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(axisWidget(QwtPlot::yLeft), SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(displayVAxisCustomContextMenu(QPoint)));
}


void AdvancedPlot::setZoomerActive(bool active) {
    //zoomer->setEnabled(active);
}


void AdvancedPlot::setAxisName(Qt::Orientation orientation, const QString &name) {
    QAction *action = (orientation == Qt::Vertical) ? vAutoscaleAction : hAutoscaleAction;
    action->setText(QString("Autoscale %1 axis").arg(name));

    action = (orientation == Qt::Vertical) ? vFullscaleAction : hFullscaleAction;
    action->setText(QString("Autorange %1 axis now").arg(name));
}





void AdvancedPlot::setVAxisAutoscale(bool on) {
    setAxisAutoScale(QwtPlot::yLeft, on);
    replot();
}


void AdvancedPlot::setHAxisAutoscale(bool on) {
    setAxisAutoScale(QwtPlot::xBottom, on);
    replot();
}





void AdvancedPlot::displayVAxisCustomContextMenu(const QPoint & pos) {
    displayAxisCustomContextMenu(pos, QwtPlot::yLeft);
}


void AdvancedPlot::displayHAxisCustomContextMenu(const QPoint & pos) {
    displayAxisCustomContextMenu(pos, QwtPlot::xBottom);
}


void AdvancedPlot::displayAxisCustomContextMenu(const QPoint & pos, int axisid) {
    QPoint globalPos = this->axisWidget(axisid)->mapToGlobal(pos);

        QMenu contextMenu;
        QAction * setRangeAction = contextMenu.addAction("Set range");
        if(axisid == xBottom) {
            contextMenu.addAction(hFullscaleAction);
            contextMenu.addAction(hAutoscaleAction);
        }
        else if(axisid == yLeft) {
            contextMenu.addAction(vFullscaleAction);
            contextMenu.addAction(vAutoscaleAction);
        }

        QAction* selectedItem = contextMenu.exec(globalPos);
        if (selectedItem == setRangeAction) {
            AxisConfigDialog dialog(this, axisid);
            dialog.exec();
        }
        else
        {
            // nothing was chosen
        }
}



void AdvancedPlot::replot() {

    if(hAutoscaleAction->isChecked() != axisAutoScale(QwtPlot::xBottom)) {
        bool signalsBlocked = hAutoscaleAction->blockSignals(true);
        hAutoscaleAction->setChecked(axisAutoScale(QwtPlot::xBottom));
        hAutoscaleAction->blockSignals(signalsBlocked);
    }

    if(vAutoscaleAction->isChecked() != axisAutoScale(QwtPlot::yLeft)) {
        bool signalsBlocked = vAutoscaleAction->blockSignals(true);
        vAutoscaleAction->setChecked(axisAutoScale(QwtPlot::yLeft));
        vAutoscaleAction->blockSignals(signalsBlocked);
    }

    replot_requested = true;
    emit internalWakeup();
}


void AdvancedPlot::wakeUp() {
    if(replot_requested) {
        replot_requested = false;
        QwtPlot::replot();
    }
}


void AdvancedPlot::setHScale(const QwtScaleDiv &scaleDiv) {
    if(!scaleDiv.isEmpty())
        this->setAxisScaleDiv(QwtPlot::xBottom, scaleDiv);
    else
        this->setAxisAutoScale(QwtPlot::xBottom);
    replot();
}


void AdvancedPlot::setVScale(const QwtScaleDiv &scaleDiv) {
    if(!scaleDiv.isEmpty())
        this->setAxisScaleDiv(QwtPlot::yLeft, scaleDiv);
    else
        this->setAxisAutoScale(QwtPlot::yLeft);
    replot();
}


void doFullScale(QwtPlot * plot, int axisid) {
    if(plot->axisAutoScale(axisid))
        return;
    plot->setAxisAutoScale(axisid, true);
    plot->updateAxes();
    plot->setAxisAutoScale(axisid, false);
}

void AdvancedPlot::doHAxisFullscale() {
    doFullScale(this, QwtPlot::xBottom);
    replot();
}

void AdvancedPlot::doVAxisFullscale() {
    doFullScale(this, QwtPlot::yLeft);
    replot();
}



const QAction * AdvancedPlot::autoscaleAction(Qt::Orientation orientation) const {
    if(orientation == Qt::Vertical)
        return vAutoscaleAction;
    else
        return hAutoscaleAction;
}


const QAction * AdvancedPlot::fullscaleAction(Qt::Orientations orientation) const {
    if(orientation == Qt::Vertical)
        return vFullscaleAction;
    else if(orientation == Qt::Horizontal)
        return hFullscaleAction;
    else
        return hvFullscaleAction;
}


void AdvancedPlot::createActions() {
    // Autoscale actions
    vAutoscaleAction = new QAction(QIcon(":/icons/actions/zoom-fit-height-auto.svg"),"",this);
    vAutoscaleAction->setIconVisibleInMenu(false);
    vAutoscaleAction->setCheckable(true);
    vAutoscaleAction->setChecked(true);
    connect(vAutoscaleAction, SIGNAL(toggled(bool)), this, SLOT(setVAxisAutoscale(bool)));
    hAutoscaleAction = new QAction(QIcon(":/icons/actions/zoom-fit-width-auto.svg"),"",this);
    hAutoscaleAction->setIconVisibleInMenu(false);
    hAutoscaleAction->setCheckable(true);
    hAutoscaleAction->setChecked(true);
    connect(hAutoscaleAction, SIGNAL(toggled(bool)), this, SLOT(setHAxisAutoscale(bool)));

    // Fullscale actions
    vFullscaleAction = new QAction(QIcon(":/icons/actions/zoom-fit-height.svg"),"",this);
    vFullscaleAction->setIconVisibleInMenu(false);
    connect(vFullscaleAction, SIGNAL(triggered()), this, SLOT(doVAxisFullscale()));
    hFullscaleAction = new QAction(QIcon(":/icons/actions/zoom-fit-width.svg"),"",this);
    hFullscaleAction->setIconVisibleInMenu(false);
    connect(hFullscaleAction, SIGNAL(triggered()), this, SLOT(doHAxisFullscale()));
    hvFullscaleAction = new QAction(QIcon(":/icons/actions/zoom-fit-best.svg"),"Autorange plot", this);
    connect(hvFullscaleAction, SIGNAL(triggered()), this, SLOT(doVAxisFullscale()));
    connect(hvFullscaleAction, SIGNAL(triggered()), this, SLOT(doHAxisFullscale()));

    setAxisName(Qt::Vertical, "Y");
    setAxisName(Qt::Horizontal, "X");
}

#include "advancedplot.moc"
