#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "lab2.c"

static bool validate(queue_t *q)
{
    struct list_head *node;
    list_for_each (node, &q->list) {
        if (node->next == &q->list)
            break;
        if (strcmp(list_entry(node, list_ele_t, list)->value,
                   list_entry(node->next, list_ele_t, list)->value) > 0)
            return false;
    }
    return true;
}

static queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q) return NULL;

    //q->head = q->tail = NULL;
    q->size = 0;
    INIT_LIST_HEAD(&q->list);
    return q;
}

static void q_free(queue_t *q)
{
    if (!q) return;

    list_ele_t *current = list_entry((&q->list)->next, list_ele_t, list);
    
    while (current) {
        list_ele_t *tmp = current;
        current = list_entry(current->list.next, list_ele_t, list);
        free(tmp->value);
        free(tmp);
    }
    free(q);
}
static void q_show(queue_t *q)
{
    if(!q) return;
    //list_ele_t *current = list_entry((&q->list)->next, list_ele_t, list);
    struct list_head *cur=(&q->list)->next;
    printf("size is %ld\n",q->size);
    for(size_t i=0;i<q->size;i++) {
        char* val=list_entry(cur, list_ele_t, list)->value;
        printf("%s",val);
        cur=cur->next;
    }
}

bool q_insert_head(queue_t *q, char *s)
{
    if (!q) return false;

    list_ele_t *newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;

    char *new_value = strdup(s);
    if (!new_value) {
        free(newh);
        return false;
    }

    newh->value = new_value;
    q->size++;
    list_add_tail(&newh->list, &q->list);

    return true;
}

int main(void)
{   
    FILE *fp = fopen("test.txt", "r");
    if (!fp) {
        perror("failed to open cities.txt");
        exit(EXIT_FAILURE);
    }

    queue_t *q = q_new();
    char buf[256];
    while (fgets(buf, 256, fp)){
        //printf("buf is %s",buf);
        q_insert_head(q, buf);
    }
    fclose(fp);
    //q_show(q);
    list_merge_sort(q);
    //q_show(q);
    assert(validate(q));

    q_free(q);

    return 0;
}