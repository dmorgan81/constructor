var liveconfig = module.exports;

var _websocket;

liveconfig.connect = function(uuid, onChange) {
    if (!uuid) {
        throw new Error("Liveconfig requires a UUID!");
    }
    _websocket = new WebSocket("wss://liveconfig.fletchto99.com/receive/"+uuid+"/"+Pebble.getWatchToken());

    _websocket.onmessage = function(message) {
        onChange(JSON.parse(message.data));
    };
};

liveconfig.prepareURL = function(uuid, url) {
    if (url.lastIndexOf('?') > 0) {
        return url.substr(0, url.lastIndexOf('?')) +'?lc_uuid=' + uuid + '&lc_watchtoken=' + Pebble.getWatchToken() + '&' + url.substr(url.lastIndexOf('?') + 1);
    } else {
        return  url+'?lc_uuid=' + uuid + '&lc_watchtoken=' + Pebble.getWatchToken();
    }
};

liveconfig.close = function() {
    _websocket.close();
}
