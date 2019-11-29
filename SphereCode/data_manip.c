#include "data_manip.h"
#include <applibs/storage.h>
#include <applibs/log.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "parson.h"
pthread_mutex_t lock;
JSON_Array *jarray;
JSON_Array *json_obj;
JSON_Object * jo;
char stringbuffer[FILE_SIZE];

void init_lock()
{
	pthread_mutex_init(&lock, NULL);
}

bool dump_json_to_storage(char jsondata[])
{
	pthread_mutex_lock(&lock);
	int fd = -1;
	fd = Storage_OpenMutableFile();
	if (fd < 0)
	{
		Log_Debug("invalid file handler \n");
		pthread_mutex_unlock(&lock);
		return false;
	}
	ssize_t written = write(fd, jsondata, strlen(jsondata));
	if (written != strlen(jsondata))
	{
		Log_Debug("only write %i bytes, expected %i \n", written, sizeof(jsondata));
		pthread_mutex_unlock(&lock);
		close(fd);
		return false;
	}
	Log_Debug("wrote %i bytes \n", written);
	close(fd);
	pthread_mutex_unlock(&lock);
	return true;
}

struct CARD_STORED card_exists(char * cardid)
{
	struct CARD_STORED card_data;
	card_data.error = true;
	pthread_mutex_lock(&lock);
	int fd = Storage_OpenMutableFile();
	// read 8KB as defined by the max file size in app manifest.json
	memset(stringbuffer, 0, FILE_SIZE);
	ssize_t read_data = read(fd, stringbuffer, 8000);
	Log_Debug("read %i bytes \n", read_data);
	close(fd);
	pthread_mutex_unlock(&lock);
	Log_Debug(stringbuffer);
	json_obj = json_parse_string(stringbuffer);

	if (json_value_get_type(json_obj) !=JSONArray)
	{
		Log_Debug("Not JSON array  \n");
		return card_data;
	}
	//json_object_get_arr
	jarray = json_value_get_array(json_obj);
	
	//JSON_Object * jo = json_array_get_object(jarray, 1);
	//Log_Debug(json_object_get_string(jo, "card"));
	
	for (int ii = 0; ii < json_array_get_count(jarray); ii++)
	{
		jo = json_array_get_object(jarray, ii);
		if (json_object_has_value_of_type(jo, "card_id", JSONString))
		{
			if (strcmp(json_object_get_string(jo, "card_id"), cardid) == 0)
			{
				//card_data.username = json_object_get_string(jo, "name");
				memset(card_data.username, 0, 11);
				snprintf(card_data.username, 11, "%s",json_object_get_string(jo, "name"));

				json_value_free(json_obj);
				Log_Debug("\n from card id %s found %s \n", cardid, card_data.username);
				card_data.error = false;
				json_obj == NULL;

				return card_data;
			}
		}
	}
	json_value_free(json_obj);
	return card_data;
}


