/*global chrome*/

var ports = {};
var hostName = "${PLUGIN_CRX_NATIVEHOST_NAME}";

console.log("Starting background script");

chrome.runtime.onConnect.addListener(function(scriptPort) {
    console.log("Connected!");
    var name = scriptPort.name;
    var hostPort = chrome.runtime.connectNative(hostName);
    var self = ports[name] = {
        script: scriptPort,
        host: hostPort
    };
    scriptPort.onMessage.addListener(function(msg) {
        // Message from the content script (from the page),
        // post it to the native message host
        hostPort.postMessage(msg);
    });
    hostPort.onMessage.addListener(function(msg) {
        // Message from the native message host,
        // post it to the content script (to the page)
        scriptPort.postMessage(msg);
    });
    scriptPort.onDisconnect.addListener(function() {
        // The script disconnected, so disconnect the hostPort
        hostPort.disconnect();
    });
    hostPort.onDisconnect.addListener(function() {
        // The host (native message host) disconnected, so disconnect
        // the script port. If there is an error, report it first
        if (chrome.runtime.lastError) {
            scriptPort.postMessage({error: "Disconnected", message: chrome.runtime.lastError.message});
        } else {
            scriptPort.postMessage({error: "Disconnected"});
        }
        scriptPort.disconnect();
    });
});
