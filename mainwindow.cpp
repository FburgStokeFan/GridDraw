#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColorDialog>
#include <QPalette>
#include "grid.h"
#include <QFileDialog>
#include <QString>
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>
#include <QFontDialog>

// Constructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    m_splash->close();
    delete ui;
}

bool MainWindow::antiAliasing() const
{
    return m_antiAliasing;
}

bool MainWindow::autoUpdate() const
{
    return m_autoUpdate;
}

QColor MainWindow::backgroundColor() const
{
    return m_backgroundColor;
}

QColor MainWindow::centerPointColor() const
{
    return m_centerPointColor;
}

QFont MainWindow::font() const
{
    return m_font;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event);
    QSettings settings("hexgrid.ini",QSettings::IniFormat);
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
}

QColor MainWindow::fontColor() const
{
    return m_fontColor;
}

// Initialize mainwindow
void MainWindow::init()
{
    QSettings settings("hexgrid.ini",QSettings::IniFormat);
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    restoreState(settings.value("mainWindowState").toByteArray());

    setAutoUpdate(false);

    QPixmap ss(":/About.png");
    m_splash = new QSplashScreen(ss);
    m_splash->hide();
    m_groupCells = new QGraphicsItemGroup();

    // Set up initial scene (we'll resize later as we draw hexes)
    m_scene = new QGraphicsScene(0,0,1,1);
    ui->graphicsView->setScene(m_scene);

    ui->comboBox_style->setCurrentIndex(0);
    setRadius(ui->spinBox_radius->value());
    setNumCols(ui->spinBox_numColumns->value());
    setNumRows(ui->spinBox_numRows->value());
    setLineColor(Qt::black);
    setCenterPointColor(Qt::black);
    setBackgroundColor(Qt::white);
    setFontColor(Qt::black);
    setFont(QFont("Arial",8,QFont::Normal));
    setRelativeScale(ui->sb_relativeScale->value());
    ui->checkBox_antiAliasing->setChecked(true);
    setCanvasColor(Qt::white);
    ui->sb_labelRotation->setMaximum(330);
    ui->sb_labelRotation->setSingleStep(30);
    ui->sb_labelRotation->setValue(0);

    setAutoUpdate(true);
    update_grid();    
}

QColor MainWindow::lineColor() const
{
    return m_lineColor;
}

int MainWindow::numCols() const
{
    return m_numCols;
}

int MainWindow::numRows() const
{
    return m_numRows;
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionLoad_Template_triggered()
{
    QSettings settings("grid.ini",QSettings::IniFormat);
    QString templatePath = settings.value("savePath",QDir::currentPath()).toString();

    QString fileName = QFileDialog::getOpenFileName(this,"Open Grid Template",templatePath.append("/untitled.grd"),
                                                    "Templates (*.grd);;All Files (*.*)");
    if(fileName.isNull()) {
        return;
    }

    settings.setValue("savePath",QFileInfo(fileName).path());

    setAutoUpdate(false);

    QSettings gridFile(fileName,QSettings::IniFormat);
    ui->cmb_gridStyle->setCurrentIndex(gridFile.value("gridStyle",0).toInt());
    ui->spinBox_radius->setValue(gridFile.value("radius",1).toInt());
    ui->spinBox_numRows->setValue(gridFile.value("numRows",5).toInt());
    ui->spinBox_numColumns->setValue(gridFile.value("numColumns",5).toInt());
    ui->comboBox_orientation->setCurrentIndex(gridFile.value("orientation",0).toInt());
    ui->sb_relativeScale->setValue(gridFile.value("relativeScale",1.00).toDouble());
    ui->cb_alternateRowColumn->setChecked(gridFile.value("alternateRowColumn",false).toBool());
    ui->doubleSpinBox_lineWidth->setValue(gridFile.value("lineWidth",1.0).toDouble());

    QString str = gridFile.value("lineColor").toString();
    QStringList rgba = str.split(",",QString::KeepEmptyParts);
    setLineColor(QColor(rgba.at(0).toInt(),rgba.at(1).toInt(),rgba.at(2).toInt(),rgba.at(3).toInt()));

    str = gridFile.value("backgroundColor").toString();
    rgba = str.split(",",QString::KeepEmptyParts);
    setBackgroundColor(QColor(rgba.at(0).toInt(),rgba.at(1).toInt(),rgba.at(2).toInt(),rgba.at(3).toInt()));

    str = gridFile.value("centerPointColor").toString();
    rgba = str.split(",",QString::KeepEmptyParts);
    setCenterPointColor(QColor(rgba.at(0).toInt(),rgba.at(1).toInt(),rgba.at(2).toInt(),rgba.at(3).toInt()));

    ui->cmb_centerPointSymbol->setCurrentIndex(gridFile.value("centerPointSymbol",0).toInt());
    ui->comboBox_position->setCurrentIndex(gridFile.value("labelPosition",0).toInt());
    ui->sb_labelRotation->setValue(gridFile.value("labelRotation",0).toInt());
    ui->comboBox_style->setCurrentIndex(gridFile.value("labelStyle",0).toInt());
    ui->le_prefix->setText(gridFile.value("labelPrefix","").toString());
    ui->le_suffix->setText(gridFile.value("labelSuffix","").toString());

    QString f = gridFile.value("labelFont",QFont("Arial",8)).toString();
    QFont fnt;
    fnt.fromString(f);
    ui->lbl_abc123->setFont(fnt);
    str = gridFile.value("fontColor").toString();
    rgba = str.split(",",QString::KeepEmptyParts);
    setFontColor(QColor(rgba.at(0).toInt(),rgba.at(1).toInt(),rgba.at(2).toInt(),rgba.at(3).toInt()));

    ui->checkBox_antiAliasing->setChecked((gridFile.value("antialiasing",true).toBool()));

    str = gridFile.value("canvasColor").toString();
    rgba = str.split(",",QString::KeepEmptyParts);
    setCanvasColor(QColor(rgba.at(0).toInt(),rgba.at(1).toInt(),rgba.at(2).toInt(),rgba.at(3).toInt()));

    setAutoUpdate(true);
    update_grid();
}

void MainWindow::on_actionSave_Template_triggered()
{
    QSettings settings("grid.ini",QSettings::IniFormat);
    QString templatePath = settings.value("savePath",QDir::currentPath()).toString();

    QString fileName = QFileDialog::getSaveFileName(this,"Save Grid Template",
                                                    templatePath.append("/untitled.grd"),
                                                    "Templates (*.grd);;All Files (*.*)");
    if(fileName.isNull()) {
        return;
    }

    settings.setValue("savePath",QFileInfo(fileName).path());

    QSettings gridFile(fileName,QSettings::IniFormat);
    gridFile.setValue("gridStyle",ui->cmb_gridStyle->currentIndex());
    gridFile.setValue("radius",radius());
    gridFile.setValue("numRows",numRows());
    gridFile.setValue("numColumns",numCols());
    gridFile.setValue("orientation",ui->comboBox_orientation->currentIndex());
    gridFile.setValue("relativeScale",ui->sb_relativeScale->text());
    gridFile.setValue("alternateRowColumn",ui->cb_alternateRowColumn->isChecked());
    gridFile.setValue("lineWidth",ui->doubleSpinBox_lineWidth->text());
    gridFile.setValue("lineColor",QString("%1,%2,%3,%4")
                     .arg(lineColor().red())
                     .arg(lineColor().green())
                     .arg(lineColor().blue())
                     .arg(lineColor().alpha()));
    gridFile.setValue("backgroundColor",QString("%1,%2,%3,%4")
                     .arg(backgroundColor().red())
                     .arg(backgroundColor().green())
                     .arg(backgroundColor().blue())
                     .arg(backgroundColor().alpha()));
    gridFile.setValue("centerPointColor",QString("%1,%2,%3,%4")
                     .arg(centerPointColor().red())
                     .arg(centerPointColor().green())
                     .arg(centerPointColor().blue())
                     .arg(centerPointColor().alpha()));
    gridFile.setValue("centerPointSymbol",ui->cmb_centerPointSymbol->currentIndex());
    gridFile.setValue("labelPosition",ui->comboBox_position->currentIndex());
    gridFile.setValue("labelRotation",ui->sb_labelRotation->value());
    gridFile.setValue("labelStyle",ui->comboBox_style->currentIndex());
    gridFile.setValue("labelPrefix",ui->le_prefix->text());
    gridFile.setValue("labelSuffix",ui->le_suffix->text());
    gridFile.setValue("labelFont",ui->lbl_abc123->font().toString());
    gridFile.setValue("fontColor",QString("%1,%2,%3,%4")
                     .arg(fontColor().red())
                     .arg(fontColor().green())
                     .arg(fontColor().blue())
                     .arg(fontColor().alpha()));
    gridFile.setValue("antialiasing",ui->checkBox_antiAliasing->isChecked());
    gridFile.setValue("canvasColor",QString("%1,%2,%3,%4")
                     .arg(canvasColor().red())
                     .arg(canvasColor().green())
                     .arg(canvasColor().blue())
                     .arg(canvasColor().alpha()));
}

// Display color dialog for grid background color selection
void MainWindow::on_btn_backgroundColor_clicked()
{
    QColorDialog diag(this);
    QColor clr = diag.getColor(backgroundColor(),this,"Background Color",
                               QColorDialog::ShowAlphaChannel);
    if(clr != QColor::Invalid) {
        setBackgroundColor(clr);
        update_grid();
    }
}

void MainWindow::on_btn_centerPointColor_clicked()
{
    QColorDialog diag(this);
    QColor clr = diag.getColor(centerPointColor(),this,"Center Point Color",
                               QColorDialog::ShowAlphaChannel);
    if(clr != QColor::Invalid) {
        setCenterPointColor(clr);
        update_grid();
    }
}

void MainWindow::on_btn_fontColor_clicked()
{
    QColorDialog diag(this);
    QColor clr = diag.getColor(fontColor(),this,"Font Color",
                               QColorDialog::ShowAlphaChannel);
    if(clr != QColor::Invalid) {
        setFontColor(clr);
        update_grid();
    }
}

// Display color dialog for grid line color selection
void MainWindow::on_btn_lineColor_clicked()
{
    QColorDialog diag(this);
    QColor clr = diag.getColor(lineColor(),this,"Line Color",
                               QColorDialog::ShowAlphaChannel);
    if(clr != QColor::Invalid) {
        setLineColor(clr);
        update_grid();
    }
}

void MainWindow::on_checkBox_antiAliasing_toggled(bool checked)
{
    setAntiAliasing(checked);
    update_grid();
}

void MainWindow::on_cb_alternateRowColumn_toggled(bool checked)
{
    Q_UNUSED(checked);
    update_grid();
}

void MainWindow::on_comboBox_orientation_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    update_grid();
}

void MainWindow::on_comboBox_position_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    update_grid();
}

void MainWindow::on_comboBox_style_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    update_grid();
}

void MainWindow::on_doubleSpinBox_lineWidth_valueChanged(double arg1)
{
    Q_UNUSED(arg1);
    update_grid();
}

void MainWindow::on_spinBox_numColumns_valueChanged(int arg1)
{
    setNumCols(arg1);
    update_grid();
}

void MainWindow::on_spinBox_numRows_valueChanged(int arg1)
{
    setNumRows(arg1);
    update_grid();
}

void MainWindow::on_spinBox_radius_valueChanged(int arg1)
{
    setRadius(arg1);
    update_grid();
}

int MainWindow::radius() const
{
    return m_radius;
}

void MainWindow::setAntiAliasing(bool antiAliasing)
{
    m_antiAliasing = antiAliasing;
}

void MainWindow::setAutoUpdate(bool autoUpdate)
{
    m_autoUpdate = autoUpdate;
}

void MainWindow::setBackgroundColor(const QColor &backgroundColor)
{
    m_backgroundColor = backgroundColor;
    QPalette pal = ui->lbl_backgroundColor->palette();
    ui->lbl_backgroundColor->setAutoFillBackground(true);
    pal.setColor(ui->lbl_backgroundColor->backgroundRole(),m_backgroundColor);
    ui->lbl_backgroundColor->setPalette(pal);
    if(backgroundColor.alpha() == 0) {
        ui->lbl_backgroundColor->setText("Transparent");
    } else {
        ui->lbl_backgroundColor->setText("");
    }
    update_grid();
}

void MainWindow::setCenterPointColor(const QColor &centerPointColor)
{
    m_centerPointColor = centerPointColor;
    QPalette pal = ui->lbl_centerPoint->palette();
    ui->lbl_centerPoint->setAutoFillBackground(true);
    pal.setColor(ui->lbl_centerPoint->backgroundRole(),m_centerPointColor);
    ui->lbl_centerPoint->setPalette(pal);
    if(centerPointColor.alpha() == 0) {
        ui->lbl_centerPoint->setText("Transparent");
    } else {
        ui->lbl_centerPoint->setText("");
    }
    update_grid();
}

void MainWindow::setFont(const QFont &font)
{
    m_font = font;
    ui->lbl_abc123->setFont(m_font);
    ui->lbl_abc123->setToolTip(QString(m_font.family()));
    if(ui->comboBox_style->currentText() != "None")
        update_grid();
}

void MainWindow::setFontColor(const QColor &fontColor)
{
    m_fontColor = fontColor;
    QPalette pal = ui->lbl_fontColor->palette();
    ui->lbl_fontColor->setAutoFillBackground(true);
    pal.setColor(ui->lbl_fontColor->backgroundRole(),m_fontColor);
    ui->lbl_fontColor->setPalette(pal);
    if(fontColor.alpha() == 0) {
        ui->lbl_fontColor->setText("Transparent");
    } else {
        ui->lbl_fontColor->setText("");
    }
    if(ui->comboBox_style->currentText() != "None")
        update_grid();
}

void MainWindow::setLineColor(const QColor &lineColor)
{
    m_lineColor = lineColor;
    QPalette pal = ui->lbl_lineColor->palette();
    ui->lbl_lineColor->setAutoFillBackground(true);
    pal.setColor(ui->lbl_lineColor->backgroundRole(),m_lineColor);
    ui->lbl_lineColor->setPalette(pal);
    if(lineColor.alpha() == 0) {
        ui->lbl_lineColor->setText("Transparent");
    } else {
        ui->lbl_lineColor->setText("");
    }
    update_grid();
}

void MainWindow::setNumCols(int numCols)
{
    m_numCols = numCols;
}

void MainWindow::setNumRows(int numRows)
{
    m_numRows = numRows;
}

void MainWindow::setRadius(int radius)
{
    m_radius = radius;
}

// Draw/Update hex grid
void MainWindow::update_grid()
{
    if(!autoUpdate()) {
        return;
    }

    // Remove existing hexes
    foreach(QGraphicsItem *hex, m_cells) {
        m_scene->removeItem(hex);
        delete(hex);
    }
    m_cells.clear();
    delete(m_groupCells);

    // Initialize some variables
    qreal row_offset = 0;
    qreal col_offset = 0;
    int cnt = 1;

    // Loop through columns and rows to generate the hex grid
    for(int c = 0; c < numCols(); c++) {
        for(int r = 0; r < numRows(); r++) {
            Grid *grid = new Grid(this);
            if(ui->cmb_gridStyle->currentText() == "Hexagon") {
                grid->setStyle(Grid::HexagonGrid);
            } else {
                grid->setStyle(Grid::SquareGrid);
            }
            grid->setRadius(radius());
            grid->setRelativeScale(m_relativeScale);
            grid->setLineWidth(ui->doubleSpinBox_lineWidth->value());
            grid->setLineColor(lineColor());
            grid->setBackgroundColor(backgroundColor());
            grid->setOrientation(ui->comboBox_orientation->currentIndex());
            grid->setCenterPointColor(centerPointColor());
            grid->setAntiAliasing(antiAliasing());
            grid->setCenterPointSymbol(ui->cmb_centerPointSymbol->currentIndex());
            grid->setLabelFont(m_font);
            if(ui->comboBox_position->currentText() != "None") {
                switch(ui->comboBox_style->currentIndex()) {
                case 0: // A1 (row,col)
                {
                    int rem = r / 26;
                    switch (rem) {
                    case 0:
                        grid->setLabel(QString("%1%2")
                                      .arg(m_ABC.at(r))
                                      .arg(c+1));
                        break;
                    case 1:
                        grid->setLabel(QString("%1%2%3")
                                      .arg(m_ABC.at(r-(26*rem)))
                                      .arg(m_ABC.at(r-(26*rem)))
                                      .arg(c+1));
                        break;
                    case 2:
                        grid->setLabel(QString("%1%2%3%4")
                                      .arg(m_ABC.at(r-(26*rem)))
                                      .arg(m_ABC.at(r-(26*rem)))
                                      .arg(m_ABC.at(r-(26*rem)))
                                      .arg(c+1));
                        break;
                    case 3:
                        grid->setLabel(QString("%1%2%3%4%5")
                                      .arg(m_ABC.at(r-(26*rem)))
                                      .arg(m_ABC.at(r-(26*rem)))
                                      .arg(m_ABC.at(r-(26*rem)))
                                      .arg(m_ABC.at(r-(26*rem)))
                                      .arg(c+1));
                        break;
                    }
                    break;
                }
                case 1: // 1A (row,col);
                {
                    int rem = c / 26;
                    switch (rem) {
                    case 0:
                        grid->setLabel(QString("%1%2")
                                      .arg(r+1)
                                      .arg(m_ABC.at(c)));
                        break;
                    case 1:
                        grid->setLabel(QString("%1%2%3")
                                      .arg(r+1)
                                      .arg(m_ABC.at(c-(26*rem)))
                                      .arg(m_ABC.at(c-(26*rem))));
                        break;
                    case 2:
                        grid->setLabel(QString("%1%2%3%4")
                                      .arg(r+1)
                                      .arg(m_ABC.at(c-(26*rem)))
                                      .arg(m_ABC.at(c-(26*rem)))
                                      .arg(m_ABC.at(c-(26*rem))));
                        break;
                    case 3:
                        grid->setLabel(QString("%1%2%3%4%5")
                                      .arg(r+1)
                                      .arg(m_ABC.at(c-(26*rem)))
                                      .arg(m_ABC.at(c-(26*rem)))
                                      .arg(m_ABC.at(c-(26*rem)))
                                      .arg(m_ABC.at(c-(26*rem))));
                        break;
                    }
                    break;
                }
                case 2: // A1 (Column, Row)
                {
                    int rem = c / 26;
                    switch (rem) {
                    case 0:
                        grid->setLabel(QString("%1%2")
                                      .arg(m_ABC.at(c))
                                      .arg(r+1));
                        break;
                    case 1:
                        grid->setLabel(QString("%1%2%3")
                                      .arg(m_ABC.at(c-(26*rem)))
                                      .arg(m_ABC.at(c-(26*rem)))
                                      .arg(r+1));
                        break;
                    case 2:
                        grid->setLabel(QString("%1%2%3%4")
                                      .arg(m_ABC.at(c-(26*rem)))
                                      .arg(m_ABC.at(c-(26*rem)))
                                      .arg(m_ABC.at(c-(26*rem)))
                                      .arg(r+1));
                        break;
                    case 3:
                        grid->setLabel(QString("%1%2%3%4%5")
                                      .arg(m_ABC.at(c-(26*rem)))
                                      .arg(m_ABC.at(c-(26*rem)))
                                      .arg(m_ABC.at(c-(26*rem)))
                                      .arg(m_ABC.at(c-(26*rem)))
                                      .arg(r+1));
                        break;
                    }
                    break;
                }
                case 3: // 1A (Column, Row)
                {
                    int rem = r / 26;
                    switch (rem) {
                    case 0:
                        grid->setLabel(QString("%1%2")
                                      .arg(c+1)
                                      .arg(m_ABC.at(r)));
                        break;
                    case 1:
                        grid->setLabel(QString("%1%2%3")
                                      .arg(c+1)
                                      .arg(m_ABC.at(r-(26*rem)))
                                      .arg(m_ABC.at(r-(26*rem))));
                        break;
                    case 2:
                        grid->setLabel(QString("%1%2%3%4")
                                      .arg(c+1)
                                      .arg(m_ABC.at(r-(26*rem)))
                                      .arg(m_ABC.at(r-(26*rem)))
                                      .arg(m_ABC.at(r-(26*rem))));
                        break;
                    case 3:
                        grid->setLabel(QString("%1%2%3%4%5")
                                      .arg(c+1)
                                      .arg(m_ABC.at(r-(26*rem)))
                                      .arg(m_ABC.at(r-(26*rem)))
                                      .arg(m_ABC.at(r-(26*rem)))
                                      .arg(m_ABC.at(r-(26*rem))));
                        break;
                    }
                    break;
                }
                case 4: // 1,1 (row,col)
                {
                    grid->setLabel(QString("%1,%2").arg(r+1).arg(c+1));
                    break;
                }
                case 5: // 1,1 (col, row)
                {
                    grid->setLabel(QString("%1,%2").arg(c+1).arg(r+1));
                    break;
                }
                case 6: // 0101 (row, col)
                {
                    grid->setLabel(QString("%1%2").arg(r+1,2,10,QChar('0')).arg(c+1,2,10,QChar('0')));
                    break;
                }
                case 7: // 0101 (col, row)
                {
                    grid->setLabel(QString("%1%2").arg(c+1,2,10,QChar('0')).arg(r+1,2,10,QChar('0')));
                    break;
                }
                case 8: // 1...N (Left to right, wrap)
                {
                    grid->setLabel(QString("%1").arg(r*numCols()+c+1));
                    break;
                }
                case 9: // 1...N (Top to bottom, wrap)
                {
                    grid->setLabel(QString("%1").arg(cnt));
                    cnt++;
                    break;
                }
                default:
                {
                    break;
                }
                }
            } else {
                grid->setLabel("");
            }
            grid->setLabel(grid->label().prepend(ui->le_prefix->text()).append(ui->le_suffix->text()));
            grid->setRotation(ui->sb_labelRotation->value());
            grid->setLabelColor(m_fontColor);
            grid->setLabelLocation(ui->comboBox_position->currentIndex());
            m_cells << grid;
            m_scene->addItem(grid);

            // HEXAGON
            //==========================================================================================================
            if(ui->cmb_gridStyle->currentText() == "Hexagon") {
                // Determine row and column offsets depending upon hex orientation
                if(grid->orientation() < 1) {
                    if(c%2) {
                        row_offset = grid->yOffset()/2;
                    } else {
                        row_offset = 0;
                    }
                    if(ui->cb_alternateRowColumn->isChecked()) {
                        row_offset = -row_offset;
                    }
                    grid->setPos(c*grid->xOffset(),r*grid->yOffset()+row_offset);
                } else {
                    if(r%2) {
                        col_offset = 0;
                    } else {
                        col_offset = grid->xOffset()/2;
                    }
                    if(ui->cb_alternateRowColumn->isChecked()) {
                        col_offset = -col_offset;
                    }
                    grid->setPos(c*grid->xOffset()-col_offset,r*grid->yOffset());
                }

            // SQUARE
            //==========================================================================================================
            } else {
                if(grid->orientation() < 1) {
                    grid->setPos(c*grid->xOffset(),r * grid->yOffset());
                } else {
                    if(c%2) {
                        row_offset = 0;
                    } else {
                        row_offset = grid->yOffset();
                        if(ui->cb_alternateRowColumn->isChecked()) {
                            row_offset = -row_offset;
                        }
                    }
                    grid->setPos(c*grid->xOffset(),(r*grid->yOffset()-row_offset)+(r*grid->yOffset()));
                }
            }
        }
    }

    // Create a cell group to get width and height for scene rect
    m_groupCells = new QGraphicsItemGroup();
    m_groupCells = m_scene->createItemGroup(m_cells);
    m_scene->setSceneRect(m_groupCells->boundingRect().x()-ui->doubleSpinBox_lineWidth->value(),
                          m_groupCells->boundingRect().y()-ui->doubleSpinBox_lineWidth->value(),
                          m_groupCells->boundingRect().width()+ui->doubleSpinBox_lineWidth->value()*2,
                          m_groupCells->boundingRect().height()+ui->doubleSpinBox_lineWidth->value()*2);
    m_groupCells->setHandlesChildEvents(false);
    m_groupCells->setZValue(1);

    // Update grid size label
    ui->label_Size->setText(QString("%1 x %2 pixels")
                            .arg(m_scene->sceneRect().width(),0,'d',0)
                            .arg(m_scene->sceneRect().height(),0,'d',0));
    ui->graphicsView->centerOn(0,0);
    m_scene->update();
}

void MainWindow::setRelativeScale(const qreal &relativeScale)
{
    m_relativeScale = relativeScale;
    update_grid();
}

void MainWindow::on_sb_relativeScale_valueChanged(double arg1)
{
    setRelativeScale(arg1);
}

qreal MainWindow::relativeScale() const
{
    return m_relativeScale;
}

void MainWindow::on_btn_canvasColor_clicked()
{
    QColorDialog diag(this);
    QColor clr = diag.getColor(canvasColor(),this,"Canvas Color",
                               QColorDialog::ShowAlphaChannel);
    if(clr != QColor::Invalid) {
        setCanvasColor(clr);
    }
}

QColor MainWindow::canvasColor() const
{
    return m_canvasColor;
}

void MainWindow::setCanvasColor(const QColor &canvasColor)
{
    m_canvasColor = canvasColor;
    QPalette pal = ui->label_canvasColor->palette();
    ui->label_canvasColor->setAutoFillBackground(true);
    pal.setColor(ui->label_canvasColor->backgroundRole(),m_canvasColor);
    ui->label_canvasColor->setPalette(pal);
    if(canvasColor.alpha() == 0) {
        ui->label_canvasColor->setText("Transparent");
    } else {
        ui->label_canvasColor->setText("");
    }
    m_scene->setBackgroundBrush(m_canvasColor);
    m_scene->update();
}

void MainWindow::on_btn_fontStyle_clicked()
{
    QFontDialog diag(this);
    QFont old = ui->lbl_abc123->font();
    QFont fnt = diag.getFont(nullptr,old,nullptr,"Label Font");
    setFont(fnt);
}

void MainWindow::on_le_suffix_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);
    if(ui->comboBox_style->currentText() != "None")
        update_grid();
}

void MainWindow::on_le_prefix_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);
    if(ui->comboBox_style->currentText() != "None")
        update_grid();
}

void MainWindow::on_sb_labelRotation_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    if(ui->comboBox_style->currentText() != "None")
        update_grid();

}

void MainWindow::on_actionAbout_triggered()
{
    m_splash->show();
    m_splash->setToolTip("Click on image to close");
}

void MainWindow::on_actionExport_to_PNG_Image_triggered()
{
    if(m_backgroundColor.alpha() == 0 && m_canvasColor.alpha() != 0) {
        int res = QMessageBox::warning(this,"Transparency Conflict","The cell background has been set to"
                             " transparent, however the undelying canvas is not transparent.  The resulting"
                             " cell background in the save image will be the same as the canvas."
                             "  Continue saving?",QMessageBox::Yes | QMessageBox::No | QMessageBox::No);
        if(res == QMessageBox::No) {
            return;
        }
    }

    ui->graphicsView->zoomReset();
    m_scene->update();
    QSettings settings("grid.ini",QSettings::IniFormat);
    QString imagePath = settings.value("imagePath",QDir::currentPath()).toString();
    QString fileName = QFileDialog::getSaveFileName(this,"Save Image",
                                                    imagePath.append("/untitled.png"),
                                                    "Images (*.png);;All Files (*.*)");
    if(fileName.isNull()) {
        return;
    }

    settings.setValue("imagePath",QFileInfo(fileName).path());

    QCursor cur = this->cursor();
    setCursor(Qt::WaitCursor);
    setWindowTitle("Grid Generator (Please wait, saving image file...)");
    qApp->processEvents();

    int width = static_cast<int>(ui->graphicsView->sceneRect().width());
    int height = static_cast<int>(ui->graphicsView->sceneRect().height());

    QImage *img = new QImage(width,height,QImage::Format_ARGB32_Premultiplied);
    img->fill(canvasColor());
    QPainter *p = new QPainter(img);
    ui->graphicsView->scene()->render(p);
    p->end();

    delete(img);
    delete(p);

    // NOTE:  for some reason, we need to render the image twice if relative scaling is used,
    // otherwise only the visible viewport area is updated within the image when saving.
    // Is this a bug?  Probably not, I'm likely not doing somehting correctly...
    img = new QImage(width,height,QImage::Format_ARGB32_Premultiplied);
    if(backgroundColor().alpha() == 0) {
        img->fill(Qt::transparent);
    } else {
        img->fill(canvasColor());
    }
    p = new QPainter(img);
    ui->graphicsView->scene()->render(p);
    p->end();

    img->save(fileName);

    delete(img);
    delete(p);

    setCursor(cur);
    setWindowTitle("Grid Generator");
    QMessageBox::information(this,"Grid Image","File Saved");
}

void MainWindow::on_btn_reset_clicked()
{
    setAutoUpdate(false);

    setBackgroundColor(QColor(255,255,255,255));
    setCanvasColor(QColor(255,255,255,255));
    setCenterPointColor(QColor(0,0,0,255));
    setFont(QFont("Arial",8,QFont::Normal));
    setFontColor(QColor(0,0,0,255));
    setLineColor(QColor(0,0,0,255));
    ui->cmb_gridStyle->setCurrentIndex(0);
    ui->cmb_centerPointSymbol->setCurrentIndex(0);
    ui->checkBox_antiAliasing->setChecked(true);
    ui->cb_alternateRowColumn->setChecked(false);
    ui->comboBox_orientation->setCurrentIndex(0);
    ui->comboBox_position->setCurrentIndex(0);
    ui->comboBox_style->setCurrentIndex(0);
    ui->doubleSpinBox_lineWidth->setValue(1.0);
    ui->le_prefix->setText("");
    ui->le_suffix->setText("");
    ui->sb_labelRotation->setValue(0);
    ui->sb_relativeScale->setValue(1.00);
    ui->spinBox_numColumns->setValue(5);
    ui->spinBox_numRows->setValue(5);
    ui->spinBox_radius->setValue(40);

    setAutoUpdate(true);
    update_grid();
}

void MainWindow::on_cmb_centerPointSymbol_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    update_grid();
}

void MainWindow::on_cmb_gridStyle_activated(const QString &arg1)
{
    setAutoUpdate(false);
    if(arg1 == "Hexagon") {
        ui->sb_labelRotation->setMaximum(330);
        ui->sb_labelRotation->setSingleStep(30);
    } else {
        ui->sb_labelRotation->setMaximum(315);
        ui->sb_labelRotation->setSingleStep(45);
    }
    ui->sb_labelRotation->setValue(0);
    setAutoUpdate(true);
    update_grid();
}
