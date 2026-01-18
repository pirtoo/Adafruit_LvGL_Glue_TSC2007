// Minimal "Hello" example for LittlevGL on Adafruit TFT FeatherWings.
// Requires LittlevGL, Adafruit_LvGL_Glue, Adafruit_STMPE610, Adafruit_GFX
// and Adafruit_ILI9341 (2.4" TFT) or Adafruit_HX8357 (3.5") libraries.
// This example doesn't use any touchscreen input, but it's declared anyway
// so this sketch can be copied-and-pasted to serve as a starting point for
// other projects. If display is scrambled, check that correct FeatherWing
// type is selected below (set BIG_FEATHERWING to 0 or 1 as needed).
// If the display is a V2 with the TSC2007 set TSC2007_TS to 1.

// Prior Adafruit_LvGL_Glue users: see hello_changes example for updates!

#define BIG_FEATHERWING 0 // Set this to 1 for 3.5" (480x320) FeatherWing!
#ifndef TSC2007_TS
#define TSC2007_TS 0 // Set this to 1 for V2 TSC2007 touchscreen displays
#endif

#define GLUE_LOCKING

#include <Arduino.h>

#include <Adafruit_LvGL_Glue.h> // Always include this BEFORE lvgl.h!
#include <lvgl.h>
#if TSC2007_TS
#include <Adafruit_TSC2007.h>
#include <Wire.h>
#else
#include <Adafruit_STMPE610.h>
#endif

#ifdef ESP32
   #define TFT_CS   15
   #define TFT_DC   33
   #define STMPE_CS 32
#else
   #define TFT_CS    9
   #define TFT_DC   10
   #define STMPE_CS  6
#endif
#define TFT_ROTATION 3 // Landscape orientation on FeatherWing
#define TFT_RST     -1

#if BIG_FEATHERWING
  #include <Adafruit_HX8357.h>
  Adafruit_HX8357  tft(TFT_CS, TFT_DC, TFT_RST);
#else
  #include <Adafruit_ILI9341.h>
  Adafruit_ILI9341 tft(TFT_CS, TFT_DC);
#endif
#if TSC2007_TS
#define TSC_IRQ STMPE_CS
Adafruit_TSC2007 ts=Adafruit_TSC2007();
#else
Adafruit_STMPE610  ts(STMPE_CS);
#endif
Adafruit_LvGL_Glue glue;

static void btn_event_cb(lv_event_t * e);
void lv_button(void);

static void btn_event_cb(lv_event_t * e) {
#ifdef GLUE_LOCKING
  glue.lvgl_acquire();
#endif //GLUE_LOCKING
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * btn = lv_event_get_target(e);
  if(code == LV_EVENT_CLICKED) {
    static uint8_t cnt = 0;
    cnt++;
    Serial.println("Button clicked");
    /*Get the first child of the button which is the label and change its text*/
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    lv_label_set_text_fmt(label, "Button: %d", cnt);
  }
#ifdef GLUE_LOCKING
  glue.lvgl_release();
#endif //GLUE_LOCKING
}

/**
 * Create a button with a label and react on click event.
 */
void lv_button(void) {
#ifdef GLUE_LOCKING
  // Glue locking here locks things up, do not know why
  glue.lvgl_acquire();
#endif //GLUE_LOCKING
  lv_obj_t * btn = lv_btn_create(lv_scr_act());     /*Add a button the current screen*/
  lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
  lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
  lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

#ifdef GLUE_LOCKING
  glue.lvgl_release();
#endif //GLUE_LOCKING
  // Doing this with the glue lock on hangs?
  lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
  lv_label_set_text(label, "Button");                     /*Set the labels text*/
  lv_obj_center(label);
}

// This example sketch's LittlevGL UI-building calls are all in this
// function rather than in setup(), so simple programs can just
// copy-and-paste this sketch as a starting point, then embellish here:
void lvgl_setup(void) {
  // Locks LVGL resource to prevent memory corrupton on ESP32
  // When using WiFi and LVGL_Glue, this function MUST be called PRIOR to LVGL function (`lv_`) calls
#ifdef GLUE_LOCKING
  glue.lvgl_acquire();
#endif //GLUE_LOCKING

  // Create simple label centered on screen
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello Arduino!");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  // Unlocks LVGL resource to prevent memory corrupton on ESP32
  // NOTE: This function MUST be called AFTER lvgl_acquire
#ifdef GLUE_LOCKING
  glue.lvgl_release();
#endif //GLUE_LOCKING
}

void setup(void) {
  Serial.begin(115200);

  // Initialize display and touchscreen BEFORE glue setup
  tft.begin();
  tft.setRotation(TFT_ROTATION);

#if TSC2007_TS
  if (ts.begin(0x48, &Wire)) {
    pinMode(TSC_IRQ, INPUT);
  } else {
    Serial.println("Couldn't start TSC2007 touchscreen controller");
    for(;;);
  }
  // Initialize glue, passing in address of display & touchscreen
  LvGLStatus status = glue.begin(&tft, &ts, TSC_IRQ);
#else
  if(!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    for(;;);
  }
  // Initialize glue, passing in address of display & touchscreen
  LvGLStatus status = glue.begin(&tft, &ts);
#endif

  if(status != LVGL_OK) {
    Serial.printf("Glue error %d\r\n", (int)status);
    for(;;);
  }

  lvgl_setup(); // Call UI-building function above
  lv_button();  // Add a button
}

void loop(void) {
  // On ESP32, LittleVGL's task handler (`lv_task_handler`) is a task in
  // FreeRTOS and is pinned to the core upon successful initialization.
  // This means that you do not need to call `lv_task_handler()` within the loop() on ESP32
#ifndef ESP32
  lv_task_handler();
#endif //ESP32
  delay(5);
}