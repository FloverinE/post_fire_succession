/**
 * ABOUT THIS PROGRAM
 *
 * Author:      Florian Wilms
 * Matr.Nr.:    28725588
 *
 * M.FES.726 Ecological Modelling with C++
 * Lecturer: Sebastian Hanß
 * M.Sc. Forest Ecosystem Sciences - Ecosystem Analysis and Modelling
 * Georg-August-Universität Göttingen
 * 11.03.2024
 * Winter Semester 2023/2024
 *
 *
 * RESEARCH QUESTION:
 * "What is the amount of natural regeneration in a post-fire forest landscape and how does it change over time?"
 *
 * This model was created after my model concept presented in the winter semester 2022/23 in the course "M.FES.111 Introduction to Ecological Modelling"
 * Excerpt from the term paper introduction:
 *
 * Forest fires are a prevalent issue in Europe, with Mediterranean countries facing the main share of burnt area and
 * the amount of fires (Tedim et al., 2015). The fire regime in Germany has so far been comparably minor in
 * comparison to southern European forest fires of much larger scale (EFFIS, 2023). Within the period from 2001 to
 * 2020, two thirds of German forest fires occurred within coniferous stands, while ca. 6000ha, half of the total burnt
 * area, amounted to the federal state of Brandenburg (Gnilke & Sanders, 2021). Intent and negligence were the main
 * identified causes of fire events. 2022 has been the single most fire-intense year so far with over 4000ha burnt (EFFIS, 2023).
 *
 * Post-fire regeneration, as a form of stand establishment, is a costly measure in forestry operations, as it may take
 * decades until applied thinnings return the investment. Only at the end of rotation times, sale of merchantable wood
 * from mature trees results in a net positive balance. Therefore, minimizing initial investments on post-fire
 * restoration sites is a main objective for forest management.
 *
 * EFFIS. (2023). EFFIS Statistics Portal. Seasonal Trend for Germany. European Forest Fire Information System.
 * https://effis.jrc.ec.europa.eu/apps/effis.statistics/seasonaltrend
 * Gnilke, A., & Sanders, T. G. M. (2021). Forest fire history in Germany (2001-2020). Thünen Institute of Forest
 * Ecosystems.
 * Tedim, F., Xanthopoulos, G., & Leone, V. (2015). Forest Fires in Europe. In Wildfire Hazards, Risks and Disasters
 * (pp. 77–99). Elsevier. https://doi.org/10.1016/B978-0-12-410434-1.00005-1
 *
 * MODEL GUIDE
 *  This grid-based, spatially explicit model simulates the post-fire regeneration of a ficticious forest landscape.  The spatial extent covers 300 * 300 pixels,
 *  i.e. patches of 5 m * 5 m = 25 ha. The model runs in annual timesteps with the temporal horizon amounting to up to 100 years (maximum setting in the UI).
 *  Upon start, the user may select the number of years to be simulated, the number of trees per hectare and the mixture ratio of the two species, "oak" and "birch".
 *  Trees of either species are randomly distributed across the map. By default, a circular forest fire area of a 50 m radius is created in the center of the map.
 *  Standing deadwood can be removed after the fire according to the checkbox input. The ecological consequences of this action are that light availability
 *  will rise (decreased mortality and increased growth rate) while water availability will decrease due to less moisture being retained (increased mortality and decreased growth rate).
 *  Light and water availability are calculated for each patch in the landscape scaled according to the minimum distance to the next trees.
 *
 * The model simulates the following processes:
 *  seed dispersal:
 *  seeds of either birch and oak are spread at random abundancy and random direction around the mother tree. Each tree in the landscape will disperse seeds.
 *
 *  mortality:  birch and oak have the same mortality rate. The survival rate is the probability of 1 - mortality  to survive the first year after dispersal.
 *  growth:     the model assumes that trees grow at a constant rate, i.e. the number of trees in the landscape increases by the growth rate.
 *  seeds / saplings that neither die nor advance into the next growth stage will remain in the same stage.
 *
 *  There are 5 growth stages: seed, height class 1, height class 2, height class 3 and height class 4.
 *
 *  Mortality and growth probabilites are multiplied with the patch-specific light and water availability to create a "mortality and growth factor".
 *  This factor checked against a random float for each seed to determine if it survives or advances to the next growth stage.
 *
 *  Detailed function/procedure descriptions are provided in the respective function headers and line comments.
 *  There are two classes: trees and patches
 *  Each tree is and object of class tree with its own species, dispersal factor and maximum seed production according to its species.
 *  Each patch is an object of class patch with its own coordinates, seed and sapling count and light and water availability.
 *
 * Model outputs:
 *  The UI visualizes the landscape in a 2D grid with highlighted tree patches and patches with at least 1 seed or sapling.
 *
 *  Each species has two output graphs: seed and sapling population line series in the whole study area and the burnt area.
 *
 *  There is no file output for the model results, which would need to be implemented for model analysis.
 *
 *  Use of external information:
 *  The lecturer Sebastian Hanß was consulted for the model idea and scope.
 *
 *  The large language model ChatGPT was used for code completion and debugging
 *  OpenAI. (2023). ChatGPT (version 3.5) [Large language model]. https://chat.openai.com/chat
 *
 *  GitHub Copilot was used to for code and comment auto-completion.
 *
 *  Further code snippets, especially for mapping and charts were adapted from the sample models used in this course
 *  "M.FES.726 Ecological Modelling with C++" by Sebastian Hanß
 *  Max Luttermann's sample project was used as a reference for the implementation of the patch class
 */
