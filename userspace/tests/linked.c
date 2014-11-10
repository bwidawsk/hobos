#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "cdefs.h"
#include "linked_list.h"

struct linked_list list;

struct tester {
	struct linked_list link;
	int val;
};

int gold[] = { 0xdefaca7e,
	0xfeedface,
	0xbeefb00b,
};

bool list_contains(int x)
{
	struct tester *t;
	int i;

	SLIST_FOR_EACH_ENTRY(t, &list, link) {
		if (t->val == x)
			return true;
	}

	return false;
}

bool local_list_contains(int x)
{
	int i;
	for (i = 0; i < sizeof(gold) / sizeof(int); i++)
		if (gold[i] == x)
			return true;

	return false;
}

void list_remove(int x)
{
	int i;
	for (i = 0; i < sizeof(gold) / sizeof(int); i++)
		if (gold[i] == x)
			gold[i] = 0;

	assert(0);
}

void test_single(void)
{
	struct linked_list *tmp;
	struct tester *t;
	SLIST_INIT(&list);
	int i;

	for (i = 0; i < sizeof(gold) / sizeof(int); i++) {
		struct tester *test_elem = malloc(sizeof(*test_elem));
		SLIST_INIT(&test_elem->link);
		test_elem->val = gold[i];
		SLIST_ADD(&test_elem->link, &list);
	}

	SLIST_FOR_EACH(tmp, &list) {
		t = CONTAINER_OF(tmp, struct tester, link);
		assert(local_list_contains(t->val));
	}

	SLIST_FOR_EACH_ENTRY(t, &list, link) {
		assert(local_list_contains(t->val));
	}

	t = CONTAINER_OF(list.next, struct tester, link);
	SLIST_REMOVE(list.next, &list);
	assert(!list_contains(gold[2]));

	SLIST_APPEND(&t->link, &list);
	assert(list_contains(gold[2]));

	assert(slist_last(&list) == &t->link);
}

int main(int argc, char *arv[])
{
	test_single();
	return 0;
}
