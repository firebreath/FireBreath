/*global chrome*/
(function() {


    ////////////////////////////////////////
    //  This script runs in the page!     //
    ////////////////////////////////////////

    var extId = "${PLUGIN_CRX_ID}";

    var Deferred;
    if (window.FireBreathPromise) {
        Deferred = window.FireBreathPromise;
    }
    function makeDeferred(val) {
        var dfd = Deferred();
        dfd.resolve(val);
        return dfd;
    }

    var dfds = [];
    var listDfds = [];

    var wyrmholes = {};

    window.addEventListener("message", function(event) {
        // We only accept messages from ourselves
        if (event.source != window) { return; }

        if (event.data && event.data.list) {
            var listDfd = listDfds.pop();
            listDfd.resolve(event.data);
        } else if (event.data && event.data.source && event.data.source == "host" && event.data.ext == extId) {
            handleEvent(event.data);
        }
    }, false);

    function handleEvent(evt) {
        var wh;
        if (evt.port && evt.message == "Created") {
            var dfd = dfds.pop();
            wh = new Wyrmhole(evt.port);
            dfd.resolve(wh);
        } else if (evt.port && evt.message == "Destroyed") {
            wh = wyrmholes[evt.port];
            wh.destroy();
        } else if (evt.port) {
            wh = wyrmholes[evt.port];
            wh.onMessage(evt);
        }
    }

    function createWyrmhole() {
        var dfd = Deferred();
        dfds.push(dfd);
        window.postMessage({
            source: "page",
            ext: extId,
            request: "Create"
        }, "*");
        return dfd.promise;
    }

    function Wyrmhole(port) {
        var loadDfd = null,
            self=this,
            destroyed = false,
            loaded = false,
            cmdMap = {},
            inMessages = {},
            nextCmdId = 1,
            onCommandFn = null;

        wyrmholes[port] = {
            onMessage: onMessage,
            destroy: function() {
                destroyed = true;
                delete wyrmholes[port];
            }
        };

        function postCommand(msg) {
            msg.source = "page";
            msg.port = port;
            msg.ext = extId;
            window.postMessage(msg, "*");
        }
        function postMessage(msg, cmdId) {
            var type = "resp";
            if (cmdId === void 0) {
                cmdId = nextCmdId++;
                type = "cmd";
            }

            postCommand({
                cmdId: cmdId,
                type: type,
                c: 1, // This direction we don't need to split
                n: 1,
                colonyId: 0,
                msg: JSON.stringify(msg)
            });

            return cmdId;
        }

        function onMessage(msg) {
            if (msg.plugin && loadDfd && !loaded) {
                loaded = true;
                loadDfd.resolve(self);
            } else if (msg.status && msg.status == "error" && loadDfd && !loaded) {
                loadDfd.reject(new Error(msg.message));
                loadDfd = void 0;
            } else if (msg.msg) {
                // This is a message from the native message host,
                // we might need to put it back together because the host
                // is limited to 1MB of data per message
                if (!inMessages[msg.cmdId]) {
                    inMessages[msg.cmdId] = {
                        parts: Array(msg.c),
                        count: 0
                    };
                }

                var obj = inMessages[msg.cmdId];
                obj.parts[msg.n-1] = msg.msg;
                obj.count++;
                if (obj.count >= msg.c) {
                    var text = obj.parts.join('');
                    processCompleteMessage(msg, text);
                }
            }
        }
        function processCompleteMessage(msg, text) {
            var dfd = null;
            // The whole message is here!
            if (msg.type == "resp") {
                // This is a response to a message sent from the page
                dfd = cmdMap[msg.cmdId];
                if (!dfd) {
                    throw new Error("Invalid msg id!");
                }
                try {
                    dfd.resolve(JSON.parse(text));
                } catch(e) {
                    // If the response was invalid, reject with that error
                    dfd.reject(e);
                }
            } else if (onCommandFn) {
                // This is a new message sent from the host to the page
                dfd = Deferred();
                var promise = null;
                try {
                    onCommandFn(JSON.parse(text), function(res, resp) {
                        if (res == 'success') {
                            dfd.resolve(Array.prototype.slice.call(arguments));
                        } else {
                            dfd.reject(resp);
                        }
                    });
                    promise = dfd.promise;
                } catch (e) {
                    postMessage(["error", {"error": "command exception", "message": e.toString()}], msg.cmdId);
                }
                if (promise) {
                    promise.then(function(resp) {
                        // Send the return value
                        postMessage(resp, msg.cmdId);
                    }, function(e) {
                        postMessage(["error", {"error": "Javascript Exception", "message": e.toString()}], msg.cmdId);
                    });
                }
            }
        }

        self.sendMessage = function(msg, callback) {
            if (destroyed) {
                throw new Error("Wyrmhole not active");
            }
            var dfd = Deferred();
            var cmdId = postMessage(msg);
            cmdMap[cmdId] = dfd;

            dfd.promise.then(function(res) {
                if (callback) { callback.apply(null, res); }
                else { console.log("No callback for", res); }
            }, function(err) {
                if (callback) { callback("error", err); }
                else { console.log("No callback for error: ", err); }
            });
        };
        self.onMessage = function(fn) { onCommandFn = fn; };
        self.loadPlugin = function(mimetype) {
            if (loadDfd) {
                throw new Error("Plugin already loaded (or loading)");
            }
            loadDfd = Deferred();
            postCommand({
                cmd: "create",
                mimetype: mimetype
            });
            return loadDfd.promise;
        };
        self.destroy = function() {
            postCommand({
                cmd: "destroy"
            });
        };
        self.listPlugins = function() {
            var dfd = Deferred();
            listDfds.unshift(dfd);
            postCommand({"cmd": "list"});
            return dfd.promise.then(function(resp) {
                if (resp.status == "success") {
                    return resp.list;
                } else {
                    return Deferred().reject(resp.error);
                }
            });
        };
    }

    // We'll check so if this is run where there is no window it won't explode
    if (typeof window != "undefined") {
        if (!window._wyrmholes) {
            window._wyrmholes = {};
        }
        window._wyrmholes["${PLUGIN_CRX_WYRMHOLE_NAME}"] = createWyrmhole;
        if (!window.createWyrmhole) {
            window.createWyrmhole = function(name) {
                for (var cur in window._wyrmholes) {
                    if (!name || name == cur) {
                        return window._wyrmholes[cur]();
                    }
                }
            };
        }
    }

})();

