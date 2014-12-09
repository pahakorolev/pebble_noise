#include <pebble.h>

static Window *window;
static Layer *canvas=NULL;
static AppTimer *timer;

static void update(){
	if (canvas!=NULL)
		layer_mark_dirty(canvas);
}

static void canvas_update_proc(Layer *this_layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(this_layer);
  GPoint pixel = GPoint(0, 0);
	for (int i=0; i<bounds.size.w; i++){
		for (int j=0; j<bounds.size.h; j++){
			pixel.x=i;
			pixel.y=j;
			graphics_context_set_stroke_color(ctx, rand() % 2 ? GColorWhite: GColorBlack);
			graphics_draw_pixel(ctx, pixel);
		}
	}
	timer = app_timer_register(20, update, NULL);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  // Create Layer
  canvas = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_add_child(window_layer, canvas);
  // Set the update_proc
  layer_set_update_proc(canvas, canvas_update_proc);
}

static void main_window_unload(Window *window) {
  // Destroy Layer
  layer_destroy(canvas);
}

static void init(void) {
  // Create main Window
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
	// Get rid of the top bar
	window_set_fullscreen(window, true);
  window_stack_push(window, false);
	//timer = app_timer_register(100, update, NULL);
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