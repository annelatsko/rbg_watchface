#include <pebble.h>

static Window *watch_window;
static TextLayer *clock_layer;
static GBitmap *current_rbg;
static BitmapLayer *image_layer;

static int start = 0;

static int minute = 0;
static int hour = 0;

static uint32_t IMAGE_RESOURCES[] = {
  RESOURCE_ID_IMAGE_OFFICIAL_IDENTIFIER,    
  RESOURCE_ID_IMAGE_FACE_IDENTIFIER,     
  RESOURCE_ID_IMAGE_CHAIR_IDENTIFIER,    
  RESOURCE_ID_IMAGE_PROFILE_IDENTIFIER,   
  RESOURCE_ID_IMAGE_BW_IDENTIFIER,    
  RESOURCE_ID_IMAGE_RBG_CLASSIC_IDENTIFIER,
  RESOURCE_ID_IMAGE_OFFICIAL_IDENTIFIER,    
  RESOURCE_ID_IMAGE_FACE_IDENTIFIER,     
  RESOURCE_ID_IMAGE_CHAIR_IDENTIFIER,    
  RESOURCE_ID_IMAGE_PROFILE_IDENTIFIER,   //10
  RESOURCE_ID_IMAGE_BW_IDENTIFIER,    
  RESOURCE_ID_IMAGE_RBG_CLASSIC_IDENTIFIER,
  RESOURCE_ID_IMAGE_OFFICIAL_IDENTIFIER,    
  RESOURCE_ID_IMAGE_FACE_IDENTIFIER,     
  RESOURCE_ID_IMAGE_CHAIR_IDENTIFIER,    
  RESOURCE_ID_IMAGE_PROFILE_IDENTIFIER,   
  RESOURCE_ID_IMAGE_BW_IDENTIFIER,    
  RESOURCE_ID_IMAGE_RBG_CLASSIC_IDENTIFIER,
  RESOURCE_ID_IMAGE_FACE_IDENTIFIER,
  RESOURCE_ID_IMAGE_FACE_IDENTIFIER
};

///////////////////////////////////////////////////NOTE: this needs to be before any functions in which it is called b/c it will get flagged as an "implicit call"
//updates the time and the picture as necessary
static void update_time(){
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp); //gets the local time

  // Create a buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  strftime(buffer, sizeof("00:00"), "%I:%M", tick_time); //srftime() is a formatting thing

  // Display this time on the TextLayer
  text_layer_set_text(clock_layer, buffer);
  
  //checks to see if this is the first time that the RBG watchface is being used and sets the default image
  
    
  minute = tick_time -> tm_min;
  hour = tick_time -> tm_hour;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "hour: %d", hour);
  
  if (start == 0 && minute < 9){
     current_rbg = gbitmap_create_with_resource(IMAGE_RESOURCES[hour -2]);
     bitmap_layer_set_bitmap(image_layer, current_rbg);
     start = -1;
  }
  if (start == 0 && minute >= 9){
     current_rbg = gbitmap_create_with_resource(IMAGE_RESOURCES[hour - 1]);
     bitmap_layer_set_bitmap(image_layer, current_rbg);
     start = -1;
  }

  
  if (minute == 9){
    GBitmap *previous_rbg = current_rbg;
    current_rbg = gbitmap_create_with_resource(IMAGE_RESOURCES[hour-1]);
    bitmap_layer_set_bitmap(image_layer, current_rbg);
    // delete the last shown picture or RBG
    gbitmap_destroy(previous_rbg);
  }
}

/////////////////////////////////////////////////////////////////////////NOTE: UNLOAD AND LOAD NEED TO GO BEFORE THE WINDOWHANDLERS, OR ELSE YOU'LL GET AN ERROR
//loads the window
static void main_window_load(Window *window){
  
  //sticks the clock layer in the right position
  clock_layer = text_layer_create(GRect(0, 125, 144, 50));
  text_layer_set_background_color(clock_layer, GColorBlack); //sets color of background
  text_layer_set_text_color(clock_layer, GColorClear);       //sets color of text
  text_layer_set_text(clock_layer, "00:00");                 //puts 00:00 on the screen as a format
  
  text_layer_set_font(clock_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD)); //font
  text_layer_set_text_alignment(clock_layer, GTextAlignmentCenter);                 //aligns the text in the center
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(clock_layer)); //puts the textlayer on the windowlayer
  
  //makes the layer that the bitmap containers are going to go on
   image_layer = bitmap_layer_create(GRect(0,0, 150, 125)); //creates a bitmap layer that extends from the top of the screen to the top of the box w/ the time
   layer_add_child(window_get_root_layer(watch_window), bitmap_layer_get_layer(image_layer)); //adds the bitmap layer to the window
   update_time();

}

//unloads the window
static void main_window_unload(Window *window){
  text_layer_destroy(clock_layer);
  gbitmap_destroy(current_rbg);
  
  //destroys the bitmap layer 
   bitmap_layer_destroy(image_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}
  
//initializes the watchface - REQUIRED
static void init(){
  //creates the window and assigns it to the window pointer above
  watch_window = window_create();
  
  //set handlers to deal with the stuff inside of the window
    WindowHandlers handlers = {
        .load = main_window_load,
        .unload = main_window_unload
    };
    window_set_window_handlers(watch_window, (WindowHandlers) handlers);
  
  //does the whole "register tick handler" thing
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  //pushes the window onto the stack and sets animate equal to 'true'
  window_stack_push(watch_window, true);
  
}



//stops the watchface - REQUIRED
static void deinit(){
  
  //destroys the window
  window_destroy(watch_window);
}


//main function - HELLA REQUIRED
int main(void){
  init();
  app_event_loop();
  deinit();
}