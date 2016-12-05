module.exports = function(minified) {
    var Clay = this;
    var _ = minified._;
    var $ = minified.$;
    var HTML = minified.HTML;

    Clay.on(Clay.EVENTS.AFTER_BUILD, function() {
        var connection = new WebSocket("wss://liveconfig.fletchto99.com/forward/" + Clay.meta.accountToken + "/" + Clay.meta.watchToken);
        connection.onopen = function() {
            Clay.getAllItems().map( function(item) {
                item.on('change', function() {
                    connection.send(JSON.stringify({id: this.messageKey, value: this.get()}));
                });
            });
        };

        var toggles = Clay.getItemsByType('toggle');
        toggles.forEach(function(toggle) {
            console.log(toggle.config);
            var group = toggle.config.group;
            if (!group) return;
            toggle.on('change', function() {
                var enabled = this.get();
                Clay.getItemsByGroup(group).forEach(function(item) {
                    if (item === toggle) return;
                    if (enabled) item.show();
                    else item.hide();
                });
            }).trigger('change');
        });

    });

};
