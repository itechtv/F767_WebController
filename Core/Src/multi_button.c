/*
 * Copyright (c) 2016 Zibin Zheng <znbin@qq.com>
 * All rights reserved
 */

#include <stdio.h>
#include "multi_button.h"
#include <db.h>
extern struct dbPinsConf PinsConf[NUMPIN];
extern struct dbPinsInfo PinsInfo[NUMPIN];

#define EVENT_CB(ev)   if(handle->cb[ev])handle->cb[ev]((void*)handle)
#define PRESS_REPEAT_MAX_NUM  15 /*!< The maximum value of the repeat counter */

//button handle list head.
static struct Button* head_handle = NULL;

static void button_handler(struct Button* handle);

/**
  * @brief  Initializes the button struct handle.
  * @param  handle: the button handle struct.
  * @param  pin_level: read the HAL GPIO of the connected button level.
  * @param  active_level: pressed GPIO level.
  * @param  button_id: the button id.
  * @retval None
  */
void button_init(struct Button* handle, uint8_t(*pin_level)(uint8_t), uint8_t active_level, uint8_t button_id)
{
	memset(handle, 0, sizeof(struct Button));
	handle->event = (uint8_t)NONE_PRESS;
	handle->hal_button_Level = pin_level;
	handle->button_level = handle->hal_button_Level(button_id);
	handle->active_level = active_level;
	handle->button_id = button_id;
}

/**
  * @brief  Attach the button event callback function.
  * @param  handle: the button handle struct.
  * @param  event: trigger event type.
  * @param  cb: callback function.
  * @retval None
  */
void button_attach(struct Button* handle, PressEvent event, BtnCallback cb)
{
	handle->cb[event] = cb;
}

/**
  * @brief  Inquire the button event happen.
  * @param  handle: the button handle struct.
  * @retval button event.
  */
PressEvent get_button_event(struct Button* handle)
{
	return (PressEvent)(handle->event);
}

/**
  * @brief  Button driver core function, driver state machine.
  * @param  handle: the button handle struct.
  * @retval None
  */
static void button_handler(struct Button* handle)
{
	uint8_t read_gpio_level = handle->hal_button_Level(handle->button_id);

	//ticks counter working..
	if((handle->state) > 0) handle->ticks++;

	/*------------button debounce handle---------------*/
	if(read_gpio_level != handle->button_level) { //not equal to prev one
		//continue read 3 times same new level change
		if(++(handle->debounce_cnt) >= DEBOUNCE_TICKS) {
			handle->button_level = read_gpio_level;
			handle->debounce_cnt = 0;
		}
	} else { //level not change ,counter reset.
		handle->debounce_cnt = 0;
	}

	/*-----------------State machine-------------------*/
	switch (handle->state) {
	case 0:
		if(handle->button_level == handle->active_level) {	//start press down
			handle->event = (uint8_t)PRESS_DOWN;
			EVENT_CB(PRESS_DOWN);
			//printf("CASE-0: handle->repeat: %d, handle->ticks: %d\n", (handle->repeat + 1), handle->ticks);
			handle->ticks = 0;
			handle->repeat = 1;
			handle->state = 1;
		} else {
			handle->event = (uint8_t)NONE_PRESS;
		}
		break;

	case 1:
		if(handle->button_level != handle->active_level) { //released press up
			handle->event = (uint8_t)PRESS_UP;
			EVENT_CB(PRESS_UP);
			//printf("CASE-1: handle->repeat: %d, handle->ticks: %d\n", handle->repeat, handle->ticks);
			handle->ticks = 0;
			handle->state = 2;
		} else if(handle->ticks > LONG_TICKS) {
			handle->event = (uint8_t)LONG_PRESS_START;
			EVENT_CB(LONG_PRESS_START);
			handle->state = 5;
		}
		break;

	case 2:
		if(handle->button_level == handle->active_level) { //press down again
			handle->event = (uint8_t)PRESS_DOWN;
			EVENT_CB(PRESS_DOWN);
			if(handle->repeat != PRESS_REPEAT_MAX_NUM) {
				handle->repeat++;
			}
			EVENT_CB(PRESS_REPEAT); // repeat hit
			//printf("CASE-2: handle->repeat: %d, handle->ticks: %d\n", handle->repeat, handle->ticks);
			handle->ticks = 0;
			handle->state = 3;
		} else if(handle->ticks > SHORT_TICKS) { //released timeout
			if(handle->repeat == 1) {
				handle->event = (uint8_t)SINGLE_CLICK;
				EVENT_CB(SINGLE_CLICK);
			} else if(handle->repeat == 2) {
				handle->event = (uint8_t)DOUBLE_CLICK;
				EVENT_CB(DOUBLE_CLICK); // repeat hit
			}
			handle->state = 0;
		}
		break;

	case 3:
		if(handle->button_level != handle->active_level) { //released press up
			handle->event = (uint8_t)PRESS_UP;
			EVENT_CB(PRESS_UP);
			if(handle->ticks < SHORT_TICKS) {
				handle->ticks = 0;
				handle->state = 2; //repeat press
			} else {
				handle->state = 0;
			}
		} else if(handle->ticks > SHORT_TICKS) { // SHORT_TICKS < press down hold time < LONG_TICKS
			handle->state = 1;
		}
		break;

	case 5:
		if(handle->button_level == handle->active_level) {
			//continue hold trigger
			handle->event = (uint8_t)LONG_PRESS_HOLD;
			EVENT_CB(LONG_PRESS_HOLD);
		} else { //released
			handle->event = (uint8_t)PRESS_UP;
			EVENT_CB(PRESS_UP);
			handle->state = 0; //reset
		}
		break;
	default:
		handle->state = 0; //reset
		break;
	}
}

/**
  * @brief  Start the button work, add the handle into work list.
  * @param  handle: target handle struct.
  * @retval 0: succeed. -1: already exist.
  */
int button_start(struct Button* handle)
{
	struct Button* target = head_handle;
	while(target) {
		if(target == handle) return -1;	//already exist.
		target = target->next;
	}
	handle->next = head_handle;
	head_handle = handle;
	return 0;
}

/**
  * @brief  Stop the button work, remove the handle off work list.
  * @param  handle: target handle struct.
  * @retval None
  */
void button_stop(struct Button* handle)
{
	struct Button** curr;
	for(curr = &head_handle; *curr; ) {
		struct Button* entry = *curr;
		if(entry == handle) {
			*curr = entry->next;
//			free(entry);
			return;//glacier add 2021-8-18
		} else {
			curr = &entry->next;
		}
	}
}

/**
  * @brief  background ticks, timer repeat invoking interval 5ms.
  * @param  None.
  * @retval None
  */

//void button_ticks(struct Button *buttons,uint8_t size) {
//	for (uint8_t i = 0; i < size; ++i) {
//		button_handler(&buttons[i]);
//	}
//	return 0;
//}

void button_ticks(struct Button *buttons) {
	button_handler(buttons);
	//return 0;
}

// Функция обратного вызова для обработки событий кнопки
void button_event_handler(Button* handle)
{
    // Обработчик событий кнопки
    PressEvent event = get_button_event(handle);

    switch (event) {
        case NONE_PRESS:
            // Нет нажатия
            break;
        case PRESS_DOWN:
            // Кнопка нажата
            printf("Button %d: PRESS_DOWN!\r\n", handle->button_id);
            break;
        case PRESS_UP:
            // Кнопка отпущена
            printf("Button %d: PRESS_UP!\r\n", handle->button_id);
            break;
        case LONG_PRESS_START:
            // Начало долгого нажатия
            printf("Button %d: LONG_PRESS_START!\r\n", handle->button_id);
            break;
        case LONG_PRESS_HOLD:
            // Продолжение долгого нажатия
            printf("Button %d: LONG_PRESS_HOLD!\r\n", handle->button_id);
            break;
        case SINGLE_CLICK:
            // Одиночное нажатие кнопки
            printf("Button %d: SINGLE_CLICK!\r\n", handle->button_id);
            break;
        case DOUBLE_CLICK:
            // Двойное нажатие кнопки
            printf("Button %d: DOUBLE_CLICK!\r\n", handle->button_id);
            break;
        case PRESS_REPEAT:
            // Повторное нажатие кнопки
            printf("Button %d: PRESS_REPEAT!\r\n", handle->button_id);
            break;
        default:
            // Обработка неизвестного значения event
            break;
    }
}

// Функция для получения состояния GPIO кнопки
 uint8_t read_button_level(uint8_t button_id)
 {
     // Вернуть состояние GPIO пина, к которому подключена кнопка
	 return  HAL_GPIO_ReadPin(PinsInfo[button_id].gpio_name, PinsInfo[button_id].hal_pin);

     //return GPIO_PIN_RESET; // Значение по умолчанию, если кнопка не найдена
 }

//	void initialize_buttons(Button *buttons, uint8_t size, uint8_t (*hal_button_Level)(uint8_t button_id),uint8_t active_level) {
//		for (uint8_t i = 0; i < size; ++i) {
//			if (PinsConf[i].topin == 1) {
//				button_init(&buttons[i], hal_button_Level, active_level, i);
//			}
//		}
//	}
//
//	void attach_events_to_all_buttons(Button *buttons, uint8_t size, BtnEvent event, BtnCallback callback) {
//		for (uint8_t i = 0; i < size; ++i) {
//			if (PinsConf[i].topin == 1) {
//				button_attach(&buttons[i], event, callback);
//			}
//		}
//	}
//
//    // Запуск работы кнопки
//	void start_all_buttons(Button *buttons, uint8_t size) {
//		for (uint8_t i = 0; i < size; ++i) {
//			if (PinsConf[i].topin == 1) {
//				button_start(&buttons[i]);
//			}
//		}
//	}
