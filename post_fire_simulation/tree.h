#ifndef TREE_H
#define TREE_H

#include <vector>        // package to use vectors
#include <QImage>

/**
 * @brief The tree class
 * Every tree is an object of this class, containing information about the tree species, location, and parameters
 */
class tree
{
public:
    tree();
    tree(int id,                        // id of the tree
         std::vector<int> x_y_cor,      // x and y coordinates of the tree
         char species,                  // species of tree, "b" for birch, "o" for oak
         int dispersal_factor,          // dispersal factor of the tree, depending on species
         int max_seed_production        // maximum number of seeds the tree disperses in each time step
         );
    int id;
    std::vector<int> x_y_cor;
    char species;
    int max_seed_production;
    int dispersal_factor;
    QRgb color;                         // color of tree species (green for birch or brown for oak)
    void update_species_params();       // update the parameters based on species assigned in setup procedure
    void set_burnt();                   // sets the tree as burnt (boolean) if forest fire is simulated, see tree.cpp
    bool burnt = false;                 // tree is not burnt by default

private:

};

#endif // TREE_H
