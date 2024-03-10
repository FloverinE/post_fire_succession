#include "patch.h"
#include <vector>
#include <string>
#include <random>

patch::patch() {}

patch::patch(std::string patch_id,
             std::vector<int> x_y_cor,
             std::vector<int> N_seeds)
    : patch_id(patch_id), x_y_cor(x_y_cor), N_seeds(N_seeds) {}

/**
 * @brief patch::update_N_seeds
 * either add/remove birch or oak seeds to the patch
 * @param count number of seeds to add or subtract
 * @param species
 */
void patch::update_N_seeds(int count, char species) {
    if (species == 'b') {
        N_seeds[0] += count;
    } else if (species == 'o') {
        N_seeds[1] += count;
    }
}

/**
 * @brief patch::set_distance_to_tree
 * @param x coordinate of the tree
 * @param y coordinate of the tree
 * @return the euclidean distance to the tree
 */
float patch::set_distance_to_tree(int x, int y) {
    float dist_to_tree = sqrt(pow(x - x_y_cor[0], 2) + pow(y - x_y_cor[1], 2));
    return dist_to_tree;
}


/**
 * @brief patch::set_burnt
 * sets the patch as burnt (boolean) if forest fire is simulated
 */
void patch::set_burnt() {
    burnt = true;
}


/**
 * @brief patch::get_all_N_seeds_saplings
 * @return count of all seeds and saplings in the patch disregarding species and height class
 */
int patch::get_all_N_seeds_saplings(){
    int N_seeds_saplings = 0;
    for (int i = 0; i < 2; ++i) {
        N_seeds_saplings += N_seeds[i];
        N_seeds_saplings += N_height_class_1[i];
        N_seeds_saplings += N_height_class_2[i];
        N_seeds_saplings += N_height_class_3[i];
        N_seeds_saplings += N_height_class_4[i];
    }
    return N_seeds_saplings;
}
