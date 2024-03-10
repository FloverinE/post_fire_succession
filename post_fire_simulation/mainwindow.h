#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include <vector>
#include <QImage>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int number_of_simulation_years = 0;

    // Vectors to store the population counts as sum of all patches at each time step
    std::vector<std::vector<int>> birch_pop_total;
    std::vector<std::vector<int>> oak_pop_total;
    std::vector<std::vector<int>> birch_pop_burnt_area_total;
    std::vector<std::vector<int>> oak_pop_burnt_area_total;

    int N_trees = 0;
    bool deadwood_removed = false;


private slots:
    void on_setup_button_clicked();
    void on_go_button_clicked();
    void setup_map();
    void setup_trees();
    void setup_patches();
    void setup_burnt_area();
    void perform_dispersal();
    void perform_pop_dynamics();
    void setup_min_distance_to_tree();
    void count_populations();

    void update_map();
    void clear_charts();
    void draw_charts();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QImage image;  // Declare image as a member variable

    //
    QRgb color_seeds = qRgb(0, 128, 0); // green color
    QRgb color_burnt_area = qRgb(0, 0, 0); // black color


    // needed for plotting
    QChart *N_birch_pop_chart;
    QChart *N_birch_burnt_area_chart;
    QChart *N_oak_pop_chart;
    QChart *N_oak_burnt_area_chart;
};
#endif // MAINWINDOW_H
