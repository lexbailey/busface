#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0x0F, 0x08, 0xA7, 0x38, 0x2E, 0xE1, 0x45, 0x06, 0xA1, 0x30, 0x11, 0x22, 0xD0, 0xF6, 0x32, 0xD5 }
PBL_APP_INFO(MY_UUID,
             "BusFace", "Daniel Bailey",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
TextLayer dateLayer;
Layer display_layer;
TextLayer busLayer;

GPath bitPath;

#define DIGIT_0C 0b11111
#define DIGIT_0B 0b10001
#define DIGIT_0A 0b11111

#define DIGIT_1C 0b00001
#define DIGIT_1B 0b11111
#define DIGIT_1A 0b10001

#define DIGIT_2C 0b11101
#define DIGIT_2B 0b10101
#define DIGIT_2A 0b10111

#define DIGIT_3C 0b11111
#define DIGIT_3B 0b10101
#define DIGIT_3A 0b10101

#define DIGIT_4C 0b11111
#define DIGIT_4B 0b00100
#define DIGIT_4A 0b11100

#define DIGIT_5C 0b10111
#define DIGIT_5B 0b10101
#define DIGIT_5A 0b11101

#define DIGIT_6C 0b10111
#define DIGIT_6B 0b10101
#define DIGIT_6A 0b11111

#define DIGIT_7C 0b11100
#define DIGIT_7B 0b10110
#define DIGIT_7A 0b10011

#define DIGIT_8C 0b11111
#define DIGIT_8B 0b10101
#define DIGIT_8A 0b11111

#define DIGIT_9C 0b11111
#define DIGIT_9B 0b10100
#define DIGIT_9A 0b11100

#define DIGIT_SPACE 0b00000


#define ROW_WIDTH 18
#define COL_HEIGHT 5
#define YSTART 48
#define YSHORT 75
#define BIT_SPACING 0

GRect bitRect = {
    .origin = {
       .x=0,
       .y=50
    },

    .size = {
      .w = ((int)(144)/ROW_WIDTH)-BIT_SPACING,
      .h = ((int)((168-YSTART)-YSHORT)/COL_HEIGHT)-BIT_SPACING
    }
};

int adjX = 2;
int adjY = 0;

// Called once per minute
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  static char dateText[] = "Wednesday 01 September "; // Needs to be static because it's used by the system later.

  PblTm currentTime;

  get_time(&currentTime);

  string_format_time(dateText, sizeof(dateText), "%A %d %B", &currentTime);

  text_layer_set_text(&dateLayer, dateText);

}

void my_in_rcv_handler(DictionaryIterator *received, void *context) {

  // incoming message received

  Tuple *textdata = dict_find(received, 0);

  if (textdata) {
    text_layer_set_text(&busLayer, textdata->value->cstring);
    layer_mark_dirty(&display_layer);
  }
  else{
    text_layer_set_text(&busLayer, "error");
    layer_mark_dirty(&display_layer);
  }

}

int quickPow2(int power){
	int result = 1;
	for (int i = 0; i<= power-1; i++){
		result *=2;
	}
	return result;
}

void displayCol(GContext* ctx, int number){
  for (int i = 0; i<=COL_HEIGHT-1; i++){
    bitRect.origin.y=(YSTART+BIT_SPACING+(i*(bitRect.size.h+BIT_SPACING)))+adjY;
    graphics_draw_rect(ctx, bitRect);
    if (!((number & (quickPow2((COL_HEIGHT-1)-i))) > 0)){
      graphics_fill_rect(ctx, bitRect, 0, GCornerNone);
    }
  }
  bitRect.origin.x+=bitRect.size.w+BIT_SPACING;
}

void displayReset(){
  bitRect.origin.x=BIT_SPACING+adjX;
}

void displayNum(GContext* ctx, int num){
	int a = 0;
	int b = 0;
	int c = 0;
	switch (num){
		case 0: a = DIGIT_0A;b = DIGIT_0B;c = DIGIT_0C;break;
		case 1: a = DIGIT_1A;b = DIGIT_1B;c = DIGIT_1C;break;
		case 2: a = DIGIT_2A;b = DIGIT_2B;c = DIGIT_2C;break;
		case 3: a = DIGIT_3A;b = DIGIT_3B;c = DIGIT_3C;break;
		case 4: a = DIGIT_4A;b = DIGIT_4B;c = DIGIT_4C;break;
		case 5: a = DIGIT_5A;b = DIGIT_5B;c = DIGIT_5C;break;
		case 6: a = DIGIT_6A;b = DIGIT_6B;c = DIGIT_6C;break;
		case 7: a = DIGIT_7A;b = DIGIT_7B;c = DIGIT_7C;break;
		case 8: a = DIGIT_8A;b = DIGIT_8B;c = DIGIT_8C;break;
		case 9: a = DIGIT_9A;b = DIGIT_9B;c = DIGIT_9C;break;
	}
	displayCol(ctx, a);
        displayCol(ctx, b);
        displayCol(ctx, c);
}

void display_layer_update_callback(Layer *me, GContext* ctx) {

  PblTm t;

  get_time(&t);
  int m = t.tm_min;
  int h = t.tm_hour;

  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorBlack);

  displayReset();

  displayCol(ctx, DIGIT_SPACE);
  displayCol(ctx, DIGIT_SPACE);
  displayNum(ctx, h/10);
  displayCol(ctx, DIGIT_SPACE);
  displayNum(ctx, h%10);
  displayCol(ctx, DIGIT_SPACE);
  displayNum(ctx, m/10);
  displayCol(ctx, DIGIT_SPACE);
  displayNum(ctx, m%10);
  displayCol(ctx, DIGIT_SPACE);
  displayCol(ctx, DIGIT_SPACE);
}

void handle_init(AppContextRef ctx) {
  srand(time(NULL));//seed randmoness!
  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);
  text_layer_init(&dateLayer, GRect(0, 0, 144, 50));
  text_layer_set_text_alignment(&dateLayer, GTextAlignmentCenter);
  text_layer_set_font(&dateLayer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_background_color(&dateLayer, GColorBlack);
  text_layer_set_text_color(&dateLayer, GColorWhite);
  text_layer_set_overflow_mode(&dateLayer, GTextOverflowModeWordWrap);
  layer_add_child(&window.layer, &dateLayer.layer);

  text_layer_init(&busLayer, GRect(0, 93, 144, 75));
  text_layer_set_text_alignment(&busLayer, GTextAlignmentCenter);
  text_layer_set_font(&busLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_background_color(&busLayer, GColorBlack);
  text_layer_set_text_color(&busLayer, GColorWhite);
  text_layer_set_overflow_mode(&busLayer, GTextOverflowModeWordWrap);
  layer_add_child(&window.layer, &busLayer.layer);

// Init the layer for the display
  layer_init(&display_layer, window.layer.frame);
  display_layer.update_proc = &display_layer_update_callback;
  layer_add_child(&window.layer, &display_layer);

  adjX = ((int) ( (144-((bitRect.size.w+BIT_SPACING)*ROW_WIDTH))/2)) -2;//change the -2 if needed
  adjY = ((int) ( ((168-YSTART)-((bitRect.size.h+BIT_SPACING)*COL_HEIGHT)  )/2)) - YSHORT/2;

  handle_minute_tick(ctx, NULL);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    },

    .messaging_info = {
      .buffer_sizes = {
        .inbound = 64, // inbound buffer size in bytes
        .outbound = 16, // outbound buffer size in bytes
      },
      .default_callbacks.callbacks = {
        //.out_sent = my_out_sent_handler,
        //.out_failed = my_out_fail_handler,
        .in_received = my_in_rcv_handler,
        //.in_dropped = my_in_drp_handler,
      },
    },

  };
  app_event_loop(params, &handlers);
}
