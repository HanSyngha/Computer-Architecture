/*
 * SWE3005-43 Introduction to Computer Architectures (Fall 2019)
 *
 * Instructor: Prof. Jinkyu Jeong (jinkyu@skku.edu)
 *
 * TA: Minwoo Ahn (minwoo.ahn@csi.skku.edu), Sunghwan Kim (sunghwan.kim@csi.skku.edu)
 * Office: Semiconductor Buildnig #400509
 *
 * Author: Minwoo Ahn
 * Description: CPU Cache Simulator
 *
 * Copyright (c) 2019 Sungkyunkwan Univ. CSI (Computer Systems and Intelligence Lab)
 */
#define MAX_TIME 0xFFFFFFFF 
#ifndef __mem__
#define __mem__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum replace_policy {
	LRU,	//Least recently used
	FIFO		//First-in-First-out
};

enum write_policy {
	WB,		//Write-back
	WT		//Write-through
};

struct cache_entry {
	char valid; // V or I
	char dirty; // D or C
	unsigned int tag;
	unsigned int time;
};

struct cache {
	struct cache_entry **way_cache;
	int size;
	int num_entry;
	int num_ways;
	int block_per_entry;		// # of blocks / entry
	int bytes_per_block;		// # of bytes / block
	enum replace_policy replace;		// LRU or FIFO
	enum write_policy write;		// WB or WT
	int access_cycle;
};

struct cache cache[2];

void read_op(unsigned int);
void write_op(unsigned int);

/* Flush data in cache to disk*/
void print_result(FILE *);

/* Initialization before start simulation. */
void initialize(void);
/* Destroy after simulation */
void destroy(void);
void get_bit_num(void);
void read_L1(unsigned int addr);
void read_L2(unsigned int addr);
void writeback_L2(unsigned int tag);
#endif

int ltime = 0;
int L1_index_bit = 0;
int L1_block_bit = 0;
int L1_byte_bit = 0;
int L1_tag_bit = 0;
int L2_index_bit = 0;
int L2_block_bit = 0;
int L2_byte_bit = 0;
int L2_tag_bit = 0;
