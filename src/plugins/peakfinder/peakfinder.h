#ifndef PeakFinder_H
#define PeakFinder_H

#include <QtGui>

#include "data2d.h"
#include "qwt_plot_curve.h"
#include "plotaddons.h"



/** @class PeakFinder
  *
  */
class PeakFinder : public PlotAddon {
    Q_OBJECT
public:
    //! Constructor
    PeakFinder();

    //! Destructor
    ~PeakFinder();

    //! Initialize a plugin
    void init(Engine *engine);

    //! Attach any plot items supplied by plugin to plot widget
    void attach(QwtPlot *plot, PlotType type);

    //! Show a dialog
    QWidget * controlWidget() {
        return _frame;
    }

    //! Some plot elements are visible only if the plugin is active
    void focusEvent(bool hasFocus);

    //! @see Xml::serializeToXml()
    void serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const;

    //! @see Xml::unserializeAttribute(QXmlStreamAttribute
    void unserializeAttribute(const QXmlStreamAttribute &attribute);

private slots:
    void markCurrent(); // test run (current spectrum only)
    void scanAll(); // copy to clipboard for all spectra

    void setDirection(Pleview::Direction direction);

    void setWindowWidth(int);
    void setMinArea(double);
    void setMinArea();
    void clearTestInfo();

private:
    QPolygonF findPeaks(const QPolygonF &curve) const;

    Engine * _engine;
    int _windowWidth; // number of pixels being analyzed simultanously
    double _minArea; // threashold for peak recognition

    QFrame *_frame;
    QPushButton *_markButton, *_scanAllButton;
    QSpinBox * _widthSpinBox;
    QLineEdit * _minAreaEdit;
    QComboBox * _typeCombo;
    QLabel * _label;

    Pleview::Direction _direction;
    QwtPlotCurve *_peaks[2];
};



//! Factory for the XSectionFitter plugin
class PeakFinderFactory : public QObject, public PlotAddonFactory {
     Q_OBJECT
     Q_INTERFACES(PlotAddonFactory)
public:

    //! Name of the plugin
    QString name() { return "Find peaks"; }

    //! Tagname used in saved files
    QString tagname() { return "peakfinder"; }

    //! Plugin description
    QString description();

    PlotAddon * instantiate() {
        return new PeakFinder();
    }
};


#endif // PeakFinder_H
