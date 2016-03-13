/**
 * @file     process_msg.c
 * @Author   Andrey Dmitriev (dmandry92@gmail.com)
 * @date     September, 2015
 * @brief    Message processing implementation
 */

#include "process_msg.h"

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "pa1.h"

extern FILE *eventlog;

int started[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int done[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int8_t started_num = 0;
int8_t done_num = 0;

int started_len = 0;
int done_len = 0;

void process_msg_started(Message *msg)
{
	int id, pid, ppid;
	msg->s_payload[msg->s_header.s_payload_len] = '\0';
	sscanf(msg->s_payload, log_started_fmt, &id, &pid, &ppid);
	if (started[id] == 0) started_num++;
	started[id]++;
}

void process_msg_done(Message *msg)
{
	int id;
	msg->s_payload[msg->s_header.s_payload_len] = '\0';
	sscanf(msg->s_payload, log_done_fmt, &id);
	if (done[id] == 0) done_num++;
	done[id]++;
}

void process_msg(Message *msg)
{
	switch (msg->s_header.s_type)
	{
		case STARTED:
		    process_msg_started(msg);
		    break;
		case DONE:
		    process_msg_done(msg);
		    break;
		default:
			fclose(eventlog);
			free(msg);
		    perror("Unknown msg type");
		    exit(EXIT_FAILURE);
	}
}

int payload_size(int16_t type)
{
	switch (type)
	{
		case STARTED:
		    return started_len;
		    break;
		case DONE:
		    return done_len;
		    break;
		default:
			return MAX_PAYLOAD_LEN;
	}
	fclose(eventlog);
	perror("WTF");
	exit(EXIT_FAILURE);
}

Message *create_msg(int16_t type, char *payload)
{
	uint16_t i;
	Message *msg;
	uint16_t payload_len;
	payload_len = payload_size(type);
	if (payload == NULL)
	{
		fclose(eventlog);
		perror("create_msg");
		exit(EXIT_FAILURE);
	}
	msg = malloc(sizeof(MessageHeader)+payload_len);
	if (msg == NULL)
	{
		fclose(eventlog);
		free(payload);
		perror("create_msg");
		exit(EXIT_FAILURE);
	}
	msg->s_header.s_magic = MESSAGE_MAGIC;
	msg->s_header.s_type = type;
	msg->s_header.s_payload_len = payload_len;
	msg->s_header.s_local_time = time(NULL);
	for (i=0; i<payload_len; i++)
	    msg->s_payload[i] = payload[i];
	return msg;
}

const char *log_fmt_type(int16_t type)
{
	switch (type)
	{
		case STARTED:
		    return log_started_fmt;
		    break;
		case DONE:
		    return log_done_fmt;
		    break;
		default:
			fclose(eventlog);
		    perror("Unknown msg type");
		    exit(EXIT_FAILURE);
	}
	fclose(eventlog);
	perror("WTF");
	exit(EXIT_FAILURE);
}

char *create_payload(int16_t type, local_id id)
{
	char *payload = malloc(payload_size(type));
	if (payload == NULL)
	{
	    fclose(eventlog);
	    perror("create_payload");
	    exit(EXIT_FAILURE);
	}
	sprintf(payload, log_fmt_type(type), id, getpid(), getppid());
	return payload;
}

void count_sent_num(local_id id, int16_t type)
{
	switch (type)
	{
		case STARTED:
		    started[id]++;
			started_num++;
		    break;
		case DONE:
			done[id]++;
			done_num++;
		    break;
		default:
			fclose(eventlog);
		    perror("Unknown msg type");
		    exit(EXIT_FAILURE);
	}
}

int8_t *get_rcvd_num(int16_t type)
{
	switch (type)
	{
		case STARTED:
		    return &started_num;
		    break;
		case DONE:
		    return &done_num;
		    break;
		default:
			fclose(eventlog);
		    perror("Unknown msg type");
		    exit(EXIT_FAILURE);
	}
	fclose(eventlog);
	perror("WTF");
	exit(EXIT_FAILURE);
}

int *get_rcvd(int16_t type)
{
	switch (type)
	{
		case STARTED:
		    return started;
		    break;
		case DONE:
		    return done;
		    break;
		default:
			fclose(eventlog);
		    perror("Unknown msg type");
		    exit(EXIT_FAILURE);
	}
	perror("WTF");
	exit(EXIT_FAILURE);
}
