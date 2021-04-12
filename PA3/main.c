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

#include "mem.h"

/* Global Variables */
int cycle = 0;
int hit_count[2] = {0};
int miss_count[2] = {0};

/* Variables for pa3_check */
int cache_level;
int memory_access;

int main() {
	char config_file[15];
	char input_file[20];
	char output_file[50];
	char policy[4];
	FILE *fd_config;
	FILE *fd_input;
	FILE *fd_output;
	char op;		//Read or Write of request
	unsigned int phy_addr;

	/* Get Input */
	strcpy(config_file, "cache.config");
	strcpy(input_file, "cache.input");
	strcpy(output_file, "cache.output");

	fd_config = fopen(config_file, "r");
	if(fd_config == NULL) {
		printf("There is no file such %s\n", config_file);
		exit(1);
	}

	fscanf(fd_config, "%d", &cache_level);
	fscanf(fd_config, "%d", &memory_access);

	fscanf(fd_config, "%d", &cache[0].size);
	fscanf(fd_config, "%d", &cache[0].num_entry);
	fscanf(fd_config, "%d", &cache[0].num_ways);

	fscanf(fd_config, "%s", policy);
	if (!strcmp(policy, "LRU"))
		cache[0].replace = LRU;
	else
		cache[0].replace = FIFO;

	fscanf(fd_config, "%s", policy);
	if (!strcmp(policy, "WB"))
		cache[0].write = WB;
	else
		cache[0].write = WT;

	fscanf(fd_config, "%d", &cache[0].access_cycle);

	cache[0].bytes_per_block = 4;
	cache[0].block_per_entry = cache[0].size / cache[0].num_ways / cache[0].num_entry / cache[0].bytes_per_block;

	if(cache_level == 2) {
		fscanf(fd_config, "%d", &cache[1].size);
		fscanf(fd_config, "%d", &cache[1].num_entry);
		fscanf(fd_config, "%d", &cache[1].num_ways);

		fscanf(fd_config, "%s", policy);
		if (!strcmp(policy, "LRU"))
			cache[1].replace = LRU;
		else
			cache[1].replace = FIFO;

		fscanf(fd_config, "%s", policy);
		if (!strcmp(policy, "WB"))
			cache[1].write = WB;
		else
			cache[1].write = WT;

		fscanf(fd_config, "%d", &cache[1].access_cycle);

		cache[1].bytes_per_block = 4;
		cache[1].block_per_entry = cache[1].size / cache[1].num_ways / cache[1].num_entry / cache[1].bytes_per_block;
	}

	initialize();

	/* Open Input & Output Files */
	fd_input = fopen(input_file, "r");
	if (fd_input == NULL) {
		printf("There is no file such %s\n", input_file);
		exit(1);
	}

	fd_output = fopen(output_file, "w+");
	/* Start Simulation */
	while(1) {
		fscanf(fd_input, "%c ", &op);
		
		switch(op) {
			case 'R':	//Read Operation
				fscanf(fd_input, "%x ", &phy_addr);
				read_op(phy_addr);
				break;
			case 'W':	//Write Operation
				fscanf(fd_input, "%x ", &phy_addr);
				write_op(phy_addr);
				break;
			default:	//HALT
				goto end;
		};
	}

end:
	fclose(fd_config);
	fclose(fd_input);
	/*	Print Results	  */
	print_result(fd_output);
	fclose(fd_output);

	destroy();

	return 0;
}

void initialize() {
	int n, i, j;
	
	printf("Initialize Your Cache...\n");
	ltime = 0;
	/* cache.way_cache[way #][entry #]*/
	for(n = 0; n < cache_level; n++) {
		cache[n].way_cache = (struct cache_entry **)malloc(sizeof(struct cache_entry *) * cache[n].num_ways);
		
		for (i = 0; i < cache[n].num_ways; i++)
			cache[n].way_cache[i] = (struct cache_entry *)malloc(sizeof(struct cache_entry) * cache[n].num_entry);
		
		for (i = 0; i < cache[n].num_ways; i++) {
			for (j = 0; j < cache[n].num_entry; j++) {				
				/* Initialize other variables in your cache_entry structure! */
				/* e.g. valid, dirty, tag, etc.                              */
				cache[n].way_cache[i][j].valid = 'I';
				cache[n].way_cache[i][j].tag = 0;
				cache[n].way_cache[i][j].dirty = 'C';
				cache[n].way_cache[i][j].time = 0;
				///////////////////////////////////////////////////////////////
			}
		}
	}

	printf("Finished to Initialize Your Cache!\n\n");
}

void destroy() {
	int n, i;

	printf("Destroy Your Cache...\n");
	for (n = 0; n < cache_level; n++) {
		for (i = 0; i < cache[n].num_ways; i++) {
			free(cache[n].way_cache[i]);
		}
		free(cache[n].way_cache);
	}
	printf("Finished to Destroy Your Cache!\n");
}

void get_bit_num()
{
	int i,temp;
	i = 0;
	temp = cache[0].num_entry;
	while(temp>1)
	{
		temp = temp/2;
		i++;
	}
	L1_index_bit = i;
	i = 0;
	temp = cache[0].block_per_entry;
	while(temp>1)
	{
		temp = temp/2;
		i++;
	}
	L1_block_bit = i;
	L1_byte_bit = 2;
	i = 0;
	temp = cache[0].block_per_entry;
	L1_tag_bit = 32 - L1_index_bit - L1_block_bit - L1_byte_bit;
	if(cache_level == 2)
	{
		while(temp>1)
		{
			temp = temp/2;
			i++;
		}
		L2_index_bit = i;
		i = 0;
		temp = cache[1].block_per_entry;
		while(temp>1)
		{
			temp = temp/2;
			i++;
		}
		L2_block_bit = i;
		L2_byte_bit = 2;
		L2_tag_bit = 32 - L2_index_bit - L2_block_bit - L2_byte_bit;
	}
}
void read_L1(unsigned int addr)
{
	ltime++;
	cycle += cache[0].access_cycle; // access L1
	//printf("addr %x + %d cycle => cycle %d\n",addr,cache[0].access_cycle,cycle);
	int i;
	unsigned int mask_bit = 0x1;
	unsigned int byte_offset = 0;
	unsigned int block_offset = 0;
	unsigned int index = 0;
	unsigned int tag = 0;
	int victim;
	unsigned int victim_age = MAX_TIME;
	for(i=0;i<L1_byte_bit;i++)
	{
		byte_offset = byte_offset | ( mask_bit & addr );
		mask_bit = mask_bit << 1;
	}
	for(i=0;i<L1_block_bit;i++)
	{
		block_offset = block_offset | ( mask_bit & addr );
		mask_bit = mask_bit << 1;
	}
	block_offset = block_offset >> (L1_byte_bit);
	for(i=0;i<L1_index_bit;i++)
	{
		index = index | ( mask_bit & addr );
		mask_bit = mask_bit << 1;
	}
	index = index >> (L1_block_bit + L1_byte_bit);
	for(i=0;i<L1_tag_bit;i++)
	{
		tag = tag | ( mask_bit & addr );
		mask_bit = mask_bit << 1;;
	}
	tag = tag >> (L1_index_bit + L1_block_bit + L1_byte_bit);
	/////////////////////////////////////////// 
	for(i=0;i<cache[0].num_ways;i++)
	{
		if(cache[0].way_cache[i][index].tag == tag)
		{
			if(cache[0].replace == LRU) cache[0].way_cache[i][index].time = ltime;
			hit_count[0]++;
			return;
		}
	}
	miss_count[0]++;
	if(cache_level == 2)
	{
		for(i=0;i<cache[0].num_ways;i++)
		{
			if(victim_age>cache[0].way_cache[i][index].time)
			{
				victim = i;
				victim_age = cache[0].way_cache[i][index].time;
			}
		}
		if(cache[0].way_cache[victim][index].dirty == 'D'){
			cycle += cycle += cache[1].access_cycle;
			//printf("addr %x + %d cycle => cycle %d\n",addr,cache[1].access_cycle,cycle);
			writeback_L2(tag);
		}
		read_L2(addr);
		cache[0].way_cache[victim][index].tag = tag;
		cache[0].way_cache[victim][index].valid = 'V';
		cache[0].way_cache[victim][index].dirty = 'C';
		cache[0].way_cache[victim][index].time = ltime;
		return;
	}
	else
	{
		for(i=0;i<cache[0].num_ways;i++)
		{
			if(victim_age>cache[0].way_cache[i][index].time)
			{
				victim = i;
				victim_age = cache[0].way_cache[i][index].time;
			}
		}
		if(cache[0].way_cache[victim][index].dirty == 'D')
		{
			cycle += memory_access;	
			//printf("addr %x + %d cycle => cycle %d\n",addr,memory_access,cycle);
		}
		cache[0].way_cache[victim][index].tag = tag;
		cache[0].way_cache[victim][index].valid = 'V';
		cache[0].way_cache[victim][index].dirty = 'C';
		cache[0].way_cache[victim][index].time = ltime;
		cycle += memory_access; // access Main Main memory
		//printf("addr %x + %d cycle => cycle %d\n",addr,memory_access,cycle);
		return;
	}
}
void read_L2(unsigned int addr)
{
	ltime++;
	cycle += cache[1].access_cycle; // access L2
	//printf("addr %x + %d cycle => cycle %d\n",addr,cache[1].access_cycle,cycle);
	int i;
	unsigned int mask_bit = 0x1;
	unsigned int byte_offset = 0;
	unsigned int block_offset = 0;
	unsigned int index = 0;
	unsigned int tag = 0;
	for(i=0;i<L2_byte_bit;i++)
	{
		byte_offset = byte_offset | ( mask_bit & addr );
		mask_bit = mask_bit << 1;
	}
	for(i=0;i<L2_block_bit;i++)
	{
		block_offset = block_offset | ( mask_bit & addr );
		mask_bit = mask_bit << 1;
	}
	block_offset = block_offset >> (L2_byte_bit);
	for(i=0;i<L2_index_bit;i++)
	{
		index = index | ( mask_bit & addr );
		mask_bit = mask_bit << 1;
	}
	index = index >> (L2_block_bit + L2_byte_bit);
	for(i=0;i<L2_tag_bit;i++)
	{
		tag = tag | ( mask_bit & addr );
		mask_bit = mask_bit << 1;;
	}
	tag = tag >> (L2_index_bit + L2_block_bit + L2_byte_bit);
	///////////////////////////////////////////
	for(i=0;i<cache[1].num_ways;i++)
	{
		if(cache[1].way_cache[i][index].tag == tag)
		{
			if(cache[1].replace == LRU) cache[1].way_cache[i][index].time = ltime;
			hit_count[1]++;
			return;
		}
	}
	miss_count[1]++;
	int victim;
	unsigned int victim_age = MAX_TIME;
	for(i=0;i<cache[1].num_ways;i++)
	{
		if(victim_age>cache[1].way_cache[i][index].time)
		{
			victim = i;
			victim_age = cache[1].way_cache[i][index].time;
		}
	}
	//printf("victim dirty : %c \n",cache[1].way_cache[victim][index].dirty);
	if(cache[1].way_cache[victim][index].dirty == 'D')
	{
		cycle += memory_access;	
		//printf("addr %x + %d cycle => cycle %d\n",addr,memory_access,cycle);
	}
	cache[1].way_cache[victim][index].tag = tag;
	cache[1].way_cache[victim][index].valid = 'V';
	cache[1].way_cache[victim][index].dirty = 'C';
	cache[1].way_cache[victim][index].time = ltime;
	cycle += memory_access; // access Main Main memory
	//printf("addr %x + %d cycle => cycle %d\n",addr,memory_access,cycle);
	return;
}
void writeback_L2(unsigned int tag){
	ltime++;
	int i,j;
	if(cache[1].write == WB)
	{
		for(j=0;j<cache[1].num_entry;j++) for(i=0;i<cache[1].num_ways;i++) if(cache[1].way_cache[i][j].tag == tag)
		{
			cache[1].way_cache[i][j].dirty = 'D';
			cycle = cycle + cache[1].access_cycle;
			cache[1].way_cache[i][j].time = ltime;
			cache[1].way_cache[i][j].valid = 'V';
		}
		//printf("addr %x + %d cycle => cycle %d\n",tag,cache[1].access_cycle,cycle);
	}
	else
	{
		for(j=0;j<cache[1].num_entry;j++) for(i=0;i<cache[1].num_ways;i++) if(cache[1].way_cache[i][j].tag == tag)
		{
			cache[1].way_cache[i][j].dirty = 'C';
			cycle = cycle + memory_access;
			cache[1].way_cache[i][j].time = ltime;
			cache[1].way_cache[i][j].valid = 'V';
		}
		//printf("addr %x + %d cycle => cycle %d\n",tag,cache[1].access_cycle + memory_access,cycle);
	}
}
void read_op(unsigned int addr) {
	//printf("\nread_op: \n");
	if(ltime == 0) get_bit_num();
	read_L1(addr);
}

void write_op(unsigned int addr) {
	//printf("\nwrite_op: \n");
	if(ltime == 0) get_bit_num();
	int i,j;
	read_L1(addr);
	unsigned int L1_tag = addr >> (32 - L1_tag_bit);
	unsigned int index_addr = addr >> (L1_byte_bit + L1_block_bit);
	unsigned int mask_bit = 0x1;
	unsigned int L1_index = 0x0;
	for(i=0;i<L1_index_bit;i++)
	{
		L1_index = L1_index | ( mask_bit & index_addr );
		mask_bit = mask_bit << 1;
	}
	if(cache[0].write == WB)
	{
		//printf("write back!\n");
		for(i=0;i<cache[0].num_ways;i++)
		{
			if(cache[0].way_cache[i][L1_index].tag == L1_tag)
			{
				cycle = cycle + cache[0].access_cycle;
				//printf("addr %x + %d cycle => cycle %d\n",addr,cache[0].access_cycle,cycle);
				cache[0].way_cache[i][L1_index].dirty = 'D';
				if(cache[0].replace == LRU) cache[0].way_cache[i][L1_index].time = ltime;
				break;
			}
		}
	}
	else
	{	if(cache[0].replace == LRU)
		{
			for(i=0;i<cache[0].num_ways;i++)
			{
				if(cache[0].way_cache[i][L1_index].tag == L1_tag)
				{
					cache[0].way_cache[i][L1_index].time = ltime;
					break;
				}
			}
		}
		if(cache_level == 2)
		{
			if(cache[1].write == WB)
			{
				unsigned int L2_tag = addr >> (32 - L2_tag_bit);
				//printf("L2_tag: %x\n",L2_tag);
				unsigned int index_addr = addr >> (L2_byte_bit + L2_block_bit);
				unsigned int mask_bit = 0x1;
				unsigned int L2_index = 0x0;
				for(i=0;i<L2_index_bit;i++)
				{
					L2_index = L2_index | ( mask_bit & index_addr );
					mask_bit = mask_bit << 1;
				}
				for(i=0;i<cache[1].num_ways;i++)
				{
					if(cache[1].way_cache[i][L2_index].tag == L2_tag)
					{
						//printf("way: %d index: %d\n",i,L2_index);
						//printf("tag: %x L2_tag: %x\n",cache[0].way_cache[i][L2_index].tag,L2_tag);
						cycle = cycle + cache[1].access_cycle;
						//printf("addr %x + %d cycle => cycle %d\n",addr,cache[1].access_cycle,cycle);
						cache[1].way_cache[i][L2_index].dirty = 'D';
						if(cache[1].replace == LRU) cache[1].way_cache[i][L2_index].time = ltime;
						break;
					}
				}
			}
			else cycle = cycle + memory_access;
		}
		else cycle = cycle + memory_access;
	}

	/*for (i = 0; i < cache[1].num_ways; i++) {
			for (j = 0; j < cache[1].num_entry; j++) {				
				printf("%d %d tag:%x dirty:%c ",i,j,cache[1].way_cache[i][j].tag,cache[1].way_cache[i][j].dirty);
			}
			printf("\n");
		}*/
}

void print_result(FILE *in) {
	fprintf(in, "Level 1 Cache\n");
	fprintf(in, "Hit Count : %d\n", hit_count[0]);
	fprintf(in, "Miss Count : %d\n", miss_count[0]);
	fprintf(in, "Hit Ratio : %.3f\n", (double)hit_count[0] / (double)(hit_count[0] + miss_count[0]));
	if(cache_level == 2) {
		fprintf(in, "\nLevel 2 Cache\n");
		fprintf(in, "Hit Count : %d\n", hit_count[1]);
		fprintf(in, "Miss Count : %d\n", miss_count[1]);
		fprintf(in, "Hit Ratio : %.3f\n", (double)hit_count[1] / (double)(hit_count[1] + miss_count[1]));
	} else {
		hit_count[1] = 0;
		miss_count[1] = 0;
	}
	fprintf(in, "\nTotal Hit Ratio : %.3f\n", (double)(hit_count[0] + hit_count[1]) / (double)(hit_count[0] + miss_count[0]));
	fprintf(in, "Total cycle: %d\n", cycle);
}
