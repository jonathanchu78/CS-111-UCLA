//NAME: Jonathan Chu, Spenser Wong
//EMAIL: jonathanchu78@gmail.com, spenserwong1@gmail.com	
//ID: 004832220, 604766156

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <time.h>
#include <errno.h>
#include "ext2_fs.h"

#define SUPER_OFFSET 1024
#define BLOCK_SIZE	1024
#define EXT2_MIN_BLOCK_SIZE	1024

char* file_name;
int imgFD;
int num_groups;

//named with rando underscores to differentiate from trivial.csv in make clean
const char* super_file = "super_.csv";
const char* group_file = "group_.csv";
const char* freeblk_file = "freeblk_.csv";
const char* freeInode_file = "freeInode_.csv";
const char* allocInode_file = "allocInode_.csv";
const char* directory_file = "directory_.csv";
const char* indirectInode_file = "indirectInode_.csv";

uint64_t buf64;
uint32_t buf32;
uint16_t buf16;
uint8_t buf8_1;
uint8_t buf8_2;

struct ext2_super_block super;
struct ext2_inode inode;
struct ext2_group_desc* group;

void printCSV(const char* filename){
	int fd = open(filename, O_RDONLY);
    char buf[1024];
    int len;
    while((len = read(fd, buf, 1024)) > 0) {
        write(1, buf, len);
    }
    close(fd);
}

void timeformat(uint32_t timestamp, const int writefd){
	char timestr[32];
	time_t rawtime = timestamp;
	struct tm* time_info = gmtime(&rawtime);
	strftime(timestr, 32, "%m/%d/%y %H:%M:%S", time_info);
	dprintf(writefd,"%s,",timestr);
}

void superblockSummary(){
	int superFD = creat(super_file, S_IRWXU);

	pread(imgFD, &super, sizeof(struct ext2_super_block), SUPER_OFFSET);

	// SUPERBLOCK
	dprintf(superFD, "SUPERBLOCK,");

	// total number of blocks (decimal)
	dprintf(superFD, "%d,", super.s_blocks_count);

	// total number of i-nodes (decimal)
	dprintf(superFD, "%d,", super.s_inodes_count);

	// block size (in bytes, decimal)
	dprintf(superFD, "%d,", BLOCK_SIZE << super.s_log_block_size);

	// i-node size (in bytes, decimal)
	dprintf(superFD, "%d,", super.s_inode_size);

	// blocks per group (decimal)
	dprintf(superFD, "%d,", super.s_blocks_per_group);

	// i-nodes per group (decimal)
	dprintf(superFD, "%d,", super.s_inodes_per_group);

	// first non-reserved i-node (decimal)
	dprintf(superFD, "%d\n", super.s_first_ino);

	close(superFD);
}

void groupSummary(){
	int groupFD = creat(group_file, S_IRWXU);

	num_groups = super.s_blocks_count / super.s_blocks_per_group + 1;

	//calculate remainders, since last group containng leftovers
	//will not follow struct member vals
	unsigned int blocks_rem = super.s_blocks_count % super.s_blocks_per_group;
	unsigned int inodes_rem = super.s_inodes_count % super.s_inodes_per_group;

	group = malloc(num_groups * sizeof(struct ext2_group_desc));

	//iterate over all groups and produce csv output
	//but note that trivial.img only has one group, so only one line of output
	int i = 0;
	for (; i < num_groups; i++){
		// GROUP
		dprintf(groupFD, "GROUP,");

		// group number (decimal, starting from zero)
		dprintf(groupFD, "%d,", i);

		// total number of blocks in this group (decimal)
		// equal to s_blocks_per_group if not on last group or last group is full
		int num_blocks = (i != num_groups - 1 || blocks_rem == 0) ? super.s_blocks_per_group : blocks_rem;
		dprintf(groupFD, "%d,", num_blocks); 

		// total number of i-nodes in this group (decimal)
		int num_inodes = (i != num_groups - 1 || inodes_rem == 0) ? super.s_inodes_per_group : inodes_rem;
		dprintf(groupFD, "%d,", num_inodes); 
		

		//read in current group 
		pread(imgFD, &group[i], sizeof(struct ext2_group_desc), SUPER_OFFSET + BLOCK_SIZE + i*sizeof(struct ext2_group_desc));
		
		// number of free blocks (decimal)
		dprintf(groupFD, "%d,", group[i].bg_free_blocks_count); 
		
		// number of free i-nodes (decimal)
		dprintf(groupFD, "%d,", group[i].bg_free_inodes_count); 
		
		// block number of free block bitmap for this group (decimal)
		dprintf(groupFD, "%d,", group[i].bg_block_bitmap); 
		
		// block number of free i-node bitmap for this group (decimal)
		dprintf(groupFD, "%d,", group[i].bg_inode_bitmap);
		
		// block number of first block of i-nodes in this group (decimal)
		dprintf(groupFD, "%d\n", group[i].bg_inode_table);
	}

	close(groupFD);
}

void freeSummary(){
	int freeblkFD = creat(freeblk_file, S_IRWXU);
	int freeInodeFD = creat(freeInode_file, S_IRWXU);

	int i = 0;
	for (; i < num_groups; i++){ //iterate over groups
		int j = 0;
		for (; j < BLOCK_SIZE << super.s_log_block_size; j++){ //iterate over blocks
			pread(imgFD, &buf8_1, 1, group[i].bg_block_bitmap * //block bitmap
				(EXT2_MIN_BLOCK_SIZE << super.s_log_block_size) + j);

			pread(imgFD, &buf8_2, 1, group[i].bg_inode_bitmap * //inode bitmap
				(EXT2_MIN_BLOCK_SIZE << super.s_log_block_size) + j);

			int k = 0;
			for (; k < 8; k++){ //iterate over entries in bitmap
				if ((buf8_1 & (1 << k)) == 0){ //check if that entry is 1
					// BFREE
					dprintf(freeblkFD, "BFREE,");
					// number of the free block (decimal)
					int blknum = i*super.s_blocks_per_group + j*8 + k + 1;
					dprintf(freeblkFD, "%d\n", blknum);
				}
				if ((buf8_2 & (1 << k)) == 0){ //check if that entry is 1
					// IFREE
					dprintf(freeInodeFD, "IFREE,");
					// number of the free inode (decimal)
					int inodenum = i*super.s_inodes_per_group + j*8 + k + 1;
					dprintf(freeInodeFD, "%d\n", inodenum);
				}
			} //k loop
		} //j loop
	} //i loop

	close(freeblkFD);
	close(freeInodeFD);
}

void InodeDriver(){
	int allocInodeFD = creat(allocInode_file, S_IRWXU);
	int directoryFD = creat(directory_file, S_IRWXU);
	int indirectInodeFD = creat(indirectInode_file, S_IRWXU);
	unsigned int i,j,k;
	unsigned int groupnum = num_groups;
	for(i = 0; i < groupnum; i++){
		for(j = 0; j < super.s_inodes_count; j++){
			pread(imgFD,&inode,sizeof(struct ext2_inode),SUPER_OFFSET + 4 * BLOCK_SIZE + j*sizeof(struct ext2_inode));
			if(inode.i_mode != 0 && inode.i_links_count != 0){
				//INODE identifier and number
				dprintf(allocInodeFD, "INODE,%d,",j + 1);
				//File Type
				uint16_t mode = inode.i_mode;
				char typechar = '?';
				if(mode & 0x8000){
					typechar = 'f';
				}
				else if(mode & 0x4000){
					typechar = 'd';
				}
				else if(mode & 0xA000){
					typechar = 's';
				}
				dprintf(allocInodeFD,"%c,",typechar);
				//Mode
				dprintf(allocInodeFD,"%o,",mode & 0x0FFF);
				//Owner
				dprintf(allocInodeFD,"%d,",inode.i_uid);
				//Group
				dprintf(allocInodeFD,"%d,",inode.i_gid);
				//linkcount
				dprintf(allocInodeFD,"%d,",inode.i_links_count);
				//inode creation time
				timeformat(inode.i_ctime,allocInodeFD);
				//inode modification time
				timeformat(inode.i_mtime,allocInodeFD);
				//inode access time
				timeformat(inode.i_atime,allocInodeFD);
				//filesize
				dprintf(allocInodeFD,"%d,",inode.i_size);
				//number of blocks
				dprintf(allocInodeFD,"%d,",inode.i_blocks);
				//individual block addresses
				for(k = 0; k < EXT2_N_BLOCKS - 1;k++){
					dprintf(allocInodeFD,"%d,",inode.i_block[k]);
				}
				dprintf(allocInodeFD,"%d\n",inode.i_block[EXT2_N_BLOCKS - 1]);

				//Directory Formatting
				if(typechar == 'd'){
					struct ext2_dir_entry entry;
					k = 0;
					while (k < EXT2_NDIR_BLOCKS && inode.i_block[k] != 0){
						int itt = 0;
						while(itt < BLOCK_SIZE){
							pread(imgFD,&entry,sizeof(struct ext2_dir_entry),inode.i_block[k] * BLOCK_SIZE + itt);
							if(entry.inode != 0){
								//DIRENT identifier
								dprintf(directoryFD, "DIRENT,");
								//Parent inode number
								dprintf(directoryFD, "%d,", j+1);
								//Offset value
								dprintf(directoryFD, "%d,", itt);
								//Reference inode number
								dprintf(directoryFD, "%d,", entry.inode);
								//Entry Length
								dprintf(directoryFD, "%d,", entry.rec_len);
								//Name Length
								dprintf(directoryFD, "%d,", entry.name_len);
								//Name
								dprintf(directoryFD, "\'%s\'\n", entry.name);
							}
							itt += entry.rec_len;
						}
						k++;
					}
				}
				//indirect block printing
				if(typechar == 'd' || typechar == 'f'){
					int lv1[BLOCK_SIZE];
					int lv2[BLOCK_SIZE];
					int lv3[BLOCK_SIZE];

					//single indirection
					if(inode.i_block[12] > 0){
						pread(imgFD,lv1,BLOCK_SIZE,inode.i_block[12] * BLOCK_SIZE);
						k = 0;
						while (k < BLOCK_SIZE/sizeof(int)){
							if(lv1[k] != 0){
										//INDIRECT idenftifierber
										dprintf(indirectInodeFD, "INDIRECT,");
										//Parent inode number
										dprintf(indirectInodeFD, "%d,", j + 1);
										//Level of Indirection
										dprintf(indirectInodeFD, "1,");
										//Offset value
										dprintf(indirectInodeFD, "%d,", k + 12);
										//Scanned block number
										dprintf(indirectInodeFD, "%d,", inode.i_block[12]);
										//Reference block number
										dprintf(indirectInodeFD, "%d\n", lv1[k]);
							}
							k++;
						}
					}
					
					//double indirection
					if(inode.i_block[13] > 0){
						pread(imgFD,lv2,BLOCK_SIZE,inode.i_block[13] * BLOCK_SIZE);
						k = 0;
						while(k < BLOCK_SIZE/sizeof(int) ){
							if (lv2[k] != 0){
								//INDIRECT idenftifier
								dprintf(indirectInodeFD, "INDIRECT,");
								//Parent inode number
								dprintf(indirectInodeFD, "%d,", j + 1);
								//Level of Indirection
								dprintf(indirectInodeFD, "2,");
								//Offset value
								dprintf(indirectInodeFD, "%d,", 12 + (k+1)* 256);
								//Scanned block number
								dprintf(indirectInodeFD, "%d,", inode.i_block[13]);
								//Reference block number
								dprintf(indirectInodeFD, "%d\n", lv2[k]);
								unsigned int l = 0;
								pread(imgFD,lv1,BLOCK_SIZE,lv2[k] * BLOCK_SIZE);
								while(l < BLOCK_SIZE/sizeof(int)){
									if(lv1[l] != 0){
											//INDIRECT idenftifier
											dprintf(indirectInodeFD, "INDIRECT,");
											//Parent inode number
											dprintf(indirectInodeFD, "%d,", j + 1);
											//Level of Indirection
											dprintf(indirectInodeFD, "1,");
											//Offset value
											dprintf(indirectInodeFD, "%d,", 12 + (k+1)* 256 + l);
											//Scanned block number
											dprintf(indirectInodeFD, "%d,", lv2[k]);
											//Reference block number
											dprintf(indirectInodeFD, "%d\n", lv1[l]);

									}
									l++;
								}
							}
							k++;
						}
					}
						
					
					//triple indirection
					if(inode.i_block[14] > 0){
						pread(imgFD,lv3,BLOCK_SIZE,inode.i_block[14] * BLOCK_SIZE);
						k = 0;
						while(k < BLOCK_SIZE/sizeof(int) ){
							if(lv3[k] != 0){
								//INDIRECT idenftifier
								dprintf(indirectInodeFD, "INDIRECT,");
								//Parent inode number
								dprintf(indirectInodeFD, "%d,", j + 1);
								//Level of Indirection
								dprintf(indirectInodeFD, "3,");
								//Offset value
								dprintf(indirectInodeFD, "%ld,", (long)(12 + (k + 1) * 65536) + 256);
								//Scanned block number
								dprintf(indirectInodeFD, "%d,", inode.i_block[14]);
								//Reference block number
								dprintf(indirectInodeFD, "%d\n", lv3[k]);
							}
							unsigned int l = 0;
							pread(imgFD,lv2,BLOCK_SIZE,lv3[k] * BLOCK_SIZE);
							while(l < BLOCK_SIZE/sizeof(int) ){
								if(lv2[l] != 0){
									//INDIRECT idenftifier
									dprintf(indirectInodeFD, "INDIRECT,");
									//Parent inode number
									dprintf(indirectInodeFD, "%d,", j + 1);
									//Level of Indirection
									dprintf(indirectInodeFD, "2,");
									//Offset value
									dprintf(indirectInodeFD, "%ld,", (long)(12 + (k + 1) * 65536 + (l * 256)) +256);
									//Scanned block number
									dprintf(indirectInodeFD, "%d,", lv3[k]);
									//Reference block number
									dprintf(indirectInodeFD, "%d\n", lv2[l]);
								}
								unsigned int m = 0;
								pread(imgFD,lv1,BLOCK_SIZE, lv2[l] * BLOCK_SIZE);
								while(m < BLOCK_SIZE/sizeof(int) ){
									if(lv1[m] != 0){
										//INDIRECT idenftifierber
										dprintf(indirectInodeFD, "INDIRECT,");
										//Parent inode number
										dprintf(indirectInodeFD, "%d,", j + 1);
										//Level of Indirection
										dprintf(indirectInodeFD, "1,");
										//Offset value
										dprintf(indirectInodeFD, "%ld,", (long)(12 + (k + 1) * 65536 + (l * 256) + m) + 256);
										//Scanned block number
										dprintf(indirectInodeFD, "%d,", lv2[l]);
										//Reference block number
										dprintf(indirectInodeFD, "%d\n", lv1[m]);
									}
									m++;
								}
								l++;
							}
							k++;
						}
					}
				}
			}
		}
	}
}

int main(int argc, char** argv){
	if (argc != 2){
		fprintf(stderr, "must be called with exactly one argument: image name\n");
		exit(1);
	}
	else{
		int len = strlen(argv[1] + 1);
		file_name = malloc(len);
		file_name = argv[1];
	}

	imgFD = open(file_name, O_RDONLY);
	if(imgFD < 0){
		fprintf(stderr, "Error opening file: %s\n", strerror(errno));
		exit(1);
	}

	//not sure if al these functions will be necessary, but for now
	//theres one for each section of Part II:
	superblockSummary();
	groupSummary();
	freeSummary();
	InodeDriver();

	close(imgFD);

	printCSV(super_file);
	printCSV(group_file);
	printCSV(freeblk_file);
	printCSV(freeInode_file);
	printCSV(allocInode_file);
	printCSV(directory_file);
	printCSV(indirectInode_file);
	return 0;
}