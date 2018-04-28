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
    struct listitem *item1 = NULL, *item2 = NULL;
    LIST_HEAD(temp);

    while (!list_empty(head) && !list_empty(newhead)) {
        item1 = list_first_entry(head, struct listitem, list);
        item2 = list_first_entry(newhead, struct listitem, list);

        if (cmpint(&item1->i, &item2->i) < 0)
            list_move_tail(&item1->list, &temp);
        else
            list_move_tail(&item2->list, &temp);
    }
    list_splice_tail_init(head, &temp);
    list_splice_tail_init(newhead, &temp);
    list_splice_init(&temp, head);
}

static void list_msort(struct list_head *head)
{
    if (list_empty(head) || list_is_singular(head))
        return;

    struct list_head *middle;
    LIST_HEAD(newhead);

    middle = getMiddle(head);
    list_cut_position(&newhead, head, middle);

    list_msort(head);
    list_msort(&newhead);
    merge(head, &newhead);

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
