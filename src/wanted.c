#include <pebble.h>

static Window *window;
static TextLayer *title_layer;
static BitmapLayer *image_layer;
static GBitmap *image;
static TextLayer *time_layer; 

// Update watchface time
static void handle_second_tick(struct tm* tick_time, TimeUnits units_changed) {

  static char time_text[] = "00:00:00 AM"; 

  strftime(time_text, sizeof(time_text), "%r %p", tick_time);
  text_layer_set_text(time_layer, time_text);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  window_set_background_color(window, GColorBlack);

  // Title layer shows the message at the top
  title_layer = text_layer_create(GRect(20, 0, 110, 28));
  text_layer_set_text_color(title_layer, GColorWhite);
  text_layer_set_background_color(title_layer, GColorClear);
  //text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_font(title_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_STENCIL_24)));


  static char title_text[] = "Wanted";
  text_layer_set_text(title_layer, title_text);

  // Load image from resources & create bitmap layer
  image = gbitmap_create_with_resource(RESOURCE_ID_HAT);
  image_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(image_layer, image);
  bitmap_layer_set_alignment(image_layer, GAlignCenter);

  // Init the text layer used to show the time
  time_layer = text_layer_create(GRect(14, 130, 144-10 /* width */, 168-54 /* height */));
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_second_tick(current_time, SECOND_UNIT);
  tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);

  layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
  layer_add_child(window_layer, text_layer_get_layer(title_layer));
  layer_add_child(window_layer, text_layer_get_layer(time_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(title_layer);
  gbitmap_destroy(image);
  bitmap_layer_destroy(image_layer);
  text_layer_destroy(time_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
