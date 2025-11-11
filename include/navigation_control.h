#ifndef NAVIGATION_CONTROL
#define NAVIGATION_CONTROL

void update_movement_state(uint8_t feature_state, uint8_t *robot_state, uint8_t *path, uint8_t following_path);

void update_path(uint8_t feature_state, uint8_t *robot_state, uint8_t *path);

/**
 * @brief
 * @param 
 */
void occupancy_grid(uint8_t feature_state, uint8_t *robot_state, uint8_t *node_list);

uint8_t* prune_map(uint8_t *node_list);

#endif