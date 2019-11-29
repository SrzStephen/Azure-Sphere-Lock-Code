#include "rdm6300.h"
#define UART_STRUCTS_VERSION 1
#include <applibs/uart.h>
#include <applibs/log.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h> 
#include <pthread.h>

#define MAX_READ_SECONDS 5
struct CARD card_data;
char data_buffer[12];
int bytes_read = 0;
long start_time;
long end_time;

const struct timespec timespec_10ms = { 0, 10000000 };
int to_hex(int higher, int lower)
{
	// yeaa......
	int t_high = 0;
	int t_low = 0;
	if (higher > 57) {t_high = higher - 55;}
	else { t_high = higher - 48; }
	if (lower > 57) { t_low = lower - 55; }
	else { t_low = lower - 48; }

	return t_high * 16 + t_low;
}

struct CARD get_card_id(int ISU_Number)
{
	int ISU1_fd = -1;
	UART_Config uartConfig;
	UART_InitConfig(&uartConfig);
	uartConfig.baudRate = 9600;
	pthread_mutex_lock(&read_lock);
	ISU1_fd = UART_Open(ISU_Number, &uartConfig);
	Log_Debug("UART STATUS %d \n", ISU1_fd);
	card_data.error = true;
	card_data.card_id;
	start_time = time(NULL);
	end_time = time(NULL);
	bytes_read = 0;
	uint8_t initial_buffer[28];

	while ((end_time - start_time) < MAX_READ_SECONDS)
	{
		end_time = time(NULL);
		nanosleep(&timespec_10ms, NULL);
		bytes_read = read(ISU1_fd, initial_buffer, 28);
		
		if (bytes_read == 28)
		Log_Debug("bytes read: %i \n", bytes_read);
		{
			for (int ii = 0; ii < 13; ii++)
			{
				if (initial_buffer[ii] == 0x02 && initial_buffer[ii + 13] == 0x03)
				{
					// strip out the start and end bytes
					for (int jj = 0; jj < 12; jj++)
					{
						data_buffer[jj] = (char)initial_buffer[ii + jj+1];
					}
					int calc_checksum = to_hex(data_buffer[0], data_buffer[1]);
					for (int cc = 2; cc < 10; cc += 2)
					{
						calc_checksum ^= to_hex(data_buffer[cc], data_buffer[cc + 1]);
					}

					int actual_checksum = to_hex(data_buffer[10], data_buffer[11]);
					if (actual_checksum == calc_checksum)
					{

						// don't copy the last 2 bytes
						strncpy(card_data.card_id, data_buffer, 10);
						card_data.error = false;
						Log_Debug("\n Found card %s \n", card_data.card_id);
						close(ISU1_fd);
						pthread_mutex_unlock(&read_lock);
						return card_data;
					}
					
					
				}
			}
		}
	}
	close(ISU1_fd);
	pthread_mutex_unlock(&read_lock);
	Log_Debug("Didn't find a card in allocated time \n");
	return card_data;
}