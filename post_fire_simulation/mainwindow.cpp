// load class files
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "patch.h"
#include "tree.h"

// include necessary libraries
#include <iostream>
#include <random>
#include <vector>
#include <QImage>
#include <string>


using namespace std;

// constructor
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    int number_of_simulation_years = 1;

    // initialize population vectors with the desired size
    birch_pop_total.resize(number_of_simulation_years, std::vector<int>(5, 0));
    oak_pop_total.resize(number_of_simulation_years, std::vector<int>(5, 0));
    birch_pop_burnt_area_total.resize(number_of_simulation_years, std::vector<int>(5, 0));
    oak_pop_burnt_area_total.resize(number_of_simulation_years, std::vector<int>(5, 0));

    // birch population charts
    N_birch_pop_chart = new QChart();                               // initialize the chart
    ui->N_birch_pop_chart->setChart(N_birch_pop_chart);             // assign the chart to the QChartView objects in the ui
    ui->N_birch_pop_chart->setRenderHint(QPainter::Antialiasing);   // set the rendering to antialiasing
    N_birch_burnt_area_chart = new QChart();
    ui->N_birch_burnt_area_chart->setChart(N_birch_burnt_area_chart);
    ui->N_birch_burnt_area_chart->setRenderHint(QPainter::Antialiasing);

    // same for oak population charts
    N_oak_pop_chart = new QChart();
    ui->N_oak_pop_chart->setChart(N_oak_pop_chart);
    ui->N_oak_pop_chart->setRenderHint(QPainter::Antialiasing);
    N_oak_burnt_area_chart = new QChart();
    ui->N_oak_burnt_area_chart->setChart(N_oak_burnt_area_chart);
    ui->N_oak_burnt_area_chart->setRenderHint(QPainter::Antialiasing);
}

// destructor
MainWindow::~MainWindow()
{
    delete ui;                  // delete the ui
    delete N_birch_pop_chart;   // delete the birch population chart etc
    delete N_birch_burnt_area_chart;
    delete N_oak_pop_chart;
    delete N_oak_burnt_area_chart;
}

// define the size of the map
// 1 patch = 1 pixel on the map = 5m x 5m = 25m^2
const int x_size = 300;         // number of horizontal pixels
const int y_size = 300;         // number of vertical pixels
const int area_to_ha_conv_factor = x_size * y_size / 10000;             // conversion factor from x and y extent in pixels to hectares for number of trees per hectare
const int patch_edge_m = 5;     // size of a patch  in meters
const int pixel_to_ha_conv_factor = 100 / (patch_edge_m * patch_edge_m);  // conversion factor from pixels to hectares for population density

// declaration of random number generators
std::random_device rd;                                              // used to obtain a seed for the random number engine
std::mt19937 gen(rd());                                             // standard mersenne_twister_engine seeded with rd()
std::uniform_int_distribution<int> rand_x_cor(0, x_size - 1);       // random x coordinate
std::uniform_int_distribution<int> rand_y_cor(0, y_size - 1);       // random y coordinate in case of rectangular map
std::uniform_real_distribution<float> rand_float_01(0.0f, 1.0f);    // random float between 0 and 1 for dispersal distance and seedling survival

// setup procedure grouping sub-procedures
void MainWindow::on_setup_button_clicked()
{
    ui->progress_output_textEdit->clear(); // clear the output in the ui
    setup_map();                    // create the map
    setup_patches();                // create the patches
    setup_trees();                  // create the trees
    setup_burnt_area();             // create the burnt area if checkbox was activated
    setup_min_distance_to_tree();   // calculate the minimum distance to the closest tree for each patch
    count_populations();            // count the populations of seeds in each patch (0 at beginning)
    clear_charts();                 // clear the population charts
    update_map();                   // update the map drawing
}

/**
 * @brief MainWindow::on_go_button_clicked()
 * Function to simulate the annual dispersal and population dynamics procedures
 */


void MainWindow::on_go_button_clicked()
{
    // warning for zero trees
    if (N_trees == 0) {
        cout << "Error: Cannot simulate with zero trees." << endl;
        ui->progress_output_textEdit->append("Error: Cannot simulate with zero trees."); // print to output in ui as well
        return;
    }
    // get the number of years to simulate from the ui spin box
    // perform the annual procedures
    for(int i = 0; i < number_of_simulation_years; ++i){
        perform_dispersal();        // seeds dispersal per tree
        perform_pop_dynamics();     // seed and sapling population dynamics according to matrix model
        count_populations();        // count the populations of seeds in each patch
        ui->progress_output_textEdit->append("simulated year " + QString::number(i+1) + " out of " + QString::number(number_of_simulation_years) + " years");
    }
    update_map();                   // update the map drawing
    draw_charts();                  // after simulation, draw the population charts for birch and oak
}

/**
 * @brief MainWindow::setup_map
 * Function to setup the map and assign the user selected value for the number of years to be simulated
 */
void MainWindow::setup_map() {
    scene = new QGraphicsScene;
    // and hook the scene to main_map
    ui->main_map->setScene(scene);
    number_of_simulation_years = ui->N_years_spinBox->value();      // set the number of simulation years to the value of the ui spinbox

    // declare and initialize an image
    ui->main_map->resize(x_size, y_size);
    image = QImage(x_size, y_size, QImage::Format_RGB32);
    image.fill(Qt::white);
    scene->addPixmap(QPixmap::fromImage(image));
}

/**
 * @brief MainWindow::setup_trees
 * Function to place the trees on the map and assign species according to the user selected ratio of birch to oak
 */
std::vector<tree> trees;        // vector of tree objects
void MainWindow::setup_trees() {
    trees.clear();
    N_trees = ui->N_trees_spinBox->value() * area_to_ha_conv_factor; // get the number of trees from the ui spinbox, multiply by factor to scale to ha
    float species_ratio = ui->species_ratio_spinBox->value();
    int N_birch_trees = N_trees * (1 - species_ratio);
    // loop over the number of trees
    for (int i = 0; i < N_trees; ++i) {
        trees.emplace_back(i, std::vector<int>{}, 'b', 1, 10);  // create dummy tree object
        trees[i].x_y_cor = {rand_x_cor(gen), rand_y_cor(gen)};          // assign random x and y coordinates
        if (i < N_birch_trees){                                 // assign birch based on species_ratio
            trees[i].species = 'b';                             // birch
            trees[i].update_species_params();                   // update species parameters
            image.setPixel(trees[i].x_y_cor[0], trees[i].x_y_cor[1], trees[i].color); // set pixel color
        } else {                                                // rest is oak
            trees[i].species = 'o';
            trees[i].update_species_params();
            image.setPixel(trees[i].x_y_cor[0], trees[i].x_y_cor[1], trees[i].color);
        }
        trees[i].id = i;                                        // assign tree id
    }
    scene->addPixmap(QPixmap::fromImage(image));                // update the map
}

/**
 * @brief MainWindow::setup_patches
 *  Patch setup procedure
    - clear the patches vector
    - loop over the x and y coordinates and create the according patch_id
    - create the patch object and add it to the patches vector
 */
std::vector<patch> patches; // vector of patch objects
void MainWindow::setup_patches() {
    patches.clear();

    for (int i = 0; i < x_size; i++) {
        for (int j = 0; j < y_size; j++) {
            std::string patch_id = std::to_string(i) + "_" + std::to_string(j);
            patches.emplace_back(patch_id, std::vector<int>{i, j}, std::vector<int>{0, 0});
        }
    }
}


/**
 * @brief setup_burnt_area
 *  Function to setup the area burnt by fire
  - circular shape, user determines radius from center of the map using spinBox
  - burnt patches are painted black
  - deadwood removal checkbox determines the if burnt trees are removed and therefore more light, but less water availability
 */

int N_burnt_patches = 0;    // count the number of burnt patches to calculate area

void MainWindow::setup_burnt_area(){
    // first check if forest fire is to be simulated according to ui checkbox
    bool simulate_fire = ui->sim_fire_checkBox->isChecked();

    // create burnt patch in the center of the map,
    // user can determine the radius of the burnt area [m] and if deadwood is to be removed
    if (simulate_fire) {
        // output the number of trees before the fire
        cout << "Number of trees before fire: " << trees.size() << endl;
        ui->progress_output_textEdit->append("Number of trees before fire: " + QString::number(trees.size())); // print to output in ui as well

        // burnt patches emerge from the center of the map
        int x_center = x_size / 2; // calculate the central x coordinate
        int y_center = y_size / 2; // and y accordingly
        int radius = ui->burnt_area_radius_spinBox->value();

        // paint the patches in the burnt area black
        for (int i = x_center - radius; i <= x_center + radius; i++) {
            for (int j = y_center - radius; j <= y_center + radius; j++) {
                if ((i - x_center) * (i - x_center) + (j - y_center) * (j - y_center) <= radius * radius) {
                    image.setPixel(i, j, color_burnt_area);
                    N_burnt_patches++; // increment the number of burnt patches
                }
            }
        }
        scene->addPixmap(QPixmap::fromImage(image)); // update the map with black burnt area to later check if patches are burnt

        // additional user input with spinBox: are the burnt trees removed or not
        bool deadwood_removed = ui->deadwood_removed_checkBox->isChecked();
        for (size_t i = 0; i < trees.size(); ++i) {
            if (image.pixel(trees[i].x_y_cor[0], trees[i].x_y_cor[1]) == color_burnt_area) {
                if (deadwood_removed) {
                    trees.erase(trees.begin() + static_cast<int>(i));
                    --i;
                } else {
                    trees[i].set_burnt(); // set tree status to burnt, can therefore not disperse seeds anymore, but will still influence the light availability
                }
            }
        }

        // loop over patches to set burnt status according to color assigned previously
        for (auto& p : patches){
            if (image.pixel(p.x_y_cor[0], p.x_y_cor[1]) == color_burnt_area){
                p.set_burnt();
            }
        }

        // output the number of trees left after the fire
        cout << "Number of trees after fire: " << trees.size() << endl;
        ui->progress_output_textEdit->append("Number of trees after fire: " + QString::number(trees.size())); // print to output in ui as well
    }
}

/**
 * @brief MainWindow::setup_min_distance_to_tree
 * Function to calculate the minimum euclidean distance between each patch and the closest tree
 * - loop over the patches and calculate the distance to each tree
 * - store the minimum distance in the patch object
 */
void MainWindow::setup_min_distance_to_tree() {
    if (trees.empty()) {                                        // check if there are any trees to compute distance to
        std::cerr << "Error: No trees to compute distance to" << std::endl;
        return;
    } else {                                                    // if there are trees, compute the distance to each patch
        for (auto& p : patches) {
            std::vector<float> distances(trees.size(), 0.0f);   // initialize distances with 0 for each patch

            for (unsigned int i = 0; i < trees.size(); i++) {   // loop over the trees
                float distance = sqrt(pow(trees[i].x_y_cor[0] - p.x_y_cor[0], 2) + pow(trees[i].x_y_cor[1] - p.x_y_cor[1], 2));
                distances[i] = distance;
            }

            auto min_distance_iter = std::min_element(distances.begin(), distances.end());
            // update patch variables accordingly
            p.distance_to_tree = *min_distance_iter;            // store the minimum distance
            // calculate light availability based on distance to trees
            if(p.distance_to_tree < 6){                         // below 6*5m = 30m distance, light availability is scaled to distance
                p.light_availability = 1 - (1 / p.distance_to_tree);
            } else {                                            // full light availability if distance to trees is greater than 30m
                p.light_availability = 1;
            }
            if(p.burnt & deadwood_removed){                     // if the patch is burnt and deadwood removed, set water availability to 0.5
                p.water_availability = 0.5;
            }

        }
    }
    cout << "Patch: 1 has distance to tree: " << patches[1].distance_to_tree << " and light availability: " << patches[1].light_availability << " and water availability" << patches[1].water_availability << endl;
    scene->addPixmap(QPixmap::fromImage(image)); // update the map
}

/**
 * @brief MainWindow::perform_dispersal
 * Procedure that is conducted each time step
 * - loop over the trees and calculate the real seed production as a random value between 0 and 1 multiplied by the max seed production
 * - seeds are dispersed in a uniform random 360 degree direction
 * - dispersal distance is modelled as exponential function
 * - seeds are registered to the destination patch
 */
void MainWindow::perform_dispersal() {
    for (auto& t : trees) {
        if(t.burnt == false){
            int real_seed_production = t.max_seed_production * rand_float_01(gen);     // real seed production as random number * max seed production
            for (int i = 1; i <= real_seed_production; i++) {
                float direction = 2 * M_PI * i / real_seed_production;             // direction of seed dispersal

                float distance_decay = std::pow(2, -3 * rand_float_01(gen));          // distance decay of seed dispersal

                int offset_x = static_cast<int>(t.dispersal_factor * distance_decay * cos(direction));
                int offset_y = static_cast<int>(t.dispersal_factor * distance_decay * sin(direction));

                int new_x = t.x_y_cor[0] + offset_x;
                int new_y = t.x_y_cor[1] + offset_y;

                // check if seed landed in the map extent
                if (new_x >= 0 && new_x < x_size && new_y >= 0 && new_y < y_size) {

                    image.setPixel(new_x, new_y, color_seeds);

                    for (auto& p : patches) { //loop over patches to find the patch where the seed landed
                        if (p.x_y_cor[0] == new_x && p.x_y_cor[1] == new_y) {
                            p.update_N_seeds(1, t.species);
                        }
                    }
                }
            }
        }
    }
    scene->addPixmap(QPixmap::fromImage(image));
}

/**
 * @brief MainWindow::perform_pop_dynamics
 *  Procdure that is conducted each time step
 * - working like a matrix model with 5 stages (seeds -> germination -> height class 1 to 4)
 * - probabilistic mortality and growth into next stages, rest is survival
 *    first: advancement of height class 3 into 4 to not have saplings from height class 2
 *        advancing and dying at the same time
 *    next:  continue with height class 3 down to the seeds

 * possible extension:
 * - implement growth rate dependent on height class,
 *   i.e. higher growth rate for lower height classes but lower if the taller saplings create too much shade
 */
void MainWindow::perform_pop_dynamics() {
    for(unsigned int i = 0; i < patches.size(); i++){ // loop over all patches
        for (int j = 0; j < 2; j++) {       // loop over both species => birch 0 and oak 1
            float mortality_factor = patches[i].mortality_rate * (1 - patches[i].light_availability) * (1 - patches[i].water_availability); // combined mortality rate
            float growth_factor =    patches[i].growth_rate *    patches[i].light_availability * patches[i].water_availability; // combined mortality rate

            // first height class 4 mortality as no further growth is implemented
            if(patches[i].N_height_class_4[j] > 0){   // only continue if there is at least 1 sapling in height class 4
                for(int k = 0; k < patches[i].N_height_class_4[j]; k++){ // loop over all saplings in height class 4
                    if(rand_float_01(gen) < mortality_factor){     // probability check for mortality
                        patches[i].N_height_class_4[j] -= 1;             // if passed, sapling dies
                    }
                }
            }
            // same for height class 3 and so on
            if(patches[i].N_height_class_3[j] > 0){   // only continue if there is at least 1 sapling in height class 3
                for(int k = 0; k < patches[i].N_height_class_3[j]; k++){ // loop over all saplings in height class 3
                    if(rand_float_01(gen) < mortality_factor){     // probability check for mortality
                        patches[i].N_height_class_3[j] -= 1;             // if passed, sapling dies
                    }
                    if(rand_float_01(gen) < growth_factor){        // probability check for growth
                        patches[i].N_height_class_4[j] += 1;             // if passed, sapling advances to height class 4
                        patches[i].N_height_class_3[j] -= 1;             // and is removed from height class 3
                    }
                }
            }
            if(patches[i].N_height_class_2[j] > 0){
                for(int k = 0; k < patches[i].N_height_class_2[j]; k++){
                    if(rand_float_01(gen) < mortality_factor){
                        patches[i].N_height_class_2[j] -= 1;
                    }
                    if(rand_float_01(gen) < growth_factor){
                        patches[i].N_height_class_3[j] += 1;
                        patches[i].N_height_class_2[j] -= 1;
                    }
                }
            }
            if(patches[i].N_height_class_1[j] > 0){
                for(int k = 0; k < patches[i].N_height_class_1[j]; k++){
                    if(rand_float_01(gen) < mortality_factor){
                        patches[i].N_height_class_1[j] -= 1;
                    }
                    if(rand_float_01(gen) < patches[i].growth_rate){
                        patches[i].N_height_class_2[j] += 1;
                        patches[i].N_height_class_1[j] -= 1;
                    }
                }
            }
            if(patches[i].N_seeds[j] > 0){
                for (int k = 0; k < patches[i].N_seeds[j]; k++) {
                    if (rand_float_01(gen) < mortality_factor) {
                        patches[i].N_seeds[j] -= 1;
                    }
                    if (rand_float_01(gen) < growth_factor) {
                        patches[i].N_height_class_1[j] += 1;
                        patches[i].N_seeds[j] -= 1;
                    }
                }
            }
        }
    }
}

/**
 * @brief MainWindow::count_populations
 * Procedure conducted each time step
 * count the population size of the different life stages from seed to height class 1-4 in the patches
 * population size counted separately for each species and for burnt patches
 */
void MainWindow::count_populations() {
    std::vector<int> birch_pop = {0, 0, 0, 0, 0};
    std::vector<int> oak_pop = {0, 0, 0, 0, 0};
    std::vector<int> birch_pop_burnt_area = {0, 0, 0, 0, 0};
    std::vector<int> oak_pop_burnt_area = {0, 0, 0, 0, 0};

    for (auto& p : patches) {
        birch_pop[0] += p.N_seeds[0];
        birch_pop[1] += p.N_height_class_1[0];
        birch_pop[2] += p.N_height_class_2[0];
        birch_pop[3] += p.N_height_class_3[0];
        birch_pop[4] += p.N_height_class_4[0];

        oak_pop[0] += p.N_seeds[1];
        oak_pop[1] += p.N_height_class_1[1];
        oak_pop[2] += p.N_height_class_2[1];
        oak_pop[3] += p.N_height_class_3[1];
        oak_pop[4] += p.N_height_class_4[1];

        if(p.burnt == true){
            birch_pop_burnt_area[0] += p.N_seeds[0];
            birch_pop_burnt_area[1] += p.N_height_class_1[0];
            birch_pop_burnt_area[2] += p.N_height_class_2[0];
            birch_pop_burnt_area[3] += p.N_height_class_3[0];
            birch_pop_burnt_area[4] += p.N_height_class_4[0];

            oak_pop_burnt_area[0] += p.N_seeds[1];
            oak_pop_burnt_area[1] += p.N_height_class_1[1];
            oak_pop_burnt_area[2] += p.N_height_class_2[1];
            oak_pop_burnt_area[3] += p.N_height_class_3[1];
            oak_pop_burnt_area[4] += p.N_height_class_4[1];
        }
    }

    birch_pop_total.push_back(birch_pop);
    birch_pop_burnt_area_total.push_back(birch_pop_burnt_area);
    oak_pop_total.push_back(oak_pop);
    oak_pop_burnt_area_total.push_back(oak_pop_burnt_area);
}

/**
 * @brief MainWindow::update_map
 *  Function to "refresh" the map according to present population density of all seeds and saplings per patch
 * - N_seeds_saplings are scaled in green
 * - trees are displayed as 5 * 5 pixels for improved visibility
 */
void MainWindow::update_map(){ // not used as of now
    std::vector<int> N_seeds_saplings(patches.size(), 0);
    for (unsigned int i = 0; i < patches.size(); i++) {
        N_seeds_saplings[i] = patches[i].get_all_N_seeds_saplings();
    }
    auto max_N_seeds_saplings_iter = std::max_element(N_seeds_saplings.begin(), N_seeds_saplings.end());
    int max_N_seeds_saplings = *max_N_seeds_saplings_iter;

    for (auto& p : patches) {
        int patch_pop = p.get_all_N_seeds_saplings();
        // set pixel color based on total number of seedlings and saplings per patch (max density is full green)
        if (patch_pop > 0){
            image.setPixelColor(p.x_y_cor[0], p.x_y_cor[1],  QColor(0, 255, 0, 255 * patch_pop / max_N_seeds_saplings));
        }
    }
    // trees mapped last to ensure they are visible
    // display trees as 5 * 5 pixels for better visibility
    for (auto& t : trees) {
        for(int i = -2; i <= 2; i++){
            for(int j = -2; j <= 2; j++){
                if (t.x_y_cor[0] + i >= 0 && t.x_y_cor[0] + i < x_size && t.x_y_cor[1] + j >= 0 && t.x_y_cor[1] + j < y_size)
                    image.setPixel(t.x_y_cor[0] + i, t.x_y_cor[1] + j, t.color);
            }
        }
    }
    scene->addPixmap(QPixmap::fromImage(image));
}

/**
 * @brief MainWindow::clear_charts
 *  Function to clear all output vectors and charts
 */
void MainWindow::clear_charts()
{
    //clear all output vectors
    birch_pop_total.clear();
    birch_pop_burnt_area_total.clear();
    oak_pop_total.clear();
    oak_pop_burnt_area_total.clear();

    // clear charts for setup
    N_birch_pop_chart->removeAllSeries();
    N_birch_burnt_area_chart->removeAllSeries();
    N_oak_pop_chart->removeAllSeries();
    N_oak_burnt_area_chart->removeAllSeries();
}

/*
 Function to draw the four output charts in the ui
 - one series per species and life stage in all patches and in just the burnt area
 - 5 series per chart
 - values scaled to hectares for comparable output
 */

void MainWindow::draw_charts(){
    QLineSeries *N_birch_seeds_series = new QLineSeries();
    N_birch_seeds_series->setColor(Qt::black); // default color: blue
    N_birch_seeds_series->setName("Seeds");
    QLineSeries *N_birch_hc1_series = new QLineSeries();
    N_birch_hc1_series->setColor(Qt::black); // default color: blue
    N_birch_hc1_series->setName("Height class 1");
    QLineSeries *N_birch_hc2_series = new QLineSeries();
    N_birch_hc2_series->setColor(Qt::black); // default color: blue
    N_birch_hc2_series->setName("Height class 2");
    QLineSeries *N_birch_hc3_series = new QLineSeries();
    N_birch_hc3_series->setColor(Qt::black); // default color: blue
    N_birch_hc3_series->setName("Height class 3");
    QLineSeries *N_birch_hc4_series = new QLineSeries();
    N_birch_hc4_series->setColor(Qt::black); // default color: blue
    N_birch_hc4_series->setName("Height class 4");

    QLineSeries *N_birch_burnt_area_seeds_series = new QLineSeries();
    N_birch_burnt_area_seeds_series->setColor(Qt::black); // default color: blue
    N_birch_burnt_area_seeds_series->setName("Seeds");
    QLineSeries *N_birch_burnt_area_hc1_series = new QLineSeries();
    N_birch_burnt_area_hc1_series->setColor(Qt::black); // default color: blue
    N_birch_burnt_area_hc1_series->setName("Height class 1");
    QLineSeries *N_birch_burnt_area_hc2_series = new QLineSeries();
    N_birch_burnt_area_hc2_series->setColor(Qt::black); // default color: blue
    N_birch_burnt_area_hc2_series->setName("Height class 2");
    QLineSeries *N_birch_burnt_area_hc3_series = new QLineSeries();
    N_birch_burnt_area_hc3_series->setColor(Qt::black); // default color: blue
    N_birch_burnt_area_hc3_series->setName("Height class 3");
    QLineSeries *N_birch_burnt_area_hc4_series = new QLineSeries();
    N_birch_burnt_area_hc4_series->setColor(Qt::black); // default color: blue
    N_birch_burnt_area_hc4_series->setName("Height class 4");

    QLineSeries *N_oak_seeds_series = new QLineSeries();
    N_oak_seeds_series->setColor(Qt::black); // default color: blue
    N_oak_seeds_series->setName("Seeds");
    QLineSeries *N_oak_hc1_series = new QLineSeries();
    N_oak_hc1_series->setColor(Qt::black); // default color: blue
    N_oak_hc1_series->setName("Height class 1");
    QLineSeries *N_oak_hc2_series = new QLineSeries();
    N_oak_hc2_series->setColor(Qt::black); // default color: blue
    N_oak_hc2_series->setName("Height class 2");
    QLineSeries *N_oak_hc3_series = new QLineSeries();
    N_oak_hc3_series->setColor(Qt::black); // default color: blue
    N_oak_hc3_series->setName("Height class 3");
    QLineSeries *N_oak_hc4_series = new QLineSeries();
    N_oak_hc4_series->setColor(Qt::black); // default color: blue
    N_oak_hc4_series->setName("Height class 4");

    QLineSeries *N_oak_burnt_area_seeds_series = new QLineSeries();
    N_oak_burnt_area_seeds_series->setColor(Qt::black); // default color: blue
    N_oak_burnt_area_seeds_series->setName("Seeds");
    QLineSeries *N_oak_burnt_area_hc1_series = new QLineSeries();
    N_oak_burnt_area_hc1_series->setColor(Qt::black); // default color: blue
    N_oak_burnt_area_hc1_series->setName("Height class 1");
    QLineSeries *N_oak_burnt_area_hc2_series = new QLineSeries();
    N_oak_burnt_area_hc2_series->setColor(Qt::black); // default color: blue
    N_oak_burnt_area_hc2_series->setName("Height class 2");
    QLineSeries *N_oak_burnt_area_hc3_series = new QLineSeries();
    N_oak_burnt_area_hc3_series->setColor(Qt::black); // default color: blue
    N_oak_burnt_area_hc3_series->setName("Height class 3");
    QLineSeries *N_oak_burnt_area_hc4_series = new QLineSeries();
    N_oak_burnt_area_hc4_series->setColor(Qt::black); // default color: blue
    N_oak_burnt_area_hc4_series->setName("Height class 4");

    N_birch_seeds_series->clear();
    N_birch_hc1_series->clear();
    N_birch_hc2_series->clear();
    N_birch_hc3_series->clear();
    N_birch_hc4_series->clear();

    N_birch_burnt_area_seeds_series->clear();
    N_birch_burnt_area_hc1_series->clear();
    N_birch_burnt_area_hc2_series->clear();
    N_birch_burnt_area_hc3_series->clear();
    N_birch_burnt_area_hc4_series->clear();

    N_oak_seeds_series->clear();
    N_oak_hc1_series->clear();
    N_oak_hc2_series->clear();
    N_oak_hc3_series->clear();
    N_oak_hc4_series->clear();

    N_oak_burnt_area_seeds_series->clear();
    N_oak_burnt_area_hc1_series->clear();
    N_oak_burnt_area_hc2_series->clear();
    N_oak_burnt_area_hc3_series->clear();
    N_oak_burnt_area_hc4_series->clear();

    // fill in the series with the data for each year
    for (int time = 0; time < number_of_simulation_years; time++) {
        N_birch_seeds_series->append(time, birch_pop_total[time][0] / pixel_to_ha_conv_factor);   // divide by conversion factor to get the number per ha as 300 m * 300 m = 90000 m^2 = 9 ha
        N_birch_hc1_series->append(time, birch_pop_total[time][1] / pixel_to_ha_conv_factor);
        N_birch_hc2_series->append(time, birch_pop_total[time][2] / pixel_to_ha_conv_factor);
        N_birch_hc3_series->append(time, birch_pop_total[time][3] / pixel_to_ha_conv_factor);
        N_birch_hc4_series->append(time, birch_pop_total[time][4] / pixel_to_ha_conv_factor);

        N_birch_burnt_area_seeds_series->append(time, birch_pop_burnt_area_total[time][0] / pixel_to_ha_conv_factor);
        N_birch_burnt_area_hc1_series->append(time, birch_pop_burnt_area_total[time][1] / pixel_to_ha_conv_factor);
        N_birch_burnt_area_hc2_series->append(time, birch_pop_burnt_area_total[time][2] / pixel_to_ha_conv_factor);
        N_birch_burnt_area_hc3_series->append(time, birch_pop_burnt_area_total[time][3] / pixel_to_ha_conv_factor);
        N_birch_burnt_area_hc4_series->append(time, birch_pop_burnt_area_total[time][4] / pixel_to_ha_conv_factor);

        N_oak_seeds_series->append(time, oak_pop_total[time][0] / pixel_to_ha_conv_factor);
        N_oak_hc1_series->append(time, oak_pop_total[time][1] / pixel_to_ha_conv_factor);
        N_oak_hc2_series->append(time, oak_pop_total[time][2] / pixel_to_ha_conv_factor);
        N_oak_hc3_series->append(time, oak_pop_total[time][3] / pixel_to_ha_conv_factor);
        N_oak_hc4_series->append(time, oak_pop_total[time][4] / pixel_to_ha_conv_factor);

        N_oak_burnt_area_seeds_series->append(time, oak_pop_burnt_area_total[time][0] / pixel_to_ha_conv_factor);
        N_oak_burnt_area_hc1_series->append(time, oak_pop_burnt_area_total[time][1] / pixel_to_ha_conv_factor);
        N_oak_burnt_area_hc2_series->append(time, oak_pop_burnt_area_total[time][2] / pixel_to_ha_conv_factor);
        N_oak_burnt_area_hc3_series->append(time, oak_pop_burnt_area_total[time][3] / pixel_to_ha_conv_factor);
        N_oak_burnt_area_hc4_series->append(time, oak_pop_burnt_area_total[time][4] / pixel_to_ha_conv_factor);
    }

    // Create legends for each chart
    QLegend *birchPopLegend = N_birch_pop_chart->legend();
    birchPopLegend->setAlignment(Qt::AlignRight);

    QLegend *birchBurntAreaLegend = N_birch_burnt_area_chart->legend();
    birchBurntAreaLegend->setAlignment(Qt::AlignRight);

    QLegend *oakPopLegend = N_oak_pop_chart->legend();
    oakPopLegend->setAlignment(Qt::AlignRight);

    QLegend *oakBurntAreaLegend = N_oak_burnt_area_chart->legend();
    oakBurntAreaLegend->setAlignment(Qt::AlignRight);

    N_birch_pop_chart->addSeries(N_birch_seeds_series);
    N_birch_pop_chart->addSeries(N_birch_hc1_series);
    N_birch_pop_chart->addSeries(N_birch_hc2_series);
    N_birch_pop_chart->addSeries(N_birch_hc3_series);
    N_birch_pop_chart->addSeries(N_birch_hc4_series);
    N_birch_pop_chart->createDefaultAxes();
    N_birch_pop_chart->setTitle("Birch seed and sapling population");
    N_birch_pop_chart->axisX()->setTitleText("time [years]");
    N_birch_pop_chart->axisY()->setTitleText("population density [N/ha]");

    N_birch_burnt_area_chart->addSeries(N_birch_burnt_area_seeds_series);
    N_birch_burnt_area_chart->addSeries(N_birch_burnt_area_hc1_series);
    N_birch_burnt_area_chart->addSeries(N_birch_burnt_area_hc2_series);
    N_birch_burnt_area_chart->addSeries(N_birch_burnt_area_hc3_series);
    N_birch_burnt_area_chart->addSeries(N_birch_burnt_area_hc4_series);
    N_birch_burnt_area_chart->createDefaultAxes();
    N_birch_burnt_area_chart->setTitle("Birch seed and sapling population in burnt area");
    N_birch_burnt_area_chart->axisX()->setTitleText("time [years]");
    N_birch_burnt_area_chart->axisY()->setTitleText("population density [N/ha]");

    N_oak_pop_chart->addSeries(N_oak_seeds_series);
    N_oak_pop_chart->addSeries(N_oak_hc1_series);
    N_oak_pop_chart->addSeries(N_oak_hc2_series);
    N_oak_pop_chart->addSeries(N_oak_hc3_series);
    N_oak_pop_chart->addSeries(N_oak_hc4_series);
    N_oak_pop_chart->createDefaultAxes();
    N_oak_pop_chart->setTitle("Oak seed and sapling population");
    N_oak_pop_chart->axisX()->setTitleText("time [years]");
    N_oak_pop_chart->axisY()->setTitleText("population density [N/ha]");

    N_oak_burnt_area_chart->addSeries(N_oak_burnt_area_seeds_series);
    N_oak_burnt_area_chart->addSeries(N_oak_burnt_area_hc1_series);
    N_oak_burnt_area_chart->addSeries(N_oak_burnt_area_hc2_series);
    N_oak_burnt_area_chart->addSeries(N_oak_burnt_area_hc3_series);
    N_oak_burnt_area_chart->addSeries(N_oak_burnt_area_hc4_series);
    N_oak_burnt_area_chart->createDefaultAxes();
    N_oak_burnt_area_chart->setTitle("Oak seed and sapling population in burnt area");
    N_oak_burnt_area_chart->axisX()->setTitleText("time [years]");
    N_oak_burnt_area_chart->axisY()->setTitleText("population density [N/ha]");
}


