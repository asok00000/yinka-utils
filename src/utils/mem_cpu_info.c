/*
 *   Copyright (C) 2017  Steven Lee <geekerlw@gmail.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <assert.h>
#include "string.h"
#include "mem_cpu_info.h"



 
int process_phy_mem_get(const pid_t pid)
{
	char file[64] = {0};
   
	FILE *fd;
	char line_buff[256] = {0};
	sprintf(file,"/proc/%d/status", pid);
 
//	fprintf (stderr, "current pid:%d\n", pid);																								   
	fd = fopen (file, "r");

	char name[32];
	int vmrss;
	 
	for (int i=0; i<VMRSS_LINE-1; i++)
	{
		fgets (line_buff, sizeof(line_buff), fd);
	}
	fgets (line_buff, sizeof(line_buff), fd);
	sscanf (line_buff, "%s %d", name, &vmrss);
	//fprintf (stderr, "====%s��%d====\n", name, vmrss);
	 
	fclose(fd);	 
	return vmrss;
} 
 
int sys_total_mem_get()
{
	char* file = "/proc/meminfo";
   
	FILE *fd;
	char line_buff[256] = {0};																					 
	char name[32];
	int memtotal;
	
	fd = fopen (file, "r");
	 
	fgets (line_buff, sizeof(line_buff), fd);
	sscanf (line_buff, "%s %d", name, &memtotal);
	//fprintf (stderr, "====%s��%d====\n", name, memtotal);
	 
	fclose(fd);
	return memtotal;
}
 
void  process_mem_rate_get(pid_t pid, long *memvalue, float *memrate)
{
	int process_mem = process_phy_mem_get(pid);
	int total = sys_total_mem_get();
    float memrate_temp = (process_mem*1.0)/(total*1.0);
    
    *memvalue = process_mem;
    *memrate = memrate_temp;
	//fprintf(stderr,"====process mem rate:%.6f\n====", memrate);
}




const char* get_items(const char* buffer,int ie)
{
	assert(buffer);
	char* p = (char *)buffer;
	int len = strlen(buffer);
	int count = 0;
	
	if (1 == ie || ie < 1)
	{
		return p;
	} 
	for (int i=0; i<len; i++)
	{
		if (' ' == *p)
		 	{
			 	count++;
			 	if (count == ie-1)
			 	{
				 	p++;
				 	break;
			 	}
		 	}
		 	p++;
	 	}
	return p;
}
unsigned int process_cpu_time_get(const pid_t pid)
{
	char file[64] = {0};//�ļ���
	process_cpu_times pt;
   
	FILE *fd;		   //�����ļ�ָ��fd
	char line_buff[1024] = {0};  //��ȡ�еĻ�����
	sprintf(file,"/proc/%d/stat", pid);//�ļ��е�11�а�����
 
//	fprintf (stderr, "current pid:%d\n", pid);																								   
	fd = fopen (file, "r"); //��R���ķ�ʽ���ļ��ٸ���ָ��fd
	fgets (line_buff, sizeof(line_buff), fd); //��fd�ļ��ж�ȡ����Ϊbuff���ַ����ٴ浽��ʼ��ַΪbuff����ռ���
 
	sscanf(line_buff,"%u", &pt.pid);//ȡ�õ�һ��
	const char* q = get_items(line_buff, PROCESS_ITEM);//ȡ�ôӵ�14�ʼ����ʼָ��
	sscanf(q,"%u %u %u %u", &pt.utime, &pt.stime, &pt.cutime, &pt.cstime);//��ʽ����14,15,16,17��
 
	//fprintf (stderr, "====pid%u:%u %u %u %u====\n", pt.pid, pt.utime, pt.stime, pt.cutime, pt.cstime);
	fclose(fd);	 //�ر��ļ�fd
	return (pt.utime + pt.stime + pt.cutime + pt.cstime);
}
 
 
unsigned int sys_cpu_time_get()
{
	FILE *fd;		   //�����ļ�ָ��fd
	char buff[1024] = {0};  //����ֲ�����buff����Ϊchar���ʹ�СΪ1024
	sys_cpu_times st;
																											  
	fd = fopen ("/proc/stat", "r"); //��R���ķ�ʽ��stat�ļ��ٸ���ָ��fd
	fgets (buff, sizeof(buff), fd); //��fd�ļ��ж�ȡ����Ϊbuff���ַ����ٴ浽��ʼ��ַΪbuff����ռ���
	/*�����ǽ�buff���ַ������ݲ���format��ת��Ϊ���ݵĽ��������Ӧ�Ľṹ����� */
	char name[16];//��ʱ��������ַ���
	sscanf (buff, "%s %u %u %u %u", name, &st.user, &st.nice, &st.system, &st.idle);
	 
	//fprintf (stderr, "====%s:%u %u %u %u====\n", name, st.user, st.nice, st.system, st.idle);
	fclose(fd);	 //�ر��ļ�fd
	return (st.user + st.nice + st.system + st.idle);
}
 
 
float process_cpu_rate_get(pid_t pid)
{
	unsigned int totalcputime1, totalcputime2;
	unsigned int procputime1, procputime2;
	 
	totalcputime1 = sys_cpu_time_get();
	procputime1 = process_cpu_time_get(pid);
	 
	usleep(500000);//�ӳ�500����
	 
	totalcputime2 = sys_cpu_time_get();
	procputime2 = process_cpu_time_get(pid);
	 
	float cpurate = 100.0*(procputime2 - procputime1)/(totalcputime2 - totalcputime1);
	return cpurate;
}