/**
 * TREE CLASS
 */

#include "tree.h"
#include <QImage>
#include <QColor>

// colors for mapping
QRgb color_birch =  qRgb(255, 0, 0); // red
QRgb color_oak =    qRgb(0, 0, 255); // bluer

// default constructor
tree::tree()
{

}

// constructor
tree::tree(int id,
           std::vector<int> x_y_cor,
           char species,
           int dispersal_factor,
           int max_seed_production)
{
//    this -> id;
//    this -> x_y_cor;
//    this -> species;
//    this -> dispersal_factor;
//    this -> max_seed_production;
//    this -> color;
}

/**
 * @brief tree::update_species_params
 * Function to update the tree object's parameters according to the assigned species
 */
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

/**
* @brief tree::set_burnt
* Function to set the tree as burnt if forest fire is simulated and the tree is located in the burnt area
*/
void tree::set_burnt() {
    burnt = true;
}

