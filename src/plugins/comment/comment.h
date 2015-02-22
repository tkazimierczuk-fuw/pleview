#ifndef COMMENT_H
#define COMMENT_H

#include <QtGui>

#include "plotaddons.h"


class Comment : public PlotAddon {
    Q_OBJECT
public:

    //! Constructor
    Comment();

    //! Destructor
    ~Comment();

    void init(Engine *engine) {}
    void attach(QwtPlot *plot, PlotType type) {}

    //! Show a dialog
    QWidget * controlWidget() {
        return _frame;
    }

    //! @see Model::serializeToXml()
    void serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const;

    //! @see Model::unserializeFromXml()
    void unserializeFromXml(QXmlStreamReader *reader);

private:
    QFrame *_frame;
    QTextEdit * _textWidget;
};



/*!
  \brief CommentFactory is a class to ... TODO: Finish the comment
*/

class CommentFactory : public QObject, public PlotAddonFactory {
     Q_OBJECT
     Q_INTERFACES(PlotAddonFactory)
public:

    //! Name of the plugin
    QString name() { return "Text comment"; }

    //! Tagname used in saved files
    QString tagname() { return "comment"; }

    //! Plugin description
    QString description();

    PlotAddon * instantiate() {
        return new Comment();
    }
};


#endif // COMMENT_H
