var liveconfig = require('./liveconfig');

var Clay = require('pebble-clay');
var clayConfig = require('./config.json');
var customClay = require('./custom-clay');
var clay = new Clay(clayConfig, customClay, {autoHandleEvents: false});

var GenericWeather = require('pebble-generic-weather');
var genericWeather = new GenericWeather();

var GeocodeMapquest = require('pebble-geocode-mapquest');
var geocodeMapquest = new GeocodeMapquest();

Pebble.addEventListener('appmessage', function(e) {
    genericWeather.appMessageHandler(e);
    geocodeMapquest.appMessageHandler(e);
});

Pebble.addEventListener('showConfiguration', function(e) {
  var settings = JSON.parse(localStorage.getItem('clay-settings')) || {};
  liveconfig.connect(Pebble.getAccountToken(), function(msg) {
    if (msg.type == 'partial') {
        settings[msg.id] = msg.value;
    } else {
        settings = msg.config;
    }
    Pebble.sendAppMessage(Clay.prepareSettingsForAppMessage(settings));
  });

  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (e && !e.response) { return; }

  // Send settings to Pebble watchapp
  Pebble.sendAppMessage(clay.getSettings(e.response), function(e) {
    liveconfig.close();
    console.log('Sent config data to Pebble');
  }, function() {
    liveconfig.close();
    console.log('Failed to send config data!');
    console.log(JSON.stringify(e));
  });
});

Pebble.addEventListener('ready', function() {
    Pebble.sendAppMessage({ 'APP_READY' : 1 });
});
