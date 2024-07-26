/*
 @Author	: ouadimjamal@gmail.com
 @date		: December 2015

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  No representations are made about the suitability of this
software for any purpose.  It is provided "as is" without express or
implied warranty.
*/
#ifndef PMPARSER_C
#define PMPARSER_C

#include "pmparser.h"

/**
 * gobal variables
 */
//procmaps_struct* g_last_head=NULL;
//procmaps_struct* g_current=NULL;


procmaps_iterator* pmparser_parse(int pid){
	auto* maps_it = (procmaps_iterator*)malloc(sizeof(procmaps_iterator));
	char maps_path[500];
	if(pid>=0 ){
		sprintf(maps_path,"/proc/%d/maps",pid);
	}else{
		sprintf(maps_path,"/proc/self/maps");
	}
	FILE* file=fopen(maps_path,"r");
	if(!file){
		fprintf(stderr,"pmparser : cannot open the memory maps, %s\n",strerror(errno));
        // printf("pmparser_parse cannot open the memory maps, %s ",strerror(errno));
		return NULL;
	}
	int ind=0;char buf[PROCMAPS_LINE_MAX_LENGTH];
	int c;
	procmaps_struct* list_maps=NULL;
	procmaps_struct* tmp;
	procmaps_struct* current_node=list_maps;
	char addr1[20],addr2[20], perm[8], offset[20], dev[10],inode[30],pathname[PATH_MAX];
    while (fgets(buf, PROCMAPS_LINE_MAX_LENGTH, file) != NULL) {
		//allocate a node
		tmp=(procmaps_struct*)malloc(sizeof(procmaps_struct));
		//fill the node
		_pmparser_split_line(buf,addr1,addr2,perm,offset, dev,inode,pathname);
		//printf("#%s",buf);
		//printf("%s-%s %s %s %s %s\t%s\n",addr1,addr2,perm,offset,dev,inode,pathname);
		//addr_start & addr_end
		unsigned long l_addr_start;
		sscanf(addr1,"%lx",(long unsigned *)&tmp->addr_start );
		sscanf(addr2,"%lx",(long unsigned *)&tmp->addr_end );
		//size
		tmp->length=(unsigned long)((unsigned long)tmp->addr_end-(unsigned long)tmp->addr_start);
		//perm
		strcpy(tmp->perm,perm);
		tmp->is_r=(perm[0]=='r');
		tmp->is_w=(perm[1]=='w');
		tmp->is_x=(perm[2]=='x');
		tmp->is_p=(perm[3]=='p');

		//offset
		sscanf(offset,"%lx",&tmp->offset );
		//device
		strcpy(tmp->dev,dev);
		//inode
		tmp->inode=atoi(inode);
		//pathname
		strcpy(tmp->pathname,pathname);
		tmp->next=NULL;
		//attach the node
		if(ind==0){
			list_maps=tmp;
			list_maps->next=NULL;
			current_node=list_maps;
		}
		current_node->next=tmp;
		current_node=tmp;
		ind++;
		//printf("%s",buf);
	}

	//close file
	fclose(file);


	//g_last_head=list_maps;
	maps_it->head = list_maps;
	maps_it->current =  list_maps;
	return maps_it;
}


procmaps_struct* pmparser_next(procmaps_iterator* p_procmaps_it){
	if(p_procmaps_it->current == NULL)
		return NULL;
	procmaps_struct* p_current = p_procmaps_it->current;
	p_procmaps_it->current = p_procmaps_it->current->next;
	return p_current;
	/*
	if(g_current==NULL){
		g_current=g_last_head;
	}else
		g_current=g_current->next;

	return g_current;
	*/
}



void pmparser_free(procmaps_iterator* p_procmaps_it){
	procmaps_struct* maps_list = p_procmaps_it->head;
	if(maps_list==NULL) return ;
	procmaps_struct* act=maps_list;
	procmaps_struct* nxt=act->next;
	while(act!=NULL){
		free(act);
		act=nxt;
		if(nxt!=NULL)
			nxt=nxt->next;
	}
	free(p_procmaps_it);
}


void _pmparser_split_line(
		char*buf,char*addr1,char*addr2,
		char*perm,char* offset,char* device,char*inode,
		char* pathname){
	//
	int orig=0;
	int i=0;
	//addr1
	while(buf[i]!='-'){
		addr1[i-orig]=buf[i];
		i++;
	}
	addr1[i]='\0';
	i++;
	//addr2
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' '){
		addr2[i-orig]=buf[i];
		i++;
	}
	addr2[i-orig]='\0';

	//perm
	while(buf[i]=='\t' || buf[i]==' ')
		i++;
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' '){
		perm[i-orig]=buf[i];
		i++;
	}
	perm[i-orig]='\0';
	//offset
	while(buf[i]=='\t' || buf[i]==' ')
		i++;
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' '){
		offset[i-orig]=buf[i];
		i++;
	}
	offset[i-orig]='\0';
	//dev
	while(buf[i]=='\t' || buf[i]==' ')
		i++;
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' '){
		device[i-orig]=buf[i];
		i++;
	}
	device[i-orig]='\0';
	//inode
	while(buf[i]=='\t' || buf[i]==' ')
		i++;
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' '){
		inode[i-orig]=buf[i];
		i++;
	}
	inode[i-orig]='\0';
	//pathname
	pathname[0]='\0';
	while(buf[i]=='\t' || buf[i]==' ')
		i++;
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' ' && buf[i]!='\n'){
		pathname[i-orig]=buf[i];
		i++;
	}
	pathname[i-orig]='\0';

}

void pmparser_print(procmaps_struct* map, int order){

	procmaps_struct* tmp=map;
	int id=0;
	if(order<0) order=-1;
	while(tmp!=NULL){
		//(unsigned long) tmp->addr_start;
		if(order==id || order==-1){
			printf("Backed by:\t%s\n",strlen(tmp->pathname)==0?"[anonym*]":tmp->pathname);
			printf("Range:\t\t%p-%p\n",tmp->addr_start,tmp->addr_end);
			printf("Length:\t\t%ld\n",tmp->length);
			printf("Offset:\t\t%ld\n",tmp->offset);
			printf("Permissions:\t%s\n",tmp->perm);
			printf("Inode:\t\t%d\n",tmp->inode);
			printf("Device:\t\t%s\n",tmp->dev);
		}
		if(order!=-1 && id>order)
			tmp=NULL;
		else if(order==-1){
			printf("#################################\n");
			tmp=tmp->next;
		}else tmp=tmp->next;

		id++;
	}
}

uintptr_t get_memory_start_address(pid_t pid,char* pathname) {
// 解析当前进程的内存映射
procmaps_iterator* maps = pmparser_parse(pid);
if (maps == nullptr) return 0;

procmaps_struct* maps_tmp;
uintptr_t addr_start = 0;

// 遍历所有内存映射区域
while ((maps_tmp = pmparser_next(maps)) != nullptr) {
// 检查路径名是否匹配
if (strstr(maps_tmp->pathname, pathname)) {
addr_start = (uintptr_t)maps_tmp->addr_start;
break; // 找到匹配的路径名，跳出循环
}
}

// 释放内存映射
pmparser_free(maps);

return addr_start;
}

procmaps_iterator* pmparser_find_by_pathname(int pid, const char* pathname) {
    // 创建一个新的迭代器用于存储筛选后的内存映射区域
    procmaps_iterator* filtered_it = (procmaps_iterator*)malloc(sizeof(procmaps_iterator));
    if (!filtered_it) {
        fprintf(stderr, "无法为迭代器分配内存\n");
        return NULL;
    }
    
    // 初始化迭代器
    filtered_it->head = NULL;
    filtered_it->current = NULL;
    
    // 获取指定 PID 的所有内存映射
    procmaps_iterator* all_maps_it = pmparser_parse(pid);
    if (!all_maps_it) {
        free(filtered_it);
        return NULL;
    }

    procmaps_struct* current_map;
    procmaps_struct* last_filtered_map = NULL;
    
    while ((current_map = pmparser_next(all_maps_it)) != NULL) {
        if (strstr(current_map->pathname, pathname)) {
            // 为筛选后的映射区域分配新节点
            procmaps_struct* new_map = (procmaps_struct*)malloc(sizeof(procmaps_struct));
            if (!new_map) {
                fprintf(stderr, "无法为新映射分配内存\n");
                pmparser_free(all_maps_it);
                pmparser_free(filtered_it); // 释放部分填充的迭代器
                return NULL;
            }
            
            // 复制映射区域数据到新节点
            memcpy(new_map, current_map, sizeof(procmaps_struct));
            new_map->next = NULL;
            
            // 将新节点附加到筛选后的列表
            if (filtered_it->head == NULL) {
                filtered_it->head = new_map;
            } else {
                last_filtered_map->next = new_map;
            }
            last_filtered_map = new_map;
        }
    }

    // 设置当前指针为筛选后列表的头部
    filtered_it->current = filtered_it->head;
    
    // 释放原始的映射迭代器
    pmparser_free(all_maps_it);

    return filtered_it;
}
#endif  // PMPARSER_C
