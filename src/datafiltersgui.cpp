#include "datafiltersgui.h"

#include "axistransformation.h"

WidgetView::WidgetView(QItemSelectionModel * selectionModel, QWidget * parent)
    : QScrollArea(parent) {
    connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(currentChanged(QModelIndex)));
}

void WidgetView::currentChanged(QModelIndex current) {
    if(!current.isValid()) {
        takeWidget(); // TODO: what about ownership?
        return;
    }
    QWidget * widget = qobject_cast<QWidget*>(current.data(Qt::UserRole).value<QWidget*>());
    takeWidget(); // we don't want to delete the old widget
    this->setWidget(widget);
}



DataFiltersGui::DataFiltersGui(DataFilterManager *manager, QWidget *parent)
    : QDialog(parent), _manager(manager)
{

    this->setWindowTitle("PLEview - transform data");
    this->setWindowIcon(QIcon(":/icons/actions/filter.png"));

    QItemSelectionModel * selectionModel = new QItemSelectionModel(manager);

    QGridLayout * layout = new QGridLayout();
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 2);
    listWidget = new QListView();
    listWidget->setModel(manager);
    listWidget->setSelectionModel(selectionModel);
    layout->addWidget(listWidget, 1, 0, 1, 1);

    QHBoxLayout * buttonLayout = new QHBoxLayout();
    QPushButton * newButton = new QPushButton("Add transform");
    connect(newButton, SIGNAL(clicked()), this, SLOT(createFilter()));
    buttonLayout->addWidget(newButton);
    layout->addLayout(buttonLayout, 0, 0, 1, 1);

    WidgetView * scroll = new WidgetView(selectionModel);
    layout->addWidget(scroll, 0, 1, 3, 1);

    QToolBar * toolbar = new QToolBar();
    for(int type = 0 ; type < 3 ; type++) { // iteration over ModelChangeAction::Type
        ModelChangeAction * act = new ModelChangeAction( (ModelChangeAction::Type) type, selectionModel, this);
        connect(act, SIGNAL(swapItems(int,int)), manager, SLOT(swapItems(int,int)));
        toolbar->addAction(act);
    }
    QAction * actionImport = new QAction(QIcon(":/icons/actions/document-open.svg"), "Import transform", this);
    connect(actionImport, SIGNAL(triggered()), this, SLOT(requestImport()));
    toolbar->addAction(actionImport);
    layout->addWidget(toolbar, 2, 0, 1, 1);

    QDialogButtonBox * buttonBox = new QDialogButtonBox();
    layout->addWidget(buttonBox, 3, 0, 1, 2);
    connect(buttonBox->addButton(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(close()));
    connect(buttonBox->addButton(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
    setLayout(layout);
}


void DataFiltersGui::createFilter() {
    QMap<QString, DataFilterFactory *> dict;
    foreach(DataFilterFactory * factory, DataFilterManager::availableFactories())
        dict.insert(factory->name(), factory);
    bool ok;
    QString chosen = QInputDialog::getItem(this, "Add transform",
                          "Select transform operation to add", dict.keys(), 0, false, &ok);
    if(!ok)
        return;
    DataFilterFactory * factory = dict.value(chosen, 0);
    if(factory)
        _manager->add(factory->instantiate());
}


void DataFiltersGui::apply() {
    _manager->notifyChange();
}


DataFiltersGui::ModelChangeAction
        ::ModelChangeAction(Type type, QItemSelectionModel *selection,
                            QObject * parent) : QAction(parent), _type(type) {
    switch(type) {
    case Up:
        setIcon(QIcon(":/icons/actions/up.svg"));
        setText("Move filter up");
        break;
    case Down:
        setIcon(QIcon(":/icons/actions/down.svg"));
        setText("Move filter down");
        break;
    case Delete:
        setIcon(QIcon(":/icons/actions/editdelete.svg"));
        setText("Delete filter");
    }

    connect(selection, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(currentChanged(QModelIndex)));
    connect(this, SIGNAL(setCurrent(QModelIndex,QItemSelectionModel::SelectionFlags)), selection, SLOT(setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)));
    connect(this, SIGNAL(triggered()), this, SLOT(sendSignal()));
    this->currentChanged(selection->currentIndex());
}


void DataFiltersGui::ModelChangeAction::currentChanged(QModelIndex current) {
    if(!current.isValid()) {
        setEnabled(false);
        return;
    }
    bool ok = true;
    row = current.row();

    switch(_type) {
    case Up:
        ok = current.sibling(current.row()-1, current.column()).isValid();
        break;
    case Down:
        ok = current.sibling(current.row()+1, current.column()).isValid();
        break;
    default:
        break;
    }

    this->setEnabled(ok);
}


void DataFiltersGui::ModelChangeAction::sendSignal() {
    emit setCurrent(QModelIndex(), 0);
    switch(_type) {
    case Up:
        emit swapItems(row, row-1);
        break;
    case Down:
        emit swapItems(row, row+1);
        break;
    case Delete:
        emit swapItems(row, -1);
        break;
    }
}


void DataFiltersGui::requestImport() {
    QString filters = "Pleview XML compressed file (*.plvz)";
    QString fname = QFileDialog::getOpenFileName(this, "Open file", "", filters);

    if(fname.isEmpty())
        return;

    QFile file(fname);

    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "IO error", "Unable to read file:" + fname);
            return; /* error opening the file */
    }
    emit(importTransform(&file, Engine::Transformations));
    file.close();
}


void DataFiltersGui::displayTrailingAxisTransform() {
    if(_manager->filters().isEmpty() || _manager->filters().last()->tagname() != AxisTransformation().tagname())
        _manager->add(new AxisTransformation());

    listWidget->selectionModel()->select(_manager->index(_manager->filters().size()-1),QItemSelectionModel::ClearAndSelect);
    if(!isVisible())
        emit finished(0); // will toggle the action which in turn will change the dialog visibility
}

