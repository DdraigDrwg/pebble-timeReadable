// define global variables
var temperatureNow;
var conditionsNow;
var httperror = 2;

/*
// seems to work ok
var oXHR = new XMLHttpRequest();
oXHR.open("GET", "http://api.openweathermap.org/data/2.5/weather?lat=55.6&lon=13&APPID=28f3872efff1012afb9c3d415d2d14b5", true);
oXHR.onreadystatechange = function (oEvent) {  
    if (oXHR.readyState === 4) {  
        if (oXHR.status === 200) {  
          httperror = 'false';
          console.log(oXHR.responseText);
          console.log("Error 1", httperror);
        } else {  
          console.log("Error statusText", oXHR.statusText);   
        }
          }  
}; 
oXHR.send(null); 
*/


// original

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
   xhr.onload = function () {
   callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};


// doesn't work any better than original
/*
var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.open(type, url);
   xhr.onreadystatechange = function (oEvent) {
     if (xhr.readyState === 4) {  
        if (xhr.status === 200) {  
          httperror = httperror - 1;
         callback(this.responseText);
          console.log(xhr.responseText);
          } else {  
          //  httperror = 'true';
          console.log("Error statusText", xhr.statusText);  
            console.log("Error >", httperror);
        }
     }      
  };
  
  xhr.send();
  };
*/

function locationSuccess(pos) {
  // Construct URL forecast
  var urlforecast = "http://api.openweathermap.org/data/2.5/forecast?lat=" + 
  // wrong http: (replicates no wifi behaviour)
// var urlforecast = "http://ap.openweathermap.org/data/2.5/forecast?lat=" +
  pos.coords.latitude + 
      "&lon=" + 
      pos.coords.longitude + 
      "&APPID=28f3872efff1012afb9c3d415d2d14b5";
// current weather
  var urlweather = "http://api.openweathermap.org/data/2.5/weather?lat=" +
      pos.coords.latitude + 
      "&lon=" + 
      pos.coords.longitude + 
      "&APPID=28f3872efff1012afb9c3d415d2d14b5";
      // wrong appid:
      //    "&APPID=28f3872efff1012afb9c3d415d2d14b";
  
 // current weather 
  xhrRequest(urlweather, 'GET', 
    function(responseText) {
     // responseText contains a JSON object with weather info
      var jsonWeather = JSON.parse(responseText);

      // Temperature in Kelvin requires adjustment
     temperatureNow = Math.round(jsonWeather.main.temp - 273.15);
      console.log("temperatureNow is " + temperatureNow);

      // Conditions
       conditionsNow = jsonWeather.weather[0].id;      
      console.log("conditionsNow are " + conditionsNow);

    }      
  );
  
  // Send request to OpenWeatherMap for forecast
  xhrRequest(urlforecast, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with weather info
      var jsonForecast = JSON.parse(responseText);

      // Temperature in Kelvin requires adjustment
      var temperatureForecast = Math.round(jsonForecast.list[2].main.temp - 273.15)+"|"+Math.round(jsonForecast.list[4].main.temp - 273.15);
      console.log("Temperature is " + temperatureForecast);
      var temperature = Math.round(jsonForecast.list[0].main.temp - 273.15);
      console.log("Temperature Forecast for now is " + temperature);
      var temperatureList = temperatureNow + "|" + temperatureForecast;
      console.log("temperatureList is " + temperatureList);

      // Conditions
    //  var conditions = json.list[0].weather[0].id; 
      var conditionsForecast = jsonForecast.list[2].weather[0].id + "|" + jsonForecast.list[4].weather[0].id;
     console.log("conditions is " + conditionsForecast);
      var conditions = conditionsNow + "|" + conditionsForecast;
      console.log("conditions is " + conditions);
 
      // timestamp
      //var timestamp = json.list[0].dt; 
      var timestamp = jsonForecast.list[0].dt + "|" + jsonForecast.list[2].dt + "|" + jsonForecast.list[4].dt;
      console.log("timestamp is " + timestamp);
          
      // timedate
      var timedate = jsonForecast.list[0].dt_txt + "|" + jsonForecast.list[2].dt_txt+ "|" + jsonForecast.list[4].dt_txt;
      console.log("timedate is " + timedate);
      console.log("httperror is " + httperror);
      // Assemble dictionary using our keys
      var dictionary = {
        "KEY_TEMPERATURE": temperature,
        "KEY_CONDITIONS": conditions,
        "KEY_TIMESTAMP": timestamp, 
        "KEY_TIMEDATE": timedate,
        "KEY_TEMP": temperatureList,
        "KEY_HTTPERROR": httperror
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
    }      
  );
}

function locationError(err) {
  console.log("Error requesting location!");
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

    // Get the initial weather
    getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getWeather();
  }                     
);
