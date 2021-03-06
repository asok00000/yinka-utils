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

#ifndef YINKAD_H
#define YINKAD_H

#define MAX_DAMEON_PROGRAMS_NUMS		2
#define MAX_KEEPALIVE_FAILED_TIMES 		10
#define DEFAULT_DELAY   				1
#define YINKA_DAEMON_PORT  				12332 

#define DEFAULT_CONF_FILE_PATH "/etc/yinkad.conf"

char *prog_names[MAX_DAMEON_PROGRAMS_NUMS]={
						"yinka-terminal", 
						"ads"
};

typedef struct {
    unsigned short type;
    unsigned short len;
    char data[0];
}yinka_daemon_tlv_t;

typedef struct {
    int version;
    char *prog_name;
    float cpurate;
    float memrate;
    long uptime;
    int reboot_times;
    int keepalive_failed_times; 
}program_state_t;

typedef struct {
		char *cmdline;
		char *program_name;
		bool dameon_switch;
}program_t;

typedef struct{
	int delay;
	program_t prog_list[MAX_DAMEON_PROGRAMS_NUMS];
}daemon_config_t;

#endif
