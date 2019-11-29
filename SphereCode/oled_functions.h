#pragma once
long screen_last_update;


void oled_refresh_screen();


void oled_present_card();

void oled_success_card_add();

void oled_remote_open();

void oled_unknown_user();
void init_screen(void);