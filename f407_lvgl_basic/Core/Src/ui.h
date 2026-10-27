/*
 * ui.h
 *
 *  Created on: Oct 26, 2026
 *      Author: JUNJIEC
 */

#ifndef SRC_UI_H_
#define SRC_UI_H_
#ifndef WEATHER_CLOCK_UI_H
#define WEATHER_CLOCK_UI_H

void ui_init(void);
void ui_set_time(int hour, int minute, int second);
void ui_set_date(const char *date_text);
void ui_set_weather(const char *condition, int temperature_c);

#endif



#endif /* SRC_UI_H_ */
