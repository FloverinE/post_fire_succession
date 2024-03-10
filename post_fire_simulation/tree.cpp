#include "tree.h"
#include <QImage>
#include <QColor>

QRgb color_birch =  qRgb(255, 0, 0); // light blue color
QRgb color_oak =    qRgb(0, 0, 255);   // brown color

tree::tree()
{

}

tree::tree(int id,
           std::vector<int> x_y_cor,
           char species,
           int dispersal_factor,
           int max_seed_production)
{
    this -> id;
    this -> x_y_cor;
    this -> species;
    this -> dispersal_factor;
    this -> max_seed_production;
    this -> color;
}

void tree::update_species_params() {
    if (species == 'b') {
        dispersal_factor = 20;
        max_seed_production = 50;
        color = color_birch;
    } else if (species == 'o') {
        dispersal_factor = 40;
        max_seed_production = 100;
        color = color_oak;
    }
}

void tree::set_burnt() {
    burnt = true;
}

