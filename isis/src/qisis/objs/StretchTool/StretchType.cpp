#include "StretchType.h"

#include <iostream>

#include <QTableWidget>
#include <QHeaderView>
#include <QColor>
#include <QString>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QPushButton>

#include "Stretch.h"
#include "Histogram.h"
#include "HistogramWidget.h"

using namespace Isis;

namespace Qisis {
  /**
   * This constructs a stretch type. It provides a main layout,
   * sizing policies, and a few widgets. Children should insert
   * themselves at the main layout grid row 1 column 0.
   *
   * @param hist
   * @param stretch
   * @param name
   * @param color
   */
  StretchType::StretchType(const Isis::Histogram &hist,
                           const Isis::Stretch &stretch,
                           const QString &name, const QColor &color) {
    p_stretch = NULL;
    p_table = NULL;
    p_cubeHist = NULL;
    p_graph = NULL;
    p_mainLayout = NULL;

    p_cubeHist = new Histogram(hist);

    p_stretch = new Stretch();

    p_graph = new HistogramWidget(QString("Visible ") + name + QString(" Hist"),
                                  color.lighter(110), color.darker(110));
    p_graph->setHistogram(*p_cubeHist);
    p_graph->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,
                                       QSizePolicy::Minimum));
    p_graph->setMinimumSize(QSize(250, 200));

    p_mainLayout = new QGridLayout();
    p_mainLayout->addWidget(p_graph, 0, 0);

    p_table = createStretchTable();
    connect(this, SIGNAL(stretchChanged()), this, SLOT(updateTable()));
    p_mainLayout->addWidget(p_table, 2, 0);

    QPushButton *saveAsButton = new QPushButton("Save Stretch Pairs...");
    connect(saveAsButton, SIGNAL(clicked(bool)), this, SLOT(savePairs()));
    p_mainLayout->addWidget(saveAsButton, 3, 0);

    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy.setHeightForWidth(true);
    p_graph->setSizePolicy(sizePolicy);
  }


  /**
   * Destructor
   */
  StretchType::~StretchType() {
    if(p_cubeHist) {
      delete p_cubeHist;
      p_cubeHist = NULL;
    }

    if(p_stretch) {
      delete p_stretch;
      p_stretch = NULL;
    }
  }


  /**
   * This should be called when the visible area changes. It
   * updates the graph and calls setStretch() so that the children
   * have a chance to update their GUI elements too.
   *
   * @param hist
   */
  void StretchType::setHistogram(const Isis::Histogram &hist) {
    p_graph->setHistogram(hist);
    *p_cubeHist = hist;
    setStretch(*p_stretch);
  }


  /**
   * This creates the stretch pairs table.
   *
   * @return QTableWidget*
   */
  QTableWidget *StretchType::createStretchTable() {
    QTableWidget *table = new QTableWidget(0, 2);

    QStringList labels;
    labels << "Input" << "Output";
    table->setHorizontalHeaderLabels(labels);

    table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);

    return table;
  }


  /**
   * This updates the table with the current stretch pairs.
   */
  void StretchType::updateTable() {
    p_graph->setStretch(*p_stretch);

    Stretch stretch = getStretch();
    p_table->setRowCount(stretch.Pairs());

    for(int i = 0; i < stretch.Pairs(); i++) {
      QTableWidgetItem *inputItem = new QTableWidgetItem(QString("%1").arg(
            stretch.Input(i)));
      inputItem->setTextAlignment(Qt::AlignCenter);
      QTableWidgetItem *outputItem = new QTableWidgetItem(QString("%1").arg(
            stretch.Output(i)));
      outputItem->setTextAlignment(Qt::AlignCenter);

      p_table->setItem(i, 0, inputItem);
      p_table->setItem(i, 1, outputItem);
      p_table->setMinimumSize(QSize(200, 100));
    }
  }


  /**
   * This asks the user for a file and saves the current stretch
   * pairs to that file.
   */
  void StretchType::savePairs() {
    QString filename = QFileDialog::getSaveFileName((QWidget *)parent(),
                       "Choose filename to save under", ".", "Text Files (*.txt)");
    if(filename.isEmpty()) return;

    QFile outfile(filename);
    bool success = outfile.open(QIODevice::WriteOnly);

    if(!success) {
      QMessageBox::critical((QWidget *)parent(),
                            "Error", "Cannot open file, please check permissions");
      return;
    }

    QString currentText;
    QTextStream stream(&outfile);

    Isis::Stretch stretch = getStretch();

    //Add the pairs to the file
    stream << stretch.Text().c_str() << endl;

    outfile.close();
  }


  /**
   * Returns the current stretch object
   *
   * @return Isis::Stretch
   */
  Isis::Stretch StretchType::getStretch() {
    return *p_stretch;
  }
}
