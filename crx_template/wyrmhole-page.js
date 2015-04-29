(function(fb) {
    var connectList = [];
    
    var Deferred;
    if (window.FireBreathPromise) {
        Deferred = window.FireBreathPromise;
    }
    
    function connectWyrmhole(extId, dfd, evt) {
        var port = {port: evt.port, extId: extId};
		var sink = function(sink) { 
			window.addEventListener("message", function(event) {
		        // We only accept messages from ourselves
		        if (event.source != window) { return; }
		        
				if (event.data && event.data.source == "host" && event.data.ext == extId && event.data.port == port.port) {
					if (event.data.message == "Disconnected") {
						sink({disconnect: true});
					} else {
						sink(event.data);
					}
				}
			});
		};
        dfd.resolve({port: port, sink: sink});
    }

    window.addEventListener("message", function(event) {
        // We only accept messages from ourselves
        if (event.source != window) { return; }

        if (event.data && event.data.source == "host" && event.data.port && event.data.message == "Created") {
        	for (var i = connectList.length - 1 ; i >= 0 ; i--)
        		if (connectList[i].extId == event.data.ext) {
        			connectWyrmhole(connectList[i].extId, connectList[i].dfd, event.data);
        			connectList.splice(i, 1);
        		}
        }
    }, false);

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
	}
    fb.wyrmhole.message = function(msg) {
        msg.source = "page";
        msg.port = this.port;
        msg.ext = this.extId;
        window.postMessage(msg, "*");
	}
})(typeof firebreath != 'undefined' ? firebreath : window);
