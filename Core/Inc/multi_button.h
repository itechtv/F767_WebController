/*
 * Copyright (c) 2016 Zibin Zheng <znbin@qq.com>
 * All rights reserved
 */

#ifndef _MULTI_BUTTON_H_
#define _MULTI_BUTTON_H_

#include <stdint.h>
#include <string.h>

//According to your need to modify the constants.
#define TICKS_INTERVAL    5	//ms
#define DEBOUNCE_TICKS    3	//MAX 7 (0 ~ 7)
#define SHORT_TICKS       (21000 / TICKS_INTERVAL)// 300 ms  ("DOUBLE_CLICK" кагда handle->ticks < SHORT_TICKS!)
#define LONG_TICKS        (70000 /TICKS_INTERVAL)// 1000


typedef void (*BtnCallback)(void*);

typedef enum {
	PRESS_DOWN = 0,
	PRESS_UP,
	PRESS_REPEAT,
	SINGLE_CLICK,
	DOUBLE_CLICK,
	LONG_PRESS_START,
	LONG_PRESS_HOLD,
	number_of_event,
	NONE_PRESS
}PressEvent;

typedef PressEvent BtnEvent;
//typedef enum {
//    PRESS_DOWN,
//    PRESS_UP,
//    LONG_PRESS_START,
//    LONG_PRESS_HOLD,
//    SINGLE_CLICK,
//    DOUBLE_CLICK,
//    PRESS_REPEAT
//} BtnEvent;

typedef struct Button {
	uint16_t ticks;
	uint8_t  repeat : 4;
	uint8_t  event : 4;
	uint8_t  state : 3;
	uint8_t  debounce_cnt : 3;
	uint8_t  active_level : 1;
	uint8_t  button_level : 1;
	uint8_t  button_id;
	uint8_t  (*hal_button_Level)(uint8_t button_id_);
	BtnCallback  cb[number_of_event];
	struct Button* next;
}Button;

#ifdef __cplusplus
extern "C" {
#endif

void button_init(struct Button* handle, uint8_t(*pin_level)(uint8_t), uint8_t active_level, uint8_t button_id);
void button_attach(struct Button* handle, PressEvent event, BtnCallback cb);
PressEvent get_button_event(struct Button* handle);
int  button_start(struct Button* handle);
void button_stop(struct Button* handle);
void button_ticks(struct Button *buttons);

void button_event_handler(Button* handle);
uint8_t read_button_level(uint8_t button_id);
uint8_t read_button_level(uint8_t button_id);
void attach_events_to_all_buttons(Button *buttons, uint8_t size, BtnEvent event, BtnCallback callback);
void initialize_buttons(Button *buttons, uint8_t size, uint8_t (*hal_button_Level)(uint8_t button_id),uint8_t active_level);
void start_all_buttons(Button *buttons, uint8_t size);
#ifdef __cplusplus
}
#endif

#endif
