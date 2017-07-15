#include <stddef.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


struct header_t {
    size_t size;
    unsigned is_free;
    struct header_t *next;
} *head, *tail;


static struct header_t *get_free_block(size_t size)
{
    struct header_t *curr = head;
    while (curr) {
        if (curr->is_free && curr->size >= size)
            return curr;
        curr = curr->next;
    }
    return NULL;
}


pthread_mutex_t global_malloc_lock = PTHREAD_MUTEX_INITIALIZER;     // Q4: Mutex object should be initialized


void *malloc(size_t size)
{
    size_t total_size;
    void *block;
    struct header_t *header;

    if (!size)
        return NULL;

    pthread_mutex_lock(&global_malloc_lock);                        // Q4: Enter critical region to prevent race-condition in the multi-thread enviroment

    if ((header = get_free_block(size))) {
        header->is_free = 0;
        pthread_mutex_unlock(&global_malloc_lock);                  // Q4: Leave critical region before function return
        return (void *) ((char *) header + sizeof (struct header_t));   // Q4: Calculate actual memory address for client usages, positive shifted by header_t bytes
    }

    total_size = sizeof(struct header_t) + size;
    if ((block = sbrk(total_size)) == (void *) -1) {
        pthread_mutex_unlock(&global_malloc_lock);                  // Q4: Leave critical region before function return
        return NULL;
    }

    header = block;
    header->size = size;
    header->is_free = 0;
    header->next = NULL;

    if (tail) {                 // Q4: The new block should be appended
        tail->next = header;    //     behind the tail of the internal
        tail = header;          //     memory linked list (specified by
    } else                      //     variables `head` and `tail`)
        head = tail = header;   // Q4: Add the first block to the memory linked list

    pthread_mutex_unlock(&global_malloc_lock);                      // Q4: Leave critical region before function return
    return (void *) ((char *) header + sizeof (struct header_t));   // Q4: Calculate actual memory address for client usages, positive shifted by header_t bytes
}


void free(void *block)
{
    if (!block) {
#ifdef DETECT_ERROR
        fprintf(stderr, "Freeing NULL block\n");
#endif
        return;
    }

    // Calculate the header address from the block pointer, negative shifted by header_t bytes
    struct header_t *header = (struct header_t *) ((char *) block - sizeof (struct header_t));

    pthread_mutex_lock(&global_malloc_lock);                // Enter critical region to prevent race-condition in the multi-thread enviroment

    // Search the matched block from the memory linked list
    struct header_t *curr = head;
    while (curr) {
        if (curr == header) {
#ifdef DETECT_ERROR
            // Test that this block should not be free
            if (curr->is_free)
                fprintf(stderr, "Freeing a non-allocated block %p\n", block);
#endif
            curr->is_free = 1;
            break;
        }
        curr = curr->next;
    }

#ifdef DETECT_ERROR
    // Test whether this block is valid or not (should be in the memory linked list)
    if (!curr)
        fprintf(stderr, "Freeing an invalid block %p\n", block);
#endif

    pthread_mutex_unlock(&global_malloc_lock);              // Leave critical region before function return
}


//------------------------------------


struct allocated_block {
    void *block;
    size_t size;
};


#define TEST_CNT 1000



int main(void)
{
    static struct allocated_block allocated_blocks[TEST_CNT];
    static size_t allocated_block_cnt = 0;

    static void *historical_block_ptrs[TEST_CNT];
    static size_t historical_block_ptr_cnt = 0;

    int stat_malloc_cnt = 0;
    int stat_malloc_fail_cnt = 0;
    int stat_reuse_malloc_block_cnt = 0;
    int stat_free_cnt = 0;


    for (int i = 0; i < TEST_CNT; ++i) {
        // Malloc a block, with probability 55%
        if (rand() % 20 < 11) {
            size_t size = rand() % 1000 + 4;
            void *block = malloc(size);
            if (!block) {
                ++stat_malloc_fail_cnt;
                fprintf(stderr, "Failed to malloc memory, at step %d\n", i);
            } else {
                ++stat_malloc_cnt;
                struct allocated_block *alloc_blk = allocated_blocks + allocated_block_cnt++;
                alloc_blk->block = block;
                alloc_blk->size = size;

                size_t j;
                for (j = 0; j < historical_block_ptr_cnt; ++j) {
                    if (historical_block_ptrs[j] == block)
                        break;
                }

                if (j >= historical_block_ptr_cnt) {
                    historical_block_ptrs[historical_block_ptr_cnt++] = block;
#ifdef DEBUG
                    printf("MALLOC: %p [%zu]\n", block, size);
#endif
                } else {
                    ++stat_reuse_malloc_block_cnt;
#ifdef DEBUG
                    printf("MALLOC: %p [%zu] (reused)\n", block, size);
#endif
                }

                // Fill the allocated block with random data, for testing whether malloc/free data structure would be destroyed
                for (j = 0; j < size; ++j)
                    ((char *) block)[j] = (char) rand();
            }
        }

        // Free one of the previously allocated blocks, with probability 45%
        else {
            if (allocated_block_cnt > 0) {
                size_t index = rand() % allocated_block_cnt;
                struct allocated_block *alloc_blk = allocated_blocks + index;
                void *block = alloc_blk->block;
                free(block);
                ++stat_free_cnt;
#ifdef DEBUG
                printf("FREE:   %p [%zu]\n", block, alloc_blk->size);
#endif
                *alloc_blk = allocated_blocks[--allocated_block_cnt];
            }
        }
    }

    printf("Blocks mallocated: %d\n", stat_malloc_cnt);
    printf("Blocks failed to be mallocated: %d\n", stat_malloc_fail_cnt);
    printf("Blocks reused: %d\n", stat_reuse_malloc_block_cnt);
    printf("Blocks freed: %d\n", stat_free_cnt);

    // Free the remaining blocks
    for (size_t i = 0; i < allocated_block_cnt; ++i) {
        struct allocated_block *alloc_blk = allocated_blocks + i;
        void *block = alloc_blk->block;
        free(block);
#ifdef DEBUG
        printf("FREE:   %p [%zu]\n", block, alloc_blk->size);
#endif
    }

    return 0;
}
