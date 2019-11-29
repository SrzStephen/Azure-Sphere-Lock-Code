#pragma once
#include <stdbool.h>
#include <stdbool.h>
#include <pthread.h>
typedef struct CARD
{
	char card_id[11];
	bool error;
};
pthread_mutex_t read_lock;
struct CARD get_card_id(int ISU_Number);