
#include "oled_functions.h"
#include "sd1306.h"
#include <time.h>
#include "mt3620_avnet_dev.h"

void init_screen(void)
{
	i2cFd = I2CMaster_Open(MT3620_RDB_HEADER4_ISU2_I2C);
	I2CMaster_SetBusSpeed(i2cFd, I2C_BUS_SPEED_STANDARD);
	I2CMaster_SetTimeout(i2cFd, 100);
	sd1306_init();
	clear_oled_buffer();
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "Starting!", FONT_SIZE_TITLE, white_pixel);
	sd1306_refresh();
	screen_last_update = time(NULL);
}


void oled_refresh_screen()
{
	clear_oled_buffer();
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "Door", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, "Present", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(OLED_LINE_3_X, OLED_LINE_3_Y, "card", FONT_SIZE_TITLE, white_pixel);
	screen_last_update = time(NULL);
	sd1306_refresh();
}

void oled_present_card()
{
	clear_oled_buffer();
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "Door", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, "Ready to", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(OLED_LINE_3_X, OLED_LINE_3_Y, "add user", FONT_SIZE_TITLE, white_pixel);
	screen_last_update = time(NULL);
	sd1306_refresh();
}
void oled_success_card_add()
{
	clear_oled_buffer();
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "Door", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, "User Added!", FONT_SIZE_TITLE, white_pixel); \
	screen_last_update = time(NULL);
	sd1306_refresh();
}
void oled_remote_open()
{
	clear_oled_buffer();
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "Door", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, "Opened", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(OLED_LINE_3_X, OLED_LINE_3_Y, "remotely", FONT_SIZE_TITLE, white_pixel);
	screen_last_update = time(NULL);
	sd1306_refresh();
}
void oled_unknown_user()
{
	clear_oled_buffer();
	sd1306_draw_string(OLED_TITLE_X, OLED_TITLE_Y, "Door", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(OLED_LINE_1_X, OLED_LINE_1_Y, "Unknown!", FONT_SIZE_TITLE, white_pixel);
	sd1306_draw_string(OLED_LINE_3_X, OLED_LINE_3_Y, "Not Opening", FONT_SIZE_TITLE, white_pixel);
	sd1306_refresh();
	screen_last_update = time(NULL);
}

