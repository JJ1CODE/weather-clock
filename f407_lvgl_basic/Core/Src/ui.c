/*
 * ui.c
 *
 *  Created on: Oct 26, 2026
 *      Author: JUNJIEC
 */
#include "ui.h"

#include <stdbool.h>

#include "lvgl/lvgl.h"

/*
 * A small 240x320 layout inspired by LVGL's official music-player and
 * smartwatch demos. All art is drawn with LVGL objects, so there are no
 * large image arrays to carry over to the STM32 project.
 */
#define C_BG      0x0B1729
#define C_CARD    0x162C46
#define C_CARD_2  0x1D3956
#define C_TEXT    0xF0F5FB
#define C_MUTED   0xA2B5CA
#define C_GOLD    0xF4C66E
#define C_BLUE    0x7BB5D8

static lv_obj_t *home_screen;
static lv_obj_t *music_screen;
static lv_obj_t *settings_panel;
static lv_obj_t *time_label;
static lv_obj_t *date_label;
static lv_obj_t *seconds_label;
static lv_obj_t *weather_label;
static lv_obj_t *temp_label;
static lv_obj_t *play_label;
static lv_obj_t *progress_bar;
static bool playing;
static int progress_value = 22;

static lv_obj_t *text_at(lv_obj_t *parent, const char *text,
                         int x, int y, const lv_font_t *font, uint32_t color)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    return label;
}

static lv_obj_t *surface(lv_obj_t *parent, int x, int y, int w, int h,
                         int radius, uint32_t color)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_style_radius(obj, radius, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_scrollable(obj, false);
    return obj;
}

static lv_obj_t *button_at(lv_obj_t *parent, int x, int y, int w, int h,
                           int radius, uint32_t color)
{
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_remove_style_all(btn);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, w, h);
    lv_obj_set_style_radius(btn, radius, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_scrollable(btn, false);
    return btn;
}

static void panel_y_cb(void *obj, int32_t y)
{
    lv_obj_set_y((lv_obj_t *)obj, y);
}

static void move_panel(int end_y)
{
    lv_anim_delete(settings_panel, panel_y_cb);
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, settings_panel);
    lv_anim_set_exec_cb(&anim, panel_y_cb);
    lv_anim_set_values(&anim, lv_obj_get_y(settings_panel), end_y);
    lv_anim_set_duration(&anim, 250);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
    lv_anim_start(&anim);
}

static void open_music_cb(lv_event_t *event)
{
    (void)event;
    lv_screen_load_anim(music_screen, LV_SCREEN_LOAD_ANIM_MOVE_LEFT,
                        240, 0, false);
}

static void back_home_cb(lv_event_t *event)
{
    (void)event;
    lv_screen_load_anim(home_screen, LV_SCREEN_LOAD_ANIM_MOVE_RIGHT,
                        240, 0, false);
}

static void open_settings_cb(lv_event_t *event)
{
    (void)event;
    move_panel(0);
}

static void close_settings_cb(lv_event_t *event)
{
    (void)event;
    move_panel(-320);
}

static void slider_changed_cb(lv_event_t *event)
{
    lv_obj_t *slider = lv_event_get_target(event);
    lv_obj_t *value = lv_event_get_user_data(event);
    lv_label_set_text_fmt(value, "%d%%", (int)lv_slider_get_value(slider));
}

static void play_clicked_cb(lv_event_t *event)
{
    (void)event;
    playing = !playing;
    lv_label_set_text(play_label, playing ? "II" : ">");
    lv_obj_center(play_label);
}

static void playback_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    if(!playing) return;
    progress_value = (progress_value + 1) % 101;
    lv_bar_set_value(progress_bar, progress_value, LV_ANIM_ON);
}

static void setup_screen(lv_obj_t *screen)
{
    lv_obj_set_style_bg_color(screen, lv_color_hex(C_BG), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_scrollable(screen, false);
}

static void build_home(void)
{
    date_label = text_at(home_screen, "MON  /  26 OCT", 17, 17,
                         &lv_font_montserrat_12, C_MUTED);
    lv_obj_t *hint = text_at(home_screen, "SWIPE DOWN", 0, 0,
                             &lv_font_montserrat_12, C_BLUE);
    lv_obj_align(hint, LV_ALIGN_TOP_RIGHT, -17, 18);

    lv_obj_t *weather_card = surface(home_screen, 14, 51, 212, 78, 18, C_CARD);
    surface(weather_card, 16, 18, 42, 42, 21, C_GOLD);
    weather_label = text_at(weather_card, "SUNNY", 73, 14,
                            &lv_font_montserrat_12, C_MUTED);
    temp_label = text_at(weather_card, "22 C", 71, 33,
                         &lv_font_montserrat_20, C_TEXT);

    time_label = text_at(home_screen, "08:36", 17, 149,
                         &lv_font_montserrat_48, C_TEXT);
    seconds_label = text_at(home_screen, "00", 197, 175,
                            &lv_font_montserrat_14, C_GOLD);
    text_at(home_screen, "A GOOD DAY STARTS HERE", 18, 209,
            &lv_font_montserrat_12, C_MUTED);

    lv_obj_t *music_card = button_at(home_screen, 14, 247, 212, 59,
                                     17, C_CARD_2);
    surface(music_card, 11, 11, 37, 37, 11, C_GOLD);
    surface(music_card, 22, 18, 4, 17, 2, C_BG);
    surface(music_card, 29, 14, 4, 21, 2, C_BG);
    text_at(music_card, "MUSIC", 61, 10,
            &lv_font_montserrat_14, C_TEXT);
    text_at(music_card, "OPEN PLAYER", 61, 32,
            &lv_font_montserrat_12, C_MUTED);
    text_at(music_card, ">", 184, 20,
            &lv_font_montserrat_18, C_GOLD);
    lv_obj_add_event_cb(music_card, open_music_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(home_screen, open_settings_cb,
                        LV_EVENT_GESTURE_DOWN, NULL);
}

static void build_music(void)
{
    lv_obj_t *back = button_at(music_screen, 15, 15, 52, 31, 10, C_CARD);
    text_at(back, "<", 18, 4, &lv_font_montserrat_18, C_TEXT);
    lv_obj_add_event_cb(back, back_home_cb, LV_EVENT_CLICKED, NULL);
    text_at(music_screen, "NOW PLAYING", 89, 23,
            &lv_font_montserrat_12, C_MUTED);

    lv_obj_t *album = surface(music_screen, 43, 55, 154, 143, 20, C_CARD_2);
    surface(album, 29, 22, 96, 96, 48, 0x315F78);
    surface(album, 47, 40, 60, 60, 30, C_GOLD);
    surface(album, 69, 55, 8, 49, 4, C_BG);
    surface(album, 77, 55, 26, 8, 4, C_BG);
    surface(album, 58, 92, 20, 13, 7, C_BG);

    text_at(music_screen, "GOLDEN HOUR", 39, 211,
            &lv_font_montserrat_18, C_TEXT);
    text_at(music_screen, "DEMO TRACK  /  LOCAL", 39, 233,
            &lv_font_montserrat_12, C_MUTED);

    progress_bar = lv_bar_create(music_screen);
    lv_obj_set_pos(progress_bar, 26, 265);
    lv_obj_set_size(progress_bar, 188, 4);
    lv_bar_set_range(progress_bar, 0, 100);
    lv_bar_set_value(progress_bar, progress_value, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(progress_bar, lv_color_hex(C_CARD_2), LV_PART_MAIN);
    lv_obj_set_style_bg_color(progress_bar, lv_color_hex(C_GOLD), LV_PART_INDICATOR);

    lv_obj_t *play = button_at(music_screen, 103, 278, 34, 34, 17, C_GOLD);
    play_label = text_at(play, ">", 0, 0, &lv_font_montserrat_18, C_BG);
    lv_obj_center(play_label);
    lv_obj_add_event_cb(play, play_clicked_cb, LV_EVENT_CLICKED, NULL);
    text_at(music_screen, "|<", 64, 285, &lv_font_montserrat_14, C_MUTED);
    text_at(music_screen, ">|", 161, 285, &lv_font_montserrat_14, C_MUTED);
    lv_obj_add_event_cb(music_screen, open_settings_cb,
                        LV_EVENT_GESTURE_DOWN, NULL);
    lv_timer_create(playback_timer_cb, 900, NULL);
}

static void build_settings(void)
{
    settings_panel = surface(lv_layer_top(), 0, -320, 240, 320, 0, 0x132840);
    surface(settings_panel, 99, 10, 42, 4, 2, C_MUTED);
    text_at(settings_panel, "QUICK SETTINGS", 22, 38,
            &lv_font_montserrat_18, C_TEXT);
    text_at(settings_panel, "DISPLAY", 22, 100,
            &lv_font_montserrat_12, C_MUTED);
    lv_obj_t *brightness_value = text_at(settings_panel, "70%", 178, 100,
                                          &lv_font_montserrat_12, C_GOLD);
    lv_obj_t *brightness = lv_slider_create(settings_panel);
    lv_obj_set_pos(brightness, 22, 132);
    lv_obj_set_size(brightness, 196, 8);
    lv_slider_set_range(brightness, 0, 100);
    lv_slider_set_value(brightness, 70, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(brightness, lv_color_hex(C_CARD_2), LV_PART_MAIN);
    lv_obj_set_style_bg_color(brightness, lv_color_hex(C_GOLD), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(brightness, lv_color_hex(C_GOLD), LV_PART_KNOB);
    lv_obj_add_event_cb(brightness, slider_changed_cb,
                        LV_EVENT_VALUE_CHANGED, brightness_value);

    text_at(settings_panel, "VOLUME", 22, 185,
            &lv_font_montserrat_12, C_MUTED);
    lv_obj_t *volume_value = text_at(settings_panel, "45%", 178, 185,
                                      &lv_font_montserrat_12, C_GOLD);
    lv_obj_t *volume = lv_slider_create(settings_panel);
    lv_obj_set_pos(volume, 22, 217);
    lv_obj_set_size(volume, 196, 8);
    lv_slider_set_range(volume, 0, 100);
    lv_slider_set_value(volume, 45, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(volume, lv_color_hex(C_CARD_2), LV_PART_MAIN);
    lv_obj_set_style_bg_color(volume, lv_color_hex(C_GOLD), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(volume, lv_color_hex(C_GOLD), LV_PART_KNOB);
    lv_obj_add_event_cb(volume, slider_changed_cb,
                        LV_EVENT_VALUE_CHANGED, volume_value);

    lv_obj_t *close = button_at(settings_panel, 22, 263, 196, 38,
                                12, C_CARD_2);
    text_at(close, "SWIPE UP  /  CLOSE", 29, 11,
            &lv_font_montserrat_12, C_TEXT);
    lv_obj_add_event_cb(close, close_settings_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(settings_panel, close_settings_cb,
                        LV_EVENT_GESTURE_UP, NULL);
}

void ui_init(void)
{
    home_screen = lv_screen_active();
    music_screen = lv_obj_create(NULL);
    setup_screen(home_screen);
    setup_screen(music_screen);
    build_home();
    build_music();
    build_settings();
}

void ui_set_time(int hour, int minute, int second)
{
    if(time_label == NULL || seconds_label == NULL) return;
    lv_label_set_text_fmt(time_label, "%02d:%02d", hour, minute);
    lv_label_set_text_fmt(seconds_label, "%02d", second);
}

void ui_set_date(const char *date_text)
{
    if(date_label == NULL || date_text == NULL) return;
    lv_label_set_text(date_label, date_text);
}

void ui_set_weather(const char *condition, int temperature_c)
{
    if(weather_label == NULL || temp_label == NULL) return;
    lv_label_set_text(weather_label, condition ? condition : "OFFLINE");
    lv_label_set_text_fmt(temp_label, "%d C", temperature_c);
}


