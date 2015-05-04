/*global chrome, Promise*/
(function() {

    var nextId = 1;
    var ports = {};
    var extId = chrome.runtime.id;
    var firebreathId = '${PLUGIN_CRX_WYRMHOLE_NAME}';

    window.addEventListener("message", function(event) {
        // We only accept messages from ourselves
        if (event.source != window) { return; }

        if (event.data && event.data.source && event.data.source == "page" && event.data.ext == extId) {
            handleEvent(event.data);
        }

        if (event.data && event.data.firebreath == firebreathId && event.data.callback) {
            initPage(event.data);
        }
    }, false);

    function handleEvent(evt) {
        if (evt.request == "Create") {
            createWyrmhole();
        } else if (evt.port) {
            var port = evt.port;
            delete evt.port;
            delete evt.source;
            if (ports[port]) {
                // Message from the page received, post it to the background script
                ports[port].postMessage(evt);
            } else {
                // Invalid port!
                window.postMessage({
                    source: "host",
                    port: port,
                    ext: extId,
                    message: "Error",
                    error: "Invalid port"
                }, "*");
            }
        }
    }

    function createWyrmhole() {
        var portName = "FireWyrmPort" + (nextId++);
        var port = chrome.runtime.connect(extId, {name: portName});
        ports[portName] = port;

        window.postMessage({
            source: "host",
            port: portName,
            ext: extId,
            message: "Created"
        }, "*");
        port.onMessage.addListener(function(msg) {
            // Message from the background script received, post it to the page
            msg.source = "host";
            msg.port = portName;
            msg.ext = extId;
            window.postMessage(msg, "*");
        });
        port.onDisconnect.addListener(function() {
            // The host port disconnected; tell the window
            window.postMessage({
                source: "host",
                port: portName,
                ext: extId,
                message: "Disconnected"
            }, "*");
            delete ports[portName];
        });
    }

    function initPage(evt) { //initializing firebreath object - and passing it to callback

        var scriptPromises = [
            includeScript("FireBreathPromise.js"),
            includeScript("firewyrm.js"),
            includeScript("wyrmhole.js"),
            includeScript("wyrmhole-page.js")
        ];
        Promise.all(scriptPromises).then(function(scripts) {
            injectScript("window[" + JSON.stringify(evt.callback) + "]((function() {" +
                         "var firebreath = {extId: '" + extId + "'};" +
                         scripts.join('\n') +
                         "return firebreath;" +
                         "})())");
        }, function() {
            console.warn("Could not load wyrmhole extension");
        });
    }

    function includeScript(path) { //getting javascript as text
        return new Promise(function(resolve, reject) {
            var req = new XMLHttpRequest();

            req.onreadystatechange = function () {
                if (req.readyState == 4) {
                    if (req.status >= 200 && req.status < 300) {
                        resolve(req.responseText);
                    } else {
                        reject(req);
                    }
                }
            };

            req.open("GET", chrome.extension.getURL(path));
            req.send();
        });
    }

    function injectScript(text) { //injecting javascript by text
        var s = document.createElement('script');
        s.appendChild(document.createTextNode(text));
        s.onload = function() {
            this.parentNode.removeChild(this);
        };
        (document.head||document.documentElement).appendChild(s);
    }
    document.documentElement.className += ' FB_FW_ext ${PLUGIN_NAME}';
})();

