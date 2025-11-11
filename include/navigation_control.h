#ifndef NAVIGATION_CONTROL
#define NAVIGATION_CONTROL

void update_movement_state(uint8_t feature_state, uint8_t *robot_state, uint8_t *path, uint8_t following_path);

void update_path(uint8_t feature_state, uint8_t *robot_state, uint8_t *path);

void prune_map(uint8_t *node_list);

void add_u_turns(uint8_t *pruned_list);

void backtrack(uint8_t* pruned_list);

void generate_shortest_path(uint8_t* node_list_R1, uint8_t* node_list_R2, uint8_t* final_map);

#endif