#ifndef PATCH_H
#define PATCH_H

#include <vector>
#include <string>
#include <random>

/**
 * @brief The patch class
 * Every pixel representing 5 m * 5 m in the map is a patch of this class
 * containing information about the number of seeds and saplings of birch and oak
 */

class patch {
public:
    // Constructors
    patch();
    patch(std::string patch_id,                     // patch id formatted as "x_y"
          std::vector<int> x_y_cor,                 // x and y coordinates of the patch, also used to register the seed landing location
          std::vector<int> N_seeds);                // number of seeds

    // Member functions
    void update_N_seeds(int count, char species);   // function to add and subtract seeds to this patch
    int get_all_N_seeds_saplings();                 // returns the total number of seeds and saplings per patch for mapping
    float set_distance_to_tree(int x, int y);       // sets the distance to the nearest tree, described in patch.cpp
    void set_burnt();                               // sets the patch as burnt (boolean) if forest fire is simulated

    // Member variables
    std::string patch_id;
    std::vector<int> x_y_cor;

    // number of seeds and saplings per patch, single vectors for each class for simple readability, could be 2*5 vector as well
    std::vector<int> N_seeds = {0, 0};              // first element is birch, second is oak
    std::vector<int> N_height_class_1 = {0, 0};
    std::vector<int> N_height_class_2 = {0, 0};
    std::vector<int> N_height_class_3 = {0, 0};
    std::vector<int> N_height_class_4 = {0, 0};

    // distance independent
    float mortality_rate = 0.2;        // chance of the seed/sapling dying at a timestep. constant for all patches, modified into mortality factor according to light and water availability
    float growth_rate = 0.2;           // same for growth rate, that is the chance of advancing to the next height class
    bool burnt = false;                // set to true if the patch is burnt, used for forest fire simulation

    // distance dependent
    float distance_to_tree = 0.0f;     // distance to nearest tree, used to calculate light and water availability
    float light_availability = 0.0f;   // higher at further distance to trees
    float water_availability = 1.0f;   // default max = 1, low when deadwood is removed

private:

};

#endif // PATCH_H
