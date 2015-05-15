#include <pebble.h>

static Window *window;
static Layer *canvas=NULL;
static AppTimer *timer;
static int current_update_proc = 0;

static void update(){
	if (canvas!=NULL)
		layer_mark_dirty(canvas);
	timer = app_timer_register(1000/30, update, NULL);
}

static void update_proc_draw_pixel(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint pixel = GPoint(0, 0);
	for (int i=0; i<bounds.size.w; i++){
		for (int j=0; j<bounds.size.h; j++){
			pixel.x=i;
			pixel.y=j;
			graphics_context_set_stroke_color(ctx, rand() % 2 ? GColorWhite: GColorBlack);
			graphics_draw_pixel(ctx, pixel);
		}
	}
}

static void update_proc_frame_buffer(Layer *layer, GContext *ctx){
	GBitmap *fb = graphics_capture_frame_buffer(ctx);
	if (fb!=NULL){
	  GRect bounds=gbitmap_get_bounds(fb);
	  uint8_t *pos = gbitmap_get_data(fb);
	  for (int16_t y = bounds.origin.y; y < bounds.size.h; y++) {
		  for (int16_t x = bounds.origin.x; x < gbitmap_get_bytes_per_row(fb); x++){
			  memset(++pos, rand(), 1);
		  }
    }
	  graphics_release_frame_buffer(ctx, fb);
  }
}

static LayerUpdateProc update_procs[] = {
    update_proc_frame_buffer,
    update_proc_draw_pixel
};

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  // Create Layer
  canvas = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_add_child(window_layer, canvas);
  // Set the update_proc
  layer_set_update_proc(canvas, update_procs[current_update_proc]);
	update();
}

static void main_window_unload(Window *window) {
  // Destroy Layer
  layer_destroy(canvas);
}

static void toggle_update_proc_click_handler(ClickRecognizerRef recognizer, void *context) {
  current_update_proc = (current_update_proc + 1) % ARRAY_LENGTH(update_procs);
  layer_set_update_proc(canvas, update_procs[current_update_proc]);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, toggle_update_proc_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, toggle_update_proc_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, toggle_update_proc_click_handler);
}

static void init(void) {
  // Create main Window
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
#ifdef PBL_SDK_2
	// Get rid of the top bar
	window_set_fullscreen(window, true);
#endif
  window_stack_push(window, false);
	window_set_click_config_provider(window, click_config_provider);
}

static void deinit(void) {
  // Destroy main Window
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}