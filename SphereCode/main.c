
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h> 
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include "applibs_versions.h"
#include "azure_iot_utilities.h"
#include "connection_strings.h"
#include <applibs/storage.h>
#include <applibs/log.h>
#include <applibs/wificonfig.h>
#include <azureiot/iothub_device_client_ll.h>
#include "epoll_timerfd_utilities.h"
#define UART_STRUCTS_VERSION 1
#define MAX_READ_TIME 30
#include <applibs/uart.h>
#include <applibs/gpio.h>
#include "mt3620_avnet_dev.h"
#include "sd1306.h"
#include "parson.h"
#include "rdm6300.h"
#include "tests.h"
#include "data_manip.h"
#include "oled_functions.h"
#include <soc/mt3620_uarts.h>
#define REFRESH_SCREEN_TIME 3
WifiConfig_ConnectedNetwork network;
int relay_fd = -1;
int reed_fd = -1;
int ISU1_fd = -1;
int buzzer_fd = -1;
GPIO_Value_Type reed_switch_last_status = -1;
struct CARD_STORED card_data;
struct CARD card_id;
bool global_connected = 0;
static bool connectedToIoTHub = false;
long last_response = 0;
extern IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubClientHandle;
const struct timespec timespec_100ms = { 0, 100000000 };
void open_door(char username[])
{
	clear_oled_buffer();
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "Door", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, "Welcome", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(OLED_LINE_3_X, OLED_LINE_3_Y, username, FONT_SIZE_TITLE, white_pixel);
	sd1306_refresh();
	screen_last_update = time(NULL);
	GPIO_SetValue(relay_fd, GPIO_Value_High);
	sleep(5);
	GPIO_SetValue(relay_fd, GPIO_Value_Low);
}

void happy_beep(int buzzer_fd)
{
	// make a happy chirp
	GPIO_SetValue(buzzer_fd, GPIO_Value_High);
	nanosleep(&timespec_100ms, NULL);
	nanosleep(&timespec_100ms, NULL);
	GPIO_SetValue(buzzer_fd, GPIO_Value_Low);


}
void sad_beep(int buzzer_fd)
{
	// beep 5 times.
	for (int ii = 0; ii < 5; ii++)
	{
		GPIO_SetValue(buzzer_fd, GPIO_Value_High);
		nanosleep(&timespec_100ms, NULL);
		GPIO_SetValue(buzzer_fd, GPIO_Value_Low);
		nanosleep(&timespec_100ms, NULL);
	}
}
static void *SetupHeapMessage(const char *messageFormat, size_t maxLength, ...)
{
	va_list args;
	va_start(args, maxLength);
	char *message =
		malloc(maxLength + 1); // Ensure there is space for the null terminator put by vsnprintf.
	if (message != NULL) {
		vsnprintf(message, maxLength, messageFormat, args);
	}
	va_end(args);
	return message;
}
static int DirectMethodCall(const char *methodName, const char *payload, size_t payloadSize, char **responsePayload, size_t *responsePayloadSize)
{
	// there is only one method supported "polltime" that should look like { "polltime":5 }
	Log_Debug("\nDirect Method called %s\n", methodName);
	Log_Debug("\nPayload %s/n", payload);
<<<<<<< HEAD

	if (strcmp(methodName, "remote_open")==0)
=======
	if (!strcmp(methodName, "polltime"))
>>>>>>> 056d1ee2b2b29a961cbda551f44351247cb76bd6
	{
		static const char response[] =
			"{ \"success\" : true, \"message\" : \"Door Opened\" }";
		size_t responseMaxLength = sizeof(response) + strlen(payload);
		*responsePayload = SetupHeapMessage(response, responseMaxLength, strlen(response));
		*responsePayloadSize = strlen(*responsePayload);
		open_door("");
		oled_remote_open();
		return 200;
	}

	if ( strcmp(methodName, "put_data")==0)
	{
		char p[FILE_SIZE] = { 0 };
		snprintf(p, FILE_SIZE, "%s", payload);

		if (dump_json_to_storage(p))
		{
			static const char response[] =
				"{ \"success\" : true, \"message\" : \"data added\" }";
			size_t responseMaxLength = sizeof(response) + strlen(payload);
			*responsePayload = SetupHeapMessage(response, responseMaxLength, strlen(response));
			*responsePayloadSize = strlen(*responsePayload);
		}
		else
		{
			static const char response[] =
				"{ \"success\" : false, \"message\" : \"data not added\" }";
			size_t responseMaxLength = sizeof(response) + strlen(payload);
			*responsePayload = SetupHeapMessage(response, responseMaxLength, strlen(response));
			*responsePayloadSize = strlen(*responsePayload);
		}
		return 200;
	}

	if (strcmp(methodName, "add_user")==0)
	{
		oled_present_card();
		happy_beep(buzzer_fd);
		long start_time;
		long end_time;
		start_time = time(NULL);
		end_time = time(NULL);
		while ((end_time - start_time) < MAX_READ_TIME)
		{
			Log_Debug("Trying to read card to add user \n");
			card_id = get_card_id(MT3620_UART_ISU1);
			if (card_data.error == false)
			{
				char response[100] = { 0 };
				sprintf(response, "{ \"success\" : true, \"message\" : \"%s\" }", card_id.card_id);
				size_t responseMaxLength = sizeof(response) + strlen(payload);
				*responsePayload = SetupHeapMessage(response, responseMaxLength, strlen(response));
				*responsePayloadSize = strlen(*responsePayload);
				oled_success_card_add();
				happy_beep(buzzer_fd);
				return 200;
			}
		}
		static const char response[] =
			"{ \"success\" : false, \"message\" : \"Couldn't add card\" }";
		size_t responseMaxLength = sizeof(response) + strlen(payload);
		*responsePayload = SetupHeapMessage(response, responseMaxLength, strlen(response));
		*responsePayloadSize = strlen(*responsePayload);
		sad_beep(buzzer_fd);
		return 200;
	}
	static const char response[] =
		"{ \"success\" : false, \"message\" : \"unknown method\" }";
	size_t responseMaxLength = sizeof(response) + strlen(payload);
	*responsePayload = SetupHeapMessage(response, responseMaxLength, strlen(response));
	*responsePayloadSize = strlen(*responsePayload);
	return 200;
}

static void IoTHubConnectionStatusChanged(bool connected)
{
	global_connected = connected;
	Log_Debug("Connection status %d\n", connected);
	connectedToIoTHub = connected;
}
static void MessageReceived(const char *payload)
{
	Log_Debug("Message received from the Azure IoT Hub: %d\n", payload);
}
static void SendMessageCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *context)
{
	Log_Debug("INFO: Message received by IoT Hub. Result is: %d\n", result);
	last_response = time(NULL);
}

static void SetupAzureClient(void)
{
	if (iothubClientHandle != NULL)
	{
		IoTHubDeviceClient_LL_Destroy(iothubClientHandle);
	}
	if (!AzureIoT_SetupClient()) {
		Log_Debug("ERROR: Failed to set up IoT Hub client\n");
	}
	AzureIoT_DoPeriodicTasks();
	sleep(10);
	AzureIoT_SetConnectionStatusCallback(&IoTHubConnectionStatusChanged);
	AzureIoT_SetDirectMethodCallback(&DirectMethodCall);
	AzureIoT_SetMessageReceivedCallback(&SendMessageCallback);
}

int main(int argc, char *argv[])
{
	sleep(5);
	relay_fd = GPIO_OpenAsOutput(AVT_MODULE_GPIO16, GPIO_OutputMode_PushPull,GPIO_Value_Low);
	reed_fd = GPIO_OpenAsInput(AVT_MODULE_GPIO42_ADC1);
	buzzer_fd = GPIO_OpenAsOutput(AVT_MODULE_GPIO0_PWM0, GPIO_OutputMode_PushPull, GPIO_Value_Low);
	int res = WifiConfig_GetCurrentNetwork(&network);
	Log_Debug("Network snr: %i\n", network.signalRssi);
	SetupAzureClient();
	init_screen();

	AzureIoT_SendMessage("Startup");
	char username[128];
	bool card_found = false;
	char data[11];
	oled_refresh_screen();

	while (true)
	{
		card_id = get_card_id(MT3620_UART_ISU1);
		if (card_id.error == false)
		{
			// reset this to force the next iteraton to run
			
			card_data = card_exists(card_id.card_id);
			if (card_data.error == false)
			{
				card_id.error = true;
				happy_beep(buzzer_fd);
				open_door(card_data.username);
				char namebuffer[100] = { 0 };
				snprintf(namebuffer, "{%s %s", strlen(namebuffer), "{\"%opened\":\"%s\"}", card_data.username);
				AzureIoT_SendMessage(namebuffer);
				screen_last_update = time(NULL);
			}
			else
			{
					oled_unknown_user();
					screen_last_update = time(NULL);
					sad_beep(buzzer_fd);
					
			}
		}
		if ((time(NULL) - screen_last_update) > REFRESH_SCREEN_TIME)
		{
			oled_refresh_screen();
		}
		AzureIoT_DoPeriodicTasks();
		GPIO_Value_Type reed_switch_state;
		GPIO_GetValue(reed_fd, reed_switch_state);
		if (reed_switch_last_status != reed_switch_state)
		{
			reed_switch_last_status = reed_switch_state;
			AzureIoT_TwinReportState("Door_open", reed_switch_state == GPIO_Value_High);
		}
		sleep(1);


	}

}
