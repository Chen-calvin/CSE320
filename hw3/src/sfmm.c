/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include "sfmm.h"
#include "hw3.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#define INT_MAX 2147483647

/**
 * You should store the head of your free list in this variable.
 * Doing so will make it accessible via the extern statement in sfmm.h
 * which will allow you to pass the address to sf_snapshot in a different file.
 */
sf_free_header* freelist_head = NULL;

static size_t allocatedBlocks = 0;
static size_t splinterBlocks = 0;
static size_t padding = 0;
static size_t splintering = 0;
static size_t coalesces = 0;
static size_t totalPayload = 0;
static size_t totalHeap = 0;

void *sf_malloc(size_t size) {
	size_t block_size;
	size_t padding_size;

	if(size == 0 || size > 16384){
		errno = EINVAL;
		return NULL;
	}

	if(freelist_head == NULL){
		freelist_head = sf_sbrk(4096);
		freelist_head->header.alloc = 0;
		//freelist_head->header.splinter = 0;
		freelist_head->header.block_size = 256;
		//freelist_head->header.requested_size = 4096;
		//freelist_head->header.splinter_size = 0;
		//freelist_head->header.padding_size = 0;
		freelist_head->next = NULL;
		freelist_head->prev = NULL;
		((sf_footer*)(freelist_head + freelist_head->header.block_size - 8))->alloc = 0;
		((sf_footer*)(freelist_head + freelist_head->header.block_size - 8))->splinter = 0;
		((sf_footer*)(freelist_head + freelist_head->header.block_size - 8))->block_size = 4096;
	}

	if(size < 16)
		block_size = 32;
	else
		block_size = 16 * ((size + 31)/16);
	padding_size = block_size - size - 16;
	padding += padding_size;

	sf_free_header* freeBlock = find_fit(block_size);
	if(freeBlock == NULL){
		sf_free_header* bp = freelist_head = (sf_free_header*)sf_sbrk;
		place(freelist_head, block_size, padding_size, size);
		return (char*)bp + SF_HEADER_SIZE;
	}
	else
		place(freeBlock, block_size, padding_size, size);
		return ((char*)freeBlock + SF_HEADER_SIZE);
}

void* find_fit(size_t asize){
	sf_free_header* currBlock = freelist_head;
	sf_free_header* bp = NULL;
	int sizeDiff = INT_MAX;
	do{
		if(currBlock->header.block_size > asize && currBlock->header.block_size - asize < sizeDiff){
			sizeDiff = currBlock->header.block_size - asize < sizeDiff;
			bp = currBlock;
		}
		currBlock = currBlock->next;
	}while(currBlock != NULL);

	return bp;
}

void place(sf_free_header* bp, size_t block_size, size_t padding, size_t reqSize){
	sf_free_header* old_bp = bp;
	size_t freeBlockSize = bp->header.block_size * 16;
	size_t splinter_size = 0;
	if(freeBlockSize - block_size < 32){
		splinter_size = freeBlockSize - block_size;
		block_size = freeBlockSize;
	}

	bp->header.alloc = 1;
	splintering += splinter_size;
	if(splinter_size){
		bp->header.splinter = 1;
		splinterBlocks++;
	}
	else
		bp->header.splinter = 0;
	bp->header.block_size = block_size / 16;
	bp->header.requested_size = reqSize;
	bp->header.splinter_size = splinter_size;
	bp->header.padding_size = padding;

	bp = ((sf_free_header*)((char*)bp + block_size - SF_FOOTER_SIZE));		//footer of alloc block
	((sf_footer*)bp)->alloc = 1;
	if(splinter_size)
		((sf_footer*)bp)->splinter = 1;
	else
		((sf_footer*)bp)->splinter = 0;
	((sf_footer*)bp)->block_size = block_size / 16;

	if(freeBlockSize > block_size){				//if block is split
		bp = (sf_free_header*)((char*)bp + SF_HEADER_SIZE);								//header of new free block
		bp->header.alloc = 0;
		bp->header.splinter = 0;
		bp->header.block_size = (freeBlockSize - block_size) / 16;

		sf_free_header* tmp = bp;
		tmp = tmp + 0;
		bp = (sf_free_header*)((char*)bp + (bp->header.block_size * 16) - SF_FOOTER_SIZE);		//footer of new free block
		((sf_footer*)bp)->block_size = (freeBlockSize - block_size) / 16;

		bp = (sf_free_header*)((char*)bp - (bp->header.block_size * 16) + SF_FOOTER_SIZE);		//header of new free block
		if(old_bp->prev == NULL && old_bp->next == NULL)
			freelist_head = bp;
		if(old_bp->prev != NULL){
			old_bp->prev->next = bp;
			bp->prev = old_bp->prev;
		}
		if(old_bp->next != NULL){
			old_bp->next->prev = bp;
			bp->next = old_bp->next;
		}
	}
	else{
		if(old_bp->prev == NULL && old_bp->next == NULL)
			freelist_head = NULL;
		else{
			sf_free_header* tmp = old_bp->next;
			if(old_bp->next != NULL)
				old_bp->next = old_bp->prev;
			if(old_bp->prev != NULL)
				old_bp->prev = tmp;
		}
	}
	allocatedBlocks++;
}

void *sf_realloc(void *ptr, size_t size) {
	return NULL;
}

void sf_free(void* ptr) {
	ptr = ((sf_free_header*)((char*)ptr - SF_HEADER_SIZE));
	sf_free_header* header = (sf_free_header*)ptr;
	(header)->header.alloc = 0;
	sf_footer* footer = (sf_footer*)((char*)header + (header->header.block_size * 16) - SF_FOOTER_SIZE);
	footer->alloc = 0;

	sf_free_header* currBlock = freelist_head;
	if(currBlock == NULL){
		freelist_head = header;
		return;
	}

	sf_free_header* spot = NULL;
	while(currBlock != NULL){
		if(header > currBlock)
			spot = currBlock;
		currBlock = currBlock->next;
	}
	if(spot == NULL){
		freelist_head->prev = header;
		header->next = freelist_head;
		freelist_head = header;
	}
	else if(spot->next == NULL){
		spot->next = header;
		header->prev = spot;
		}
		else{
			header->prev = spot;
			header->next = spot->next;
			spot->next = header;
			spot->next->prev = header;
		}
	coalesce(header);
}

void* coalesce(sf_free_header* bp){
	int prev_alloc = -1;
	int next_alloc = -1;
	size_t size = bp->header.block_size;

	if(bp->prev != NULL)
		prev_alloc = ((sf_footer*)((char*)bp - SF_HEADER_SIZE))->alloc;
	if(bp->next != NULL)
		next_alloc = ((sf_free_header*)((char*)bp + size))->header.alloc;

	if((prev_alloc == 1 && next_alloc == 1) || (prev_alloc == -1 && next_alloc == -1))
		return bp;
	else if((prev_alloc == 1 && next_alloc == 0) || (prev_alloc == -1 && next_alloc == 0)){
		size += bp->next->header.block_size;
		bp->header.block_size = size;
		((sf_footer*)((char*)bp->next + bp->next->header.block_size - SF_FOOTER_SIZE))->block_size = size;
		bp->next = bp->next->next;
		bp->next->next->prev = bp;
	}
		else if((prev_alloc == 0 && next_alloc == 1) || (prev_alloc == 0 && next_alloc == -1)){
			size += bp->prev->header.block_size;
			bp->prev->header.block_size = size;
			((sf_footer*)((char*)bp + bp->header.block_size - SF_FOOTER_SIZE))->block_size = size;
			bp->prev->next = bp->next;
			bp->next->prev = bp->prev;
			bp = bp->prev;
		}
		else{
			size += bp->next->header.block_size + bp->prev->header.block_size;
			bp->prev->header.block_size = size;
			((sf_footer*)((char*)bp->next + bp->next->header.block_size - 8))->block_size = size;
			bp->prev->next = bp->next->next;
			bp->next->prev = bp->prev->prev;
			bp = bp->prev;
		}

	return bp;
}

int sf_info(info* ptr) {
	ptr->allocatedBlocks = allocatedBlocks;
	ptr->splinterBlocks = splinterBlocks;
	ptr->padding = padding;
	ptr->splintering = splintering;
	ptr->coalesces = coalesces;
	ptr->peakMemoryUtilization = (double)totalPayload / totalHeap;
	return -1;
}
