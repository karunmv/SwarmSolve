#ifndef NAVIGATION_CONTROL
#define NAVIGATION_CONTROL

/**
 * @brief Updates the movement state of the robot based on the navigation algorithm and the current feature
 * @param feature_state The feature state of the robot
 * @param movement_state Pointer to the movement state variable, gets updated in place
 * @param path Pointer to the array of turns the robot has made at each intersection
 * @param following_path Flag that states if the robot is following the path, or still exploring the maze
 */
void update_movement_state(uint8_t feature_state, uint8_t *movement_state, uint8_t *path, uint8_t following_path);

/**
 * @brief Appends a new movement_state to the path array
 * @param feature_state The feature state of the robot
 * @param movement_state The movement state of the robot to be appended to path
 * @param path Array of current path
 */
void update_path(uint8_t feature_state, uint8_t *movement_state, uint8_t *path);

/**
 * @brief Prunes the map of all extranious paths
 * @param path Array of path to prune
 */
void prune_map(uint8_t *path);

/**
 * @brief Adds u_turn movement state to start and end of the 
 * @param path Array of path to add U turns to
 */
void add_u_turns(uint8_t *path);

/**
 * @brief Inverts the path such that it gives the path from the robot's current position back to the start, rather than from the start to the current position
 * @param path Array of path to invert
 */
void backtrack(uint8_t *path);

/**
 * @brief Generates the shortest path from the robot's local position to the end of the maze
 * @param local_path Array containing the path the local robot has explored already
 * @param solved_path Array containing the path the other robot found from the start to the goal
 */
void generate_shortest_path(uint8_t *local_path, uint8_t *solved_path, uint8_t *final_path);

#endif