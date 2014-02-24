/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
/**
 * @file util.h
 * @author li_zhe(li_zhe@baidu.com)
 * @date 2013/04/03 10:03:42
 * @brief 
 **/

#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>
#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <string>

#define notice(...) printf(__VA_ARGS__)
#define warning(...) printf(__VA_ARGS__)
#define fatal(...) printf(__VA_ARGS__)

uint64_t get_timestamp_us()
{
	struct timeval tv;
	int ret = 0;
	if( 0 != (ret = gettimeofday(&tv, NULL))) {
		fatal("gettimeofday failed. [ret:%i] [errno:%i] [%m]", ret, errno);
	}   
	return tv.tv_sec * 1000000LU + tv.tv_usec;
}

void string_random_append(std::string *str, int size) {
    for (int i = 0; i < size; ++i) {
        char c = 'a' + rand()%26;
        str->append(&c, 1);
    }
}

void string_random_append(std::string *str, int min, int max) {
    int len = min;
    if (min != max) {
        len = min + rand() % (max - min);
    }
    string_random_append(str, len);
}

#endif //__UTIL_H__

