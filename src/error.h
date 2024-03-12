#ifndef ERROR_H
#define ERROR_H

void check_malloc(void *ptr);
void err_overflow();
void err_entity_not_found();
void err_free_list_head();

#endif /* ERROR_H */
