#ifndef ERROR_H
#define ERROR_H

void check_malloc(void *ptr);
void err_overflow();
void err_entity_not_found();
void err_free_list_head();
void err_component_exists(const char* component_str);
void err_free_missing_component(const char* component_str);
void err_no_sprite_to_draw(int row, int column);

#endif /* ERROR_H */
