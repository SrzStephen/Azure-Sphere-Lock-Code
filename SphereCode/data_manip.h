#pragma once
#include "parson.h"
#include <stdbool.h>
#define FILE_SIZE 8000
struct CARD_STORED {
	char username[11];
	bool error;
};

bool get_storage_json(JSON_Array *jsondata);
struct CARD_STORED card_exists(char * cardid);
void init_lock();
bool dump_json_to_storage(char jsondata[]);