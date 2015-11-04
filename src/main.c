#include <pebble.h>
#include "weather.c"

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_TIMESTAMP 2
#define KEY_TIMEDATE 3
#define KEY_TEMP 4
#define KEY_HTTPERROR 5
  
  
// working with bigSimple font 110
  // seperate time to min & hour , date, weather
  // added battery meter
  // big_font_simple '/' ?
  //  weather text works ok
  // weather backgorund changes color
  // waether icon works from https://github.com/Niknam/futura-weather/tree/master/resources/src/images
  // updates icon at same time as bkground color
  // added three icon layers showing dferent color & icons
  // moved battery to under mins, adjusted h,m & date 
 // remove leading zero
  // added function -2Temp + 70 = Battery
  // added color scale as function
   // - fetch weather as code   // -->procedure
  // - assign correct symbol
 //  mystrtok to split string
  // split_string fn works
  // weather code & temp working from array
  // add background waether layer with time
    // place fn prottypes in .h files
  // move some code to other files
   // add more colors to temp scale
    // fetch date from weather and double check data  
    // fetch weather compare to current forecast
// temp out of range ok
// outside_range function

// --to do--
  // fine adjust pos of date & time - use overlay to measure
  // add moon symbol for nighttime
  // optimize battery usauge
  // bt symbol & vibrate on disconect
  // conf page
// #define LEFT RIGHT MIDDLE to use in array??
// seperate update_time --> update_time & ipdate_date & update_hour? ---> update_display fn (requisite)
//timestyle js retry weather API
// bitmap_layer_set_background_color(icon_right_layer, set to clear explcity?

// problems
// setting text to clear doesnt work
// split string function dont work
// no wifi panic! ?? works sometimes?
// how is waether fetch when app started?

// simply_light ui_weather_update()  struct State  ui.state.error = FETCH_ERROR;  handle_failed_message
// update_weather() - move getch array to global scope??

  
//static  int temp_temp = 0;
static BitmapLayer *icon_left_layer,*icon_middle_layer,*icon_right_layer;
static  GBitmap *icon_bitmap;

static Window *s_main_window;
static TextLayer *s_hour_layer,*s_min_layer,*s_date_layer;
static TextLayer *s_weather_left_layer,*s_weather_middle_layer,*s_weather_right_layer,*s_weather_fail_layer;
static Layer *s_battery_layer;

static GFont s_hour_font,s_min_font,s_date_font,s_weather_font,s_weather_fail_font;
static int s_battery_level;
static int weather_fail = 0;
bool weather_error = false;

static void battery_callback(BatteryChargeState state) {
  // Record the new battery level
  s_battery_level = state.charge_percent;
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, set_colour(battery_to_temp((int)s_battery_level))); 
  graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, bounds.size.h), 8, GCornersAll);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer_hour[] = "00";
  static char buffer_min[] = "00";
  static char buffer_date[] = "00/00";

   strftime(buffer_min, sizeof(buffer_min), "%M", tick_time);
    strftime(buffer_hour, sizeof(buffer_hour), "%H", tick_time);
   // APP_LOG(APP_LOG_LEVEL_INFO, "leading digit ASCII: %d", buffer_hour[0]);
  if('0' == buffer_hour[0]) {
    memmove(buffer_hour, &buffer_hour[1], sizeof(buffer_hour)-1);
  }
 // APP_LOG(APP_LOG_LEVEL_INFO, "leading digit ASCII: %d", buffer_hour[0]);
strftime(buffer_date, sizeof(buffer_date), "%m/%d", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_hour_layer, buffer_hour);
  text_layer_set_text(s_min_layer, buffer_min);
  text_layer_set_text(s_date_layer, buffer_date);
}

static void main_window_load(Window *window) {
   // Create battery meter Layer
s_battery_layer = layer_create(GRect(84, 66, 35, 10));
layer_set_update_proc(s_battery_layer, battery_update_proc);
  
  // Create hour TextLayer
   s_hour_layer = text_layer_create(GRect(-10, -13, 80, 110));
//  s_hour_layer = text_layer_create(GRect(0, 0, 144, 168));
//  text_layer_set_background_color(s_hour_layer, GColorCyan);
  text_layer_set_background_color(s_hour_layer, GColorClear);
  text_layer_set_text_color(s_hour_layer, GColorBlack);
  text_layer_set_text(s_hour_layer, "00");
  
  // Update Font Size Layer
  // added to help getsize of font for debug
static char s_size_text[256] = "size";
 // hour
  GSize textSize = text_layer_get_content_size(s_hour_layer);
  snprintf(s_size_text, sizeof(s_size_text), "H: %d W: %d", textSize.h, textSize.w);
   APP_LOG(APP_LOG_LEVEL_INFO, "hour: %s", s_size_text);
 
  s_min_layer = text_layer_create(GRect(85,2, 54, 60));
 // text_layer_set_background_color(s_min_layer, GColorCyan);
 text_layer_set_background_color(s_min_layer, GColorClear);
  text_layer_set_text_color(s_min_layer, GColorBlack);
  text_layer_set_text(s_min_layer, "00");
   
   s_date_layer = text_layer_create(GRect(83, 83, 54, 30));
// text_layer_set_background_color(s_date_layer, GColorOrange);
 text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_text(s_date_layer, "00/00");
 
  // weather
  s_weather_left_layer = text_layer_create(GRect(0, 120, 48, 48));
  text_layer_set_background_color(s_weather_left_layer , GColorLightGray);
  text_layer_set_text(s_weather_left_layer, "+0H" );
  s_weather_middle_layer = text_layer_create(GRect(48, 120, 48, 48));
  text_layer_set_background_color(s_weather_middle_layer, GColorLightGray);
  text_layer_set_text(s_weather_middle_layer, "+6H" );
  s_weather_right_layer = text_layer_create(GRect(96, 120, 48, 48));
  text_layer_set_background_color(s_weather_right_layer, GColorLightGray);
  text_layer_set_text(s_weather_right_layer, "+12H" );
  s_weather_fail_layer = text_layer_create(GRect(0, 120, 144, 48));
  text_layer_set_background_color(s_weather_fail_layer, GColorClear);
  text_layer_set_text_color(s_weather_fail_layer, GColorClear);
  

  //Create GFont
  s_hour_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SIMPLE_BIG_110));
 // not so good...
  //  s_hour_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WTIT_80));
  s_min_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SIMPLE_BIG_60));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_14));
  s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DROID_SANS_18));
  s_weather_fail_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_HHGTTG_20));

   // icon_left_layer
icon_left_layer = bitmap_layer_create(GRect(0, 120, 48, 48));
  bitmap_layer_set_compositing_mode(icon_left_layer, GCompOpClear);

  // icon_middle_layer
icon_middle_layer = bitmap_layer_create(GRect(48, 120, 48, 48));
  bitmap_layer_set_compositing_mode(icon_middle_layer, GCompOpClear);
 
    // icon_right_layer
icon_right_layer = bitmap_layer_create(GRect(96, 120, 48, 48));
  bitmap_layer_set_compositing_mode(icon_right_layer, GCompOpClear);
  
  //Apply to TextLayer
  text_layer_set_font(s_hour_layer, s_hour_font);
  text_layer_set_font(s_min_layer, s_min_font);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_font(s_weather_left_layer, s_weather_font);
  text_layer_set_font(s_weather_middle_layer, s_weather_font);
  text_layer_set_font(s_weather_right_layer, s_weather_font);
  text_layer_set_font(s_weather_fail_layer, s_weather_fail_font);
// text_layer_set_text_alignment(s_hour_layer, GTextAlignmentRight);
   text_layer_set_text_alignment(s_hour_layer, GTextAlignmentRight);
  text_layer_set_text_alignment(s_min_layer, GTextAlignmentLeft);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hour_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_min_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_left_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_middle_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_right_layer));
  layer_add_child(window_get_root_layer(window), s_battery_layer);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(icon_left_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(icon_middle_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(icon_right_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_fail_layer));
  
  // Make sure the time is displayed from the start
  update_time();
    // Update meter
  layer_mark_dirty(s_battery_layer);
}

static void main_window_unload(Window *window) {
  //Unload GFont
  fonts_unload_custom_font(s_hour_font);
  fonts_unload_custom_font(s_min_font);
  fonts_unload_custom_font(s_date_font);
  fonts_unload_custom_font(s_weather_font);
  fonts_unload_custom_font(s_weather_fail_font);
  
  // Destroy TextLayer
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_min_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_weather_left_layer);
  text_layer_destroy(s_weather_middle_layer);
  text_layer_destroy(s_weather_right_layer);
  text_layer_destroy(s_weather_fail_layer);
  layer_destroy(s_battery_layer);
  // Destroy GBitmap
 gbitmap_destroy(icon_bitmap);

// Destroy BitmapLayer
bitmap_layer_destroy(icon_left_layer);
  bitmap_layer_destroy(icon_middle_layer);
  bitmap_layer_destroy(icon_right_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  // update_weather()  if weather_fail "dont panic" else show waether
    // text_layer_set_text(s_weather_fail_layer, "Panic" );
  }
  // every minute
    if ( weather_error) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Weather error");
    text_layer_set_background_color(s_weather_fail_layer, GColorLightGray);
    text_layer_set_text_color(s_weather_fail_layer, GColorRed);
    text_layer_set_text_alignment(s_weather_fail_layer, GTextAlignmentCenter);
    text_layer_set_text(s_weather_fail_layer, "1on't Panic" );
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, "Weather no error");
    text_layer_set_background_color(s_weather_fail_layer, GColorClear);
    text_layer_set_text_color(s_weather_fail_layer, GColorClear);
    text_layer_set_text(s_weather_fail_layer, "" );
 
  }
  
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  //static char temperature_buffer[8];
    static int temperature_buffer = 0;
   static int timestamp_buffer = 1000000000;
 // static int conditions_buffer = 100;
  static char conditions_buffer[32];
//  static char weather_layer_buffer[64];
  static char timedate_buffer[60];
    static char tempary_buffer[32];
  static char httperror_buffer[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_TEMPERATURE:
     // snprintf(temperature_buffer, sizeof(temperature_buffer), "%d", (int)t->value->int32);
      temperature_buffer = (int)t->value->int32;
      break;
    case KEY_CONDITIONS:
     //conditions_buffer = (int)t->value->int32;
      snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
      break;
    case KEY_TIMESTAMP:
        timestamp_buffer = (int)t->value->int32;
      break;
    case KEY_TIMEDATE:
        snprintf(timedate_buffer, sizeof(timedate_buffer), "%s", t->value->cstring);
      break;
      case KEY_TEMP:
        snprintf(tempary_buffer, sizeof(tempary_buffer), "%s", t->value->cstring); 
      break;
       case KEY_HTTPERROR:
        snprintf(httperror_buffer, sizeof(httperror_buffer), "%s", t->value->cstring); 
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // Assemble full string and display
   APP_LOG(APP_LOG_LEVEL_INFO, "timedate,timestamp cond temp %s %d %s %d", timedate_buffer,timestamp_buffer, conditions_buffer,temperature_buffer);
   APP_LOG(APP_LOG_LEVEL_INFO, "http error:  %s", httperror_buffer);
 // APP_LOG(APP_LOG_LEVEL_INFO, "temperatureList, %s", tempary_buffer);
    
  static char delim[] = "|";
  char *TempArray[3]={"777","888","999"};
  split_string(TempArray,delim,tempary_buffer);
  char *ConditionsArray[3]={"1000","2000","3000"};
  split_string(ConditionsArray,delim,conditions_buffer);
 char *TimeDateArray[3]={"2000-00-00 00:00:00","2001-00-00 00:00:00","2002-00-00 00:00:00"};
  split_string(TimeDateArray,delim,timedate_buffer);
//  int i;
 //   for (i=0;i<3; ++i) {
 //   APP_LOG(APP_LOG_LEVEL_ERROR, "after fn string split2: %d <%s> %s %s", i,TempArray[i],ConditionsArray[i],TimeDateArray[i]);
 // }
  
  bitmap_layer_set_background_color(icon_left_layer, set_colour(atoi(TempArray[0])));
   bitmap_layer_set_background_color(icon_middle_layer, set_colour(atoi(TempArray[1])));
  bitmap_layer_set_background_color(icon_right_layer, set_colour(atoi(TempArray[2])));
 
  icon_bitmap=set_weather_icon(atoi(ConditionsArray[0]));
  bitmap_layer_set_bitmap(icon_left_layer, icon_bitmap);
 icon_bitmap=set_weather_icon(atoi(ConditionsArray[1]));
   bitmap_layer_set_bitmap(icon_middle_layer, icon_bitmap);
   icon_bitmap=set_weather_icon(atoi(ConditionsArray[2]));
   bitmap_layer_set_bitmap(icon_right_layer, icon_bitmap);

   if ( outsideRange( atoi(TempArray[0]),-50,60) ||
       outsideRange( atoi(TempArray[1]),-50,60) ||
       outsideRange( atoi(TempArray[2]),-50,60) ||
       outsideRange( atoi(ConditionsArray[0]),100,999) ||
       outsideRange( atoi(ConditionsArray[1]),100,999) ||
       outsideRange( atoi(ConditionsArray[2]),100,999) )
    {
    weather_error = true;
    APP_LOG(APP_LOG_LEVEL_ERROR, "Weather fail temp: %s:%s:%s",TempArray[0],TempArray[1],TempArray[2]);
    APP_LOG(APP_LOG_LEVEL_ERROR, "Weather fail cond: %s:%s:%s",ConditionsArray[0],ConditionsArray[1],ConditionsArray[2]);
  }  else {
      weather_error = false;
   }
  
 // end of fn 
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  weather_error = true;
//  text_layer_set_background_color(s_weather_fail_layer, GColorLightGray);
 //   text_layer_set_text_color(s_weather_fail_layer, GColorRed);
  //  text_layer_set_text_alignment(s_weather_fail_layer, GTextAlignmentCenter);
   // text_layer_set_text(s_weather_fail_layer, "2on't Panic" );
    APP_LOG(APP_LOG_LEVEL_ERROR, "Weather fail %d ", weather_fail);
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  weather_error = true;
//  text_layer_set_background_color(s_weather_fail_layer, GColorLightGray);
 //   text_layer_set_text_color(s_weather_fail_layer, GColorRed);
  //  text_layer_set_text_alignment(s_weather_fail_layer, GTextAlignmentCenter);
   // text_layer_set_text(s_weather_fail_layer, "3on't Panic" );
  // BT off wifi on press button
  // BT on wifi off press button
    APP_LOG(APP_LOG_LEVEL_ERROR, "Weather fail %d ", weather_fail);
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  weather_error = false;
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    // Register for battery level updates
  battery_state_service_subscribe(battery_callback);
  // Ensure battery level is displayed from the start
  battery_callback(battery_state_service_peek());
  
  // Register callbacks
app_message_register_inbox_received(inbox_received_callback);
app_message_register_inbox_dropped(inbox_dropped_callback);
app_message_register_outbox_failed(outbox_failed_callback);
app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  
// when app is loaded
   if ( weather_error) {
    APP_LOG(APP_LOG_LEVEL_ERROR, ">>Weather error");
       text_layer_set_background_color(s_weather_fail_layer, GColorLightGray);
    text_layer_set_text_color(s_weather_fail_layer, GColorRed);
    text_layer_set_text_alignment(s_weather_fail_layer, GTextAlignmentCenter);
    text_layer_set_text(s_weather_fail_layer, "2on't Panic" );
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, ">>Weather no error");
       text_layer_set_background_color(s_weather_fail_layer, GColorClear);
    text_layer_set_text_color(s_weather_fail_layer, GColorClear);
    text_layer_set_text(s_weather_fail_layer, "" );
  }
  
  
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  
 // char value[5] = "5";
//int a = atoi(value);
 // APP_LOG(APP_LOG_LEVEL_INFO, "atoi 5: %d", a);
  /*
   temp_temp = battery_to_temp(100);
  APP_LOG(APP_LOG_LEVEL_INFO, "batter: 100 temp: %d", temp_temp);
   temp_temp = battery_to_temp(0);
  APP_LOG(APP_LOG_LEVEL_INFO, "batter: 0 temp: %d", temp_temp);
  temp_temp = battery_to_temp(10);
  APP_LOG(APP_LOG_LEVEL_INFO, "batter: 10 temp: %d", temp_temp);
   temp_temp = battery_to_temp(90);
  APP_LOG(APP_LOG_LEVEL_INFO, "batter: 90 temp: %d", temp_temp);
   temp_temp = battery_to_temp(50);
  APP_LOG(APP_LOG_LEVEL_INFO, "batter: 50 temp: %d", temp_temp);
  */

  
  init();
  app_event_loop();
  deinit();
  

}
