#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "grid.h"
#include <QSplashScreen>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QColor backgroundColor() const;
    QColor canvasColor() const;
    QColor centerPointColor() const;
    QColor fontColor() const;
    QColor lineColor() const;
    QFont font() const;
    bool antiAliasing() const;
    bool autoUpdate() const;
    int numCols() const;
    int numRows() const;
    int radius() const;
    qreal relativeScale() const;
    void setAntiAliasing(bool antiAliasing);
    void setAutoUpdate(bool autoUpdate);
    void setBackgroundColor(const QColor &backgroundColor);
    void setCanvasColor(const QColor &canvasColor);
    void setCenterPointColor(const QColor &centerPointColor);
    void setFont(const QFont &font);
    void setFontColor(const QColor &fontColor);
    void setLineColor(const QColor &lineColor);
    void setNumCols(int numCols);
    void setNumRows(int numRows);
    void setRadius(int radius);
    void setRelativeScale(const qreal &relativeScale);

private slots:
    void on_actionAbout_triggered();
    void on_actionExit_triggered();
    void on_actionExport_to_PNG_Image_triggered();
    void on_actionLoad_Template_triggered();
    void on_actionSave_Template_triggered();
    void on_btn_backgroundColor_clicked();
    void on_btn_canvasColor_clicked();
    void on_btn_centerPointColor_clicked();
    void on_btn_fontColor_clicked();
    void on_btn_fontStyle_clicked();
    void on_btn_lineColor_clicked();
    void on_btn_reset_clicked();
    void on_cb_alternateRowColumn_toggled(bool checked);
    void on_checkBox_antiAliasing_toggled(bool checked);
    void on_cmb_centerPointSymbol_currentIndexChanged(int index);
    void on_comboBox_orientation_currentIndexChanged(int index);
    void on_comboBox_position_currentIndexChanged(int index);
    void on_comboBox_style_currentIndexChanged(int index);
    void on_doubleSpinBox_lineWidth_valueChanged(double arg1);
    void on_le_prefix_textEdited(const QString &arg1);
    void on_le_suffix_textEdited(const QString &arg1);
    void on_sb_labelRotation_valueChanged(int arg1);
    void on_sb_relativeScale_valueChanged(double arg1);
    void on_spinBox_numColumns_valueChanged(int arg1);
    void on_spinBox_numRows_valueChanged(int arg1);
    void on_spinBox_radius_valueChanged(int arg1);
    void on_cmb_gridStyle_activated(const QString &arg1);

private:
    QColor m_backgroundColor;
    QColor m_canvasColor;
    QColor m_centerPointColor;
    QColor m_fontColor;
    QColor m_lineColor;
    QFont  m_font;
    QGraphicsItemGroup *m_groupCells;
    QGraphicsScene *m_scene;
    QList<QGraphicsItem*> m_cells;
    QSplashScreen *m_splash;
    QString m_prefix;
    QString m_suffix;
    Ui::MainWindow *ui;
    bool m_antiAliasing;
    bool m_autoUpdate;
    int m_numCols;
    int m_numRows;
    int m_radius;
    qreal m_relativeScale;
    void closeEvent(QCloseEvent *event);
    void init();
    void update_grid();

    const QStringList m_ABC = {"A","B","C","D","E","F","G","H","I","J","K","L","M",
                               "N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};
};

#endif // MAINWINDOW_H
