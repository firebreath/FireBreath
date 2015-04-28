(function() {
    var extId = "dlfdfkalmjbghacnadegnkmbcnpmnmfn";
    var dfds = [];
    
    var Deferred;
    if (window.FireBreathPromise) {
        Deferred = window.FireBreathPromise;
    }
    
    window.addEventListener("message", function(event) {
        // We only accept messages from ourselves
        if (event.source != window) { return; }

        if (event.data && event.data.source == "host" && event.data.ext == extId && event.data.port && event.data.message == "Created") {
            var dfd = dfds.pop();
            var port = event.data.port;
    		var sink = function(sink) { 
    			window.addEventListener("message", function(event) {
    		        // We only accept messages from ourselves
    		        if (event.source != window) { return; }
    		        
    				if (event.data && event.data.source == "host" && event.data.ext == extId && event.data.port == port) {
    					if (event.data.message == "Disconnected") {
    						sink({disconnect: true});
    					} else {
    						sink(event.data);
    					}
    				}
    			});
    		};
            dfd.resolve(port, sink);
        }
    }, false);

	window.wyrmhole.connect = function() {
	    var dfd = Deferred();
	    dfds.push(dfd);
	    window.postMessage({
	        source: "page",
	        ext: extId,
	        request: "Create"
	    }, "*");
	    return dfd.promise;
	}
	window.wyrmhole.message = function(msg) {
        msg.source = "page";
        msg.port = this;
        msg.ext = extId;
        window.postMessage(msg, "*");
	}
})();