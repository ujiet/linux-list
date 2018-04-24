#include <assert.h>
#include <stdlib.h>
#include "list.h"

#include "common.h"

static uint16_t values[256];

static struct list_head *getMiddle(struct list_head *head)
{
    if (head->next == head)
        return head;

    struct list_head *fast, *slow;

    slow = fast = head;
    while (fast->next != head && fast->next->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }
    return slow;
}

static void merge(struct list_head *head, struct list_head *newhead)
{
    struct listitem *item = NULL, *is = NULL;
    struct listitem *pivot;
    LIST_HEAD(temp);

    list_for_each_entry_safe (item, is, head, list) {
        if (newhead->next != newhead)
            pivot = list_first_entry(newhead, struct listitem, list);
        else
            break;

        if (cmpint(&item->i, &pivot->i) < 0)
            list_move_tail(&item->list, &temp);
        else {
            list_move_tail(&pivot->list, &temp);
            is = item;
            item = list_entry(item->list.prev, __typeof__(*item), list);
        }
    }
    list_splice(temp, head);
    list_splice(newhead, head);
}

static void list_msort(struct list_head *head)
{
    if (list_empty(head) || list_is_singular(head))
        return;

    static struct list_head *middle = getMiddle(head);
    LIST_HEAD(newhead);
    list_cut_position(newhead, head, middle);

    list_msort(head);
    list_msort(newhead);
    merge(head, newhead);
    return;
}

int main(void)
{
    struct list_head testlist;
    struct listitem *item, *is = NULL;
    size_t i;

    random_shuffle_array(values, (uint16_t) ARRAY_SIZE(values));

    INIT_LIST_HEAD(&testlist);

    assert(list_empty(&testlist));

    for (i = 0; i < ARRAY_SIZE(values); i++) {
        item = (struct listitem *) malloc(sizeof(*item));
        assert(item);
        item->i = values[i];
        list_add_tail(&item->list, &testlist);
    }

    assert(!list_empty(&testlist));

    qsort(values, ARRAY_SIZE(values), sizeof(values[0]), cmpint);
    list_msort(&testlist);

    i = 0;
    list_for_each_entry_safe (item, is, &testlist, list) {
        assert(item->i == values[i]);
        list_del(&item->list);
        free(item);
        i++;
    }

    assert(i == ARRAY_SIZE(values));
    assert(list_empty(&testlist));

    return 0;
}
