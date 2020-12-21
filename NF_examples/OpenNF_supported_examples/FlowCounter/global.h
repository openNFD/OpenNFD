/*
 * global.h
 *
 *  Created on: Nov 8, 2017
 *      Author: evan
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <pthread.h>
#include "serialize.h"
#include "basic_classes.h"

extern pthread_mutex_t counterLock;
extern ser_tra_t *head_tra;

#endif /* GLOBAL_H_ */
