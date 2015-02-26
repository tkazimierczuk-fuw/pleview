#include "comment.h"

Comment::Comment() : PlotAddon(new CommentFactory())
{
    _frame = new QFrame();
    QVBoxLayout * layout = new QVBoxLayout();
    _textWidget = new QTextEdit();
    layout->addWidget(_textWidget);
    _frame->setLayout(layout);
}


Comment::~Comment() {
}


void Comment::serializeToXml(QXmlStreamWriter *writer, const QString &tagName) const{
    writer->writeStartElement(tagName);
    writeXmlAttribute(writer, "text", _textWidget->toPlainText());
    writer->writeEndElement();
}


void Comment::unserializeFromXml(QXmlStreamReader *reader) {
    QString text;
    readXmlAttribute(reader, "text", &text);
    if(!text.isEmpty())
        _textWidget->setText(text);
    seekEndElement(reader);
}


QString CommentFactory::description() {
    return "Attach a plain text comment to the file";
}
