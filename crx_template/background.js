/*global chrome*/

var firebreath = {}; //global object
var ports = {};
var hostName = "${PLUGIN_CRX_NATIVEHOST_NAME}";
var mimeType = "<enter-mime-here>"; //ex: application/x-fbtestplugin

console.log("Starting background script");

/* Example of using Fire Wyrm in extension only */
//window.addEventListener("load", function() {
//  firebreath.wyrmhole.create(hostName, mimeType).then(function(wyrmhole) {
//      var wyrm = new FireWyrmJS(wyrmhole);
//      return wyrm.create(mimeType);
//  }).then(function(plugin) {
//      /* use plugin */
//  }, function() {
//      console.log('wyrmhole error');
//  });
//});

/* Example of using Fire Wyrm via webpage */
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
            console.warn("Disconnected:", chrome.runtime.lastError.message);
        } else {
            scriptPort.postMessage({error: "Disconnected"});
        }
        scriptPort.disconnect();
    });
});
