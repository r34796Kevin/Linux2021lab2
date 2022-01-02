#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
typedef struct __element {
    char *value;
    //struct __element *next;
    struct list_head list;
} list_ele_t;

static struct list_head *get_middle(struct list_head *list)
{
    struct list_head *fast = list->next, *slow;
    list_for_each (slow, list) {
        if (fast->next == list || fast->next->next == list)
            break;
        fast = fast->next->next;
    }
    return slow;
}

static void list_merge(struct list_head *lhs,
                       struct list_head *rhs,
                       struct list_head *head)
{
    INIT_LIST_HEAD(head);
    while (!list_empty(lhs) && !list_empty(rhs)) {
        char *lv = list_entry(lhs->next, list_ele_t, list)->value;
        char *rv = list_entry(rhs->next, list_ele_t, list)->value;
        struct list_head *tmp = strcmp(lv, rv) <= 0 ? lhs->next : rhs->next;
        list_del(tmp);
        list_add_tail(tmp, head);
    }
    list_splice_tail(list_empty(lhs) ? rhs : lhs, head);
}

void list_merge_sort(struct list_head *q)
{
    if (list_is_singular(q))
        return;

    struct list_head left;
    struct list_head sorted;
    INIT_LIST_HEAD(&left);
    list_cut_position(&left, q, get_middle(q));
    list_merge_sort(&left);
    list_merge_sort(q);
    list_merge(&left, q, &sorted);
    INIT_LIST_HEAD(q);
    list_splice_tail(&sorted, q);
}

static bool validate(struct list_head *q)
{
    struct list_head *node;
    list_for_each (node, q) {
        if (node->next == q)
            break;
        if (strcmp(list_entry(node, list_ele_t, list)->value,
                   list_entry(node->next, list_ele_t, list)->value) > 0)
            return false;
    }
    return true;
}

static struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (!q) return NULL;
    INIT_LIST_HEAD(q);
    return q;
}
static void q_show(struct list_head *q)
{
    struct list_head *cur=NULL;
    list_for_each(cur, q){
        char* val=list_entry(cur, list_ele_t, list)->value;
        printf("%s",val);
    }
}
static void q_free(struct list_head *q)
{
    struct list_head *current = q->next;
    while (current != q) {
        list_ele_t *tmp = list_entry(current, list_ele_t, list);
        current = current->next;
        free(tmp->value);
        free(tmp);
    }
    free(q);
}


bool q_insert_head(struct list_head *q, char *s)
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
    list_add_tail(&newh->list,q);

    return true;
}

int main(void)
{   
    FILE *fp = fopen("test.txt", "r");
    if (!fp) {
        perror("failed to open cities.txt");
        exit(EXIT_FAILURE);
    }

    struct list_head *q = q_new();
    char buf[256];
    while (fgets(buf, 256, fp)){
        //printf("buf is %s",buf);
        q_insert_head(q, buf);
    }
    fclose(fp);
    q_show(q);
    list_merge_sort(q);
    q_show(q);
    assert(validate(q));

    q_free(q);

    return 0;
}