/*
 * Parkinsense app: accelerometer pipeline and BLE sync for inspection (e.g. nRF Connect).
 */
#include "config.h"
#include "quickglui/quickglui.h"
#include "parkinsense.h"
#include <stdio.h>
#include <string.h>
#include "gui/mainbar/mainbar.h"
#include "gui/widget_styles.h"
#include "gui/app.h"
#include "hardware/parkinsense_data.h"
#include "hardware/blectl.h"
#include "hardware/motor.h"

#ifdef NATIVE_64BIT
#else
    #if defined(LILYGO_WATCH_2020_V1) || defined(LILYGO_WATCH_2020_V2) || defined(LILYGO_WATCH_2020_V3)
        #include <TTGO.h>
    #endif
#endif

LV_IMG_DECLARE(move_64px);
LV_IMG_DECLARE(trash_32px);
LV_IMG_DECLARE(play_32px);
LV_IMG_DECLARE(stop_32px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);

#define YES "Yes"
#define NO  "No"

static SynchronizedApplication parkinsenseApp;
static Style big, small;
static Label lblStatus;
static Label lblPipeline;
static Label lblBuffer;
static Label lblHint;
static lv_task_t* parkinsense_refresh_task = NULL;
static lv_event_cb_t default_msgbox_cb;

static void build_main_page(void);
static void refresh_main_page(void);
static void build_settings(void);
static void parkinsense_activate_cb(void);
static void parkinsense_reset_cb(lv_obj_t* obj, lv_event_t event);

static int registered = app_autocall_function(&parkinsense_app_setup, 8);

void parkinsense_app_setup(void) {
    if (!registered) return;

    parkinsense_data_pipeline_init();

    parkinsenseApp.init("parkinsense", &move_64px, 1, 1);
    mainbar_add_tile_activate_cb(parkinsenseApp.mainTileId(), parkinsense_activate_cb);

    build_main_page();
    build_settings();

    parkinsenseApp.synchronizeActionHandler([](SyncRequestSource source) {
        (void)source;
        parkinsense_data_pipeline_send_data();
        motor_vibe(20);
    });

    parkinsenseApp.mainPage().addAppButton(play_32px, [](Widget btn) {
        (void)btn;
        if (!parkinsense_data_pipeline_is_started()) {
            parkinsense_data_pipeline_start();
            motor_vibe(20);
            refresh_main_page();
        }
    });

    parkinsenseApp.mainPage().addAppButton(stop_32px, [](Widget btn) {
        (void)btn;
        if (parkinsense_data_pipeline_is_started()) {
            parkinsense_data_pipeline_stop();
            motor_vibe(20);
            refresh_main_page();
        }
    });

    parkinsenseApp.mainPage().addAppButton(trash_32px, [](Widget btn) {
        (void)btn;
        static const char* btns[] = { YES, NO, "" };
        lv_obj_t* mbox = lv_msgbox_create(lv_scr_act(), NULL);
        lv_msgbox_set_text(mbox, "Reset all collected data?");
        lv_msgbox_add_btns(mbox, btns);
        lv_obj_set_width(mbox, 200);
        default_msgbox_cb = lv_obj_get_event_cb(mbox);
        lv_obj_set_event_cb(mbox, parkinsense_reset_cb);
        lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
    });

#ifndef NATIVE_64BIT
    parkinsense_refresh_task = lv_task_create([](lv_task_t* task) {
        (void)task;
        refresh_main_page();
    }, 500, LV_TASK_PRIO_MID, NULL);
#endif
    refresh_main_page();
}

void build_main_page(void) {
    big = Style::Create(ws_get_mainbar_style(), true);
    big.textFont(&Ubuntu_32px).textOpacity(LV_OPA_80);
    small = Style::Create(ws_get_mainbar_style(), true);
    small.textFont(&Ubuntu_16px).textOpacity(LV_OPA_80);

    AppPage& screen = parkinsenseApp.mainPage();

    lblStatus = Label(&screen);
    lblStatus.text("BLE: --")
        .style(small, true)
        .align(screen, LV_ALIGN_IN_TOP_MID, 0, 8);

    lblPipeline = Label(&screen);
    lblPipeline.text("Pipeline: Stopped")
        .style(small, true)
        .align(lblStatus, LV_ALIGN_OUT_BOTTOM_MID);

    lblBuffer = Label(&screen);
    lblBuffer.text("Buffer: 0")
        .style(big, true)
        .align(lblPipeline, LV_ALIGN_OUT_BOTTOM_MID);

    lblHint = Label(&screen);
    lblHint.text("Refresh = Send to phone (nRF)")
        .style(small, true)
        .align(lblBuffer, LV_ALIGN_OUT_BOTTOM_MID);
}

void refresh_main_page(void) {
#ifndef NATIVE_64BIT
    char buf[48];
    bool ble = blectl_get_event(BLECTL_CONNECT);
    lblStatus.text(ble ? "BLE: Connected" : "BLE: Disconnected").realign();

    bool started = parkinsense_data_pipeline_is_started();
    lblPipeline.text(started ? "Pipeline: Running" : "Pipeline: Stopped").realign();

    int n = parkinsense_data_pipeline_buff_count();
    snprintf(buf, sizeof(buf), "Buffer: %d", n);
    lblBuffer.text(buf).realign();
#else
    (void)0;
#endif
}

void parkinsense_activate_cb(void) {
    refresh_main_page();
}

static void parkinsense_reset_cb(lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_VALUE_CHANGED) {
        const char* answer = lv_msgbox_get_active_btn_text(obj);
        if (answer && strcmp(answer, YES) == 0) {
            parkinsense_data_pipeline_reset();
            refresh_main_page();
        }
    }
    if (default_msgbox_cb)
        default_msgbox_cb(obj, event);
}

void build_settings(void) {
    SettingsPage& settings = parkinsenseApp.settingsPage();
    (void)settings;
}
