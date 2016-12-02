var liveconfig = require('./liveconfig');

var Clay = require('pebble-clay');
var clayConfig = require('./config.json');
var customClay = require('./custom-clay');
var clay = new Clay(clayConfig, customClay, {autoHandleEvents: false});

Pebble.addEventListener('showConfiguration', function(e) {
  var settings = JSON.parse(localStorage.getItem('clay-settings')) || {};
  liveconfig.connect(Pebble.getAccountToken(), function(id, value) {
    settings[id] = value;
    Pebble.sendAppMessage(Clay.prepareSettingsForAppMessage(settings));
  });

  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (e && !e.response) { return; }

  // Send settings to Pebble watchapp
  Pebble.sendAppMessage(clay.getSettings(e.response), function(e) {
    console.log('Sent config data to Pebble');
  }, function() {
    console.log('Failed to send config data!');
    console.log(JSON.stringify(e));
  });
});
