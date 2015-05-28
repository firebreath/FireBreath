(function(fb) {
    var connectList = [];
    var ports = {};

    var Deferred;
    if (window.FireBreathPromise) {
        Deferred = window.FireBreathPromise;
    }

    function connectWyrmhole(extId, dfd, evt) {
        var port = {port: evt.port, extId: extId};
        var sink = function(sink) {
            ports[evt.port] = function(event) {
                if (event.data.message == "Disconnected") {
                    sink({disconnect: true});
                } else {
                    sink(event.data);
                }
            };
        };
        dfd.resolve({port: port, sink: sink});
    }

    function onMessage(event) {
        // We only accept messages from ourselves
        if (event.source != window) { return; }

        if (event.data && event.data.source == "host" && event.data.port) {
            if (event.data.message == "Created") {
                while (connectList.length) {
                    var cur = connectList.pop();
                    connectWyrmhole(cur.extId, cur.dfd, event.data);
                }
            } else {
                var port = event.data.port;
                if (!ports[port]) {
                    console.error("Invalid port!");
                } else {
                    ports[port](event);
                }
            }
        }
    }
    function destroyHelper() {
        Object.keys(ports).forEach(function(port) {
            ports[port]({data: {message: "Disconnected"}});
        });
        window.removeEventListener("message", onMessage);
    }
    window.addEventListener("message", onMessage, false);

    // ext is optional -- will default to the current extension
    fb.wyrmhole.connect = function(ext) {
        var dfd = Deferred();
        var extId = ext || fb.extId;
        connectList.push({extId: extId, dfd: dfd});
        window.postMessage({
            source: "page",
            ext: extId,
            request: "Create"
        }, "*");
        return dfd.promise;
    };
    fb.wyrmhole.message = function(msg) {
        msg.source = "page";
        msg.port = this.port;
        msg.ext = this.extId;
        window.postMessage(msg, "*");
    };
    fb.wyrmhole.destroy = destroyHelper;
})(typeof firebreath != 'undefined' ? firebreath : window);
