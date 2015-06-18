/*global chrome,firebreath*/
(function(fb) {
    fb.wyrmhole = {
        instances: [],
        create: createWyrmhole,
        connect: connectWyrmhole,
        message: messageWyrmhole,
        destroy: function() {}
    };

    var nativeHostApp = "${PLUGIN_CRX_NATIVEHOST_NAME}";

    var Deferred;
    if (window.FireBreathPromise) {
        Deferred = window.FireBreathPromise;
    }

    function connectWyrmhole(application) {
        var dfd = Deferred();
        var port = chrome.runtime.connectNative(application || nativeHostApp);
        var sink = function(sink) {
            port.onMessage.addListener(sink);
            port.onDisconnect.addListener(function() {
                sink({disconnect: true});
            });
        };
        dfd.resolve({port: port, sink: sink});
        return dfd.promise;
    }

    function messageWyrmhole(msg) {
        this.postMessage(msg);
    }

    function createWyrmhole(application, mimetype) {
        var dfd = Deferred();
        if (!mimetype) {
            mimetype = application;
            application = void 0;
        }
        fb.wyrmhole.connect(application).then(function(resp) {
            var wyrmhole = new Wyrmhole(resp.port, resp.sink);
            fb.wyrmhole.instances.push(wyrmhole);

            if (typeof mimetype == 'string') {
                wyrmhole.loadPlugin(mimetype).then(function() {
                    dfd.resolve.apply(dfd, arguments);
                }, function() {
                    dfd.reject.apply(dfd, arguments);
                });
            } else {
                dfd.resolve(wyrmhole);
            }
        });
        return dfd.promise;
    }

    function Wyrmhole(port, sink) {
        var loadDfd = null,
            listDfds = [],
            self = this,
            destroyed = false,
            loaded = false,
            cmdMap = {},
            inMessages = {},
            nextCmdId = 1,
            onCommandFn = null,
            onDisconnectFn = null,
            errMsg;

        sink(function(msg) {
            if (msg.plugin && loadDfd && !loaded) {
                loaded = true;
                loadDfd.resolve(self);
            } else if (msg.status && msg.status == "error" && loadDfd && !loaded) {
                loadDfd.reject(new Error(msg.message));
                loadDfd = void 0;
            } else if (msg.message && msg.message == "Error" && loadDfd && !loaded) {
                // This can happen if the port closed unexpectedly
                loadDfd.reject(new Error(msg.error));
                loadDfd = void 0;
            } else if (msg.list) {
                var listDfd = listDfds.pop();
                if (listDfd) { listDfd.resolve(msg); }
            } else if (msg.disconnect) {
                errMsg = msg.message || "Host disconnected";
                if (loadDfd) {
                    loadDfd.reject(errMsg);
                }
                Object.keys(cmdMap).forEach(function(cmdId) {
                    cmdMap[cmdId].reject(errMsg);
                });
                listDfds.forEach(function(dfd) {
                    dfd.reject(errMsg);
                });
                destroyed = true;
                if (onDisconnectFn) {
                    onDisconnectFn(errMsg);
                }
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
        });

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
                    command(["error", {"error": "command exception", "message": e.toString()}], msg.cmdId);
                }
                if (promise) {
                    promise.then(function(resp) {
                        // Send the return value
                        command(resp, msg.cmdId);
                    }, function(e) {
                        command(["error", {"error": "Javascript Exception", "message": e.toString()}], msg.cmdId);
                    });
                }
            }
        }

        function message(cmd) {
            fb.wyrmhole.message.call(port, cmd);
        }

        function command(msg, cmdId) {
            var type = "resp";
            if (cmdId === void 0) {
                cmdId = nextCmdId++;
                type = "cmd";
            }

            message({
                cmdId: cmdId,
                type: type,
                c: 1, // This direction we don't need to split
                n: 1,
                colonyId: 0,
                msg: JSON.stringify(msg)
            });

            return cmdId;
        }

        this.sendMessage = function(msg, callback) {
            var dfd = Deferred();
            if (destroyed) {
                callback(new Error("Wyrmhole not active"));
                return;
            }

            var cmdId = command(msg);
            cmdMap[cmdId] = dfd;

            dfd.promise.then(function(res) {
                if (callback) { callback.apply(null, res); }
                else { console.log("No callback for", res); }
            }, function(err) {
                if (callback) { callback("error", err); }
                else { console.log("No callback for error: ", err); }
            });
        };
        this.onDisconnect = function(fn) { onDisconnectFn = fn; };
        this.onMessage = function(fn) { onCommandFn = fn; };
        this.loadPlugin = function(mimetype) {
            if (loadDfd) {
                throw new Error("Plugin already loaded (or loading)");
            }
            loadDfd = Deferred();
            message({
                cmd: "create",
                mimetype: mimetype
            });
            return loadDfd.promise;
        };
        this.destroy = function() {
            if (destroyed) { return; }
            message({cmd: "destroy"});
            destroyed = true;
        };
        this.listPlugins = function() {
            //console.log("Listing plugins");
            var dfd = Deferred();
            if (destroyed) {
                dfd.reject(new Error("Wyrmhole not active"));
                return dfd.promise;
            }

            listDfds.unshift(dfd);
            message({"cmd": "list"});
            // This hack works because a promises/a+ compliant promise ignores a reject or resolve
            // if it has already settled.  This fixes an intermittent bug that I can't track down
            // which causes a list to *sometimes* never get responded to.
            setTimeout(function() {
                dfd.reject(new Error("Timout talking to Wyrmhole"));
            }, 2000);

            return dfd.promise.then(function(resp) {
                //console.log("Plugin list:", resp);
                if (resp.status == "success") {
                    return resp.list;
                } else {
                    return Deferred().reject(resp.error);
                }
            });
        };
    }
})(typeof firebreath != 'undefined' ? firebreath : window);
