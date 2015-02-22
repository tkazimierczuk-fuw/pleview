#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QtGui>
#include "model.h"
#include "ColorMapSlider.h"
#include "parserwidget.h"
#include "plotrangesmanager.h"


/**
 * Class representing a single page in configuration dialog.
 */
class Page : public QWidget {
public:
    /**
     * Constructor
     */
    Page(QWidget *parent) : QWidget(parent) { }

    /**
     * Commit all pending changes (e.g. on OK or Apply)
     */
    virtual void commit(Engine * engine) { }

    /**
     * Revert to clean state (i.e. initial or explicitely commited)
     */
    virtual void cancel(Engine * engine) { }

    /**
     * Display help about offered settings.
     */
    void displayHelp() { QMessageBox::information(this, "Help", helpText); }


protected:
    QString helpText;
};



/**
 * @brief A class representing main configuration dialog.
 */
class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * @param engine
     */
    ConfigDialog(Engine * engine);

    /**
     * Select current page
     * @param index an index of desired page (0 = cross-section, 1 = xy values,
     *        2 = map colors)
     */
    void setPage(int index);

    void accept();
    void reject();


private:
    /**
     * Helper function to populate contentsWidget
     */
    void createIcons();


private slots:
    /**
     * Display help for current page (internal use only).
     */
    void displayHelp();

    /**
     * Change current page (internal use only).
     */
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Engine * _engine;
    QListWidget *contentsWidget;
    QStackedWidget *pagesWidget;
    QVector<Page*> pages;

};



class CrossSectionPage : public Page
 {
    Q_OBJECT

public:
    CrossSectionPage(Engine * engine, QWidget *parent = 0);

    /**
     * @see Page::commit(Engine*)
     */
    virtual void commit(Engine * engine);

private:
    QLineEdit *widthEdit[2];

 };




class ColorPage : public Page
{
public:
    ColorPage(Engine * engine, QWidget *parent = 0);


    /**
     * @see Page::commit(Engine*)
     */
    virtual void commit(Engine * engine);

    /**
     * @see Page::cancel(Engine*)
     */
    virtual void cancel(Engine * engine);

private:
    ColorMapConfig * configWidget;
};




class DataPage : public Page
{
    Q_OBJECT

public:
    DataPage(Engine * engine, QWidget *parent = 0);

    /**
     * @see Page::commit(Engine*)
     */
    virtual void commit(Engine * engine);

    /**
     * @see Page::cancel(Engine*)
     */
    virtual void cancel(Engine * engine);

private:
    void init(Engine *engine);

private slots:
    void emitSignal(int axis);

signals:
    void axisConfigurationChanged(int axis, const AxisConfiguration &config);

private:
    AxisConfiguration axisConfiguration[2], currentConfiguration[2];
    QComboBox *typeCombo[2];
    QCheckBox *enableTransformBox[2];
    ParserComboWidget *transformEdit[2];
    double dummy; // dummy variable for transformation evaluation
};

/**
  * @class AxisScalePage
  *
  * This class provides the control over scale for X and Y axes.
  * For each of the axes you can configure minimum and maximum value
  * or turn on the autoscaling.
  */
class AxisScalePage : public Page
 {
    Q_OBJECT

public:
    AxisScalePage(Engine * engine, QWidget *parent = 0);

    /**
     * @see Page::commit(Engine*)
     */
    virtual void commit(Engine * engine);

    /**
     * @see Page::cancel(Engine*)
     */
    virtual void cancel(Engine * engine);

 signals:
    void autoscaleRequested(PlotRangesManager::PlotAxis);
    void rangeRequested(PlotRangesManager::PlotAxis, double min, double max);


private:
    QLineEdit *minValueEdit[2], *maxValueEdit[2];
    QCheckBox *autoOption[2];
    double prevMinValue[2], prevMaxValue[2];
    bool prevAutoscale[2];
};


#endif // CONFIGDIALOG_H
