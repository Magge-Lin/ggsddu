

#include <stdio.h>
#include <stdlib.h>


#define MEM_PAGE_SIZE		0x1000


typedef struct mempool_s {
	int block_size; // 
	int free_count;

	char *free_ptr;
	char *mem;
} mempool_t;


int memp_init(mempool_t *m, int block_size) {

	if (!m) return -2;

	m->block_size = block_size;
	m->free_count = MEM_PAGE_SIZE / block_size;

	m->free_ptr = (char*)malloc(MEM_PAGE_SIZE);
	if (!m->free_ptr) return -1;
	m->mem = m->free_ptr;

	int i = 0;
	char *ptr = m->free_ptr;
	for (i = 0;i < m->free_count;i ++) {

		*(char **)ptr = ptr + block_size;
		ptr += block_size;

	}
	*(char **)ptr = NULL;

	return 0;
}


void* memp_alloc(mempool_t *m) {

	if (!m || m->free_count == 0) return NULL;

	void *ptr = m->free_ptr;

	m->free_ptr = *(char **)ptr;
	m->free_count --;

	return ptr;
}

void *memp_free(mempool_t *m, void *ptr) {

	*(char**)ptr = m->free_ptr;
	m->free_ptr = (char *)ptr;
	m->free_count ++;
	

}


#if 1

int main() {

	mempool_t m;

	memp_init(&m, 32);

	void *p1 = memp_alloc(&m);
	printf("memp_alloc : %p\n", p1);

	void *p2 = memp_alloc(&m);
	printf("memp_alloc : %p\n", p2);

	void *p3 = memp_alloc(&m);
	printf("memp_alloc : %p\n", p3);

	void *p4 = memp_alloc(&m);
	printf("memp_alloc : %p\n", p4);
	
	memp_free(&m, p1);
	memp_free(&m, p3);

	void *p5 = memp_alloc(&m);
	printf("memp_alloc p5 : %p\n", p5);

	void *p6 = memp_alloc(&m);
	printf("memp_alloc p6 : %p\n", p6);

}

#endif


