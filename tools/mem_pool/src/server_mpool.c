



typedef struct mp_large_s {
	struct mp_large_s *next;
	void *alloc;
} mp_large_t;


typedef struct mp_small_s {

	unsigned char *last;
	unsigned char *end;
	mp_small_s *next;

} mp_small_t;


typedef struct mp_pool_s {

	int block_size;

	mp_large_t *large;
	mp_small_t *small;

} mp_pool_t;

// 
mp_pool_t * mp_pool_init(int size) {

	void *ptr = malloc(size + sizeof(mp_small_t) + sizeof(mp_pool_t));

	mp_pool_t *pool = (mp_pool_t*)ptr;
	pool->block_size = size;
	
	mp_small_t *small = pool + 1;
	pool->small = small;
	pool->large = NULL;

	small->last = (unsigned char *)ptr + sizeof(mp_pool_t) 
		+ sizeof(mp_small_t);
	small->end = small->last + size;
	small->next = NULL;
	
	return pool;

}


int mp_pool_dest(mp_pool_t *pool) {

	mp_free(pool);

	mp_small_t *s = pool->small->next;
	while (s) {
		mp_small_t *t = s->next;
		free(s);
		s = t;
	}
	free(pool);
}

// mp_alloc(>4k)
// mp_alloc(8/32/..., <4k);

void *mp_alloc_large(mp_pool_t *pool, size_t size) {

	// implement ...

}


void *mp_alloc_small(mp_pool_t *pool, size_t size) {

	// implement ...
	
}


void *mp_alloc(mp_pool_t *pool, size_t size) {

	if (size >= pool->block_size) {

		// alloc large
		return mp_alloc_large(pool, size);
	} else {

		// alloc small
		return mp_alloc_small(pool, size);
	}

}


void mp_free(mp_pool_t *pool) {

	mp_large_t *l = pool->large;

	for (; l; l = l->next) {
		if (l->alloc) {
			free(l->alloc);
			l->alloc = NULL;
		}
	}

}


#if 1

int main() {

	//mp_pool_t pool;

	//mp_pool_init(&pool, 0x1000);

}

#endif



