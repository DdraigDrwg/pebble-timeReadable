#include <pebble.h>
#include "weather.h"
#include "utility.c"
  
 static GColor8 set_colour(int temp){
  GColor8 s_main_colour;
 // APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting temp: %d", temp);
  if(temp > 35){
    //error, or apocalypse.
    s_main_colour = GColorBulgarianRose;
  }else if(temp > 30){
    //red
    s_main_colour = GColorDarkCandyAppleRed;
    }else if(temp > 27){
    //
    s_main_colour = GColorRed;
    }else if(temp > 23){
    //orange
    s_main_colour = GColorOrange;
  }else if(temp > 20){
    //dark orange
    s_main_colour = GColorChromeYellow;
  }else if(temp > 17){
    //
    s_main_colour = GColorRajah;
  }else if(temp > 13){
    //yellow
    s_main_colour = GColorYellow;
  }else if(temp > 10){
    //yellow
    s_main_colour = GColorIcterine;
  }else if(temp > 7){
    //green
    s_main_colour =  GColorGreen;
  }else if(temp > 3){
    //green
    s_main_colour = GColorScreaminGreen;
     }else if(temp > 0){
    //blue
    s_main_colour = GColorMintGreen;
     }else if(temp > -5){
    //blue
    s_main_colour = GColorPictonBlue;
    }else if(temp > -10){
    //blue
    s_main_colour = GColorBlue;
    }else if(temp > -15){
    //blue
    s_main_colour = GColorDukeBlue;
  }else{
    s_main_colour = GColorOxfordBlue;
  }
 return s_main_colour;
//    APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting color: %s", s_main_colour);

} 


  static GBitmap* set_weather_icon (int conditions) {
 static GBitmap *icon_bitmap;
   if ( first_digit(conditions) == 2) {
    //thunder
    icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_THUNDER_BLACK); 
   } else if ( first_digit(conditions) == 3 || first_digit(conditions) == 5 ) {
    //rain
    icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RAIN_BLACK); 
     } else if ( first_digit(conditions) == 6 ) {
    //snow
     icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SNOW_BLACK);
    } else if ( first_digit(conditions) == 7 ) {
    //mist
     icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FOG_BLACK);
    } else if ( first_digit(conditions) == 9 ) {
    //extreme
     icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WIND_BLACK);
    } else if ( conditions == 800 ) {
    //sunny 
    icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CD40T_BLACK);
   } else if ( first_digit(conditions) == 8 ) {
    //cloudy
    icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CLOUDY_BLACK); 
  } else {
   // general weather symbol
     icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NA_BLACK); 
  }
  return icon_bitmap;
  }