(function e(t,n,r){function s(o,u){if(!n[o]){if(!t[o]){var a=typeof require=="function"&&require;if(!u&&a)return a(o,!0);if(i)return i(o,!0);var f=new Error("Cannot find module '"+o+"'");throw f.code="MODULE_NOT_FOUND",f}var l=n[o]={exports:{}};t[o][0].call(l.exports,function(e){var n=t[o][1][e];return s(n?n:e)},l,l.exports,e,t,n,r)}return n[o].exports}var i=typeof require=="function"&&require;for(var o=0;o<r.length;o++)s(r[o]);return s})({1:[function(require,module,exports){
/*
 * base64-arraybuffer
 * https://github.com/niklasvh/base64-arraybuffer
 *
 * Copyright (c) 2012 Niklas von Hertzen
 * Licensed under the MIT license.
 */
(function(chars){
  "use strict";

  exports.encode = function(arraybuffer) {
    var bytes = new Uint8Array(arraybuffer),
    i, len = bytes.length, base64 = "";

    for (i = 0; i < len; i+=3) {
      base64 += chars[bytes[i] >> 2];
      base64 += chars[((bytes[i] & 3) << 4) | (bytes[i + 1] >> 4)];
      base64 += chars[((bytes[i + 1] & 15) << 2) | (bytes[i + 2] >> 6)];
      base64 += chars[bytes[i + 2] & 63];
    }

    if ((len % 3) === 2) {
      base64 = base64.substring(0, base64.length - 1) + "=";
    } else if (len % 3 === 1) {
      base64 = base64.substring(0, base64.length - 2) + "==";
    }

    return base64;
  };

  exports.decode =  function(base64) {
    var bufferLength = base64.length * 0.75,
    len = base64.length, i, p = 0,
    encoded1, encoded2, encoded3, encoded4;

    if (base64[base64.length - 1] === "=") {
      bufferLength--;
      if (base64[base64.length - 2] === "=") {
        bufferLength--;
      }
    }

    var arraybuffer = new ArrayBuffer(bufferLength),
    bytes = new Uint8Array(arraybuffer);

    for (i = 0; i < len; i+=4) {
      encoded1 = chars.indexOf(base64[i]);
      encoded2 = chars.indexOf(base64[i+1]);
      encoded3 = chars.indexOf(base64[i+2]);
      encoded4 = chars.indexOf(base64[i+3]);

      bytes[p++] = (encoded1 << 2) | (encoded2 >> 4);
      bytes[p++] = ((encoded2 & 15) << 4) | (encoded3 >> 2);
      bytes[p++] = ((encoded3 & 3) << 6) | (encoded4 & 63);
    }

    return arraybuffer;
  };
})("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

},{}],2:[function(require,module,exports){
(function() {
if (this.FireBreathPromise) { return; }
var global=this;
function isObject(obj) { var type = typeof obj; return type === 'function' || type === 'object' && !!obj; }
var toString = Object.prototype.toString;
// Shamelessly borrowed from underscore.js
var isFunction = function (obj) { return toString.call(obj) === '[object Function]'; };
// Optimize `isFunction` if appropriate. Work around an IE 11 bug. Pulled from underscore.js
if (typeof /./ !== 'function') {
    isFunction = function(obj) { return typeof obj == 'function' || false; };
}
function defer(fn, arg) { setTimeout(function() { fn(arg); }, 0); }

var STATES = { PEND: 1, PROMISE: 2, RESOLVE: 3, REJECT: 4 };

function DeferredObject(name) {
    var self=this;
    var fulfillHandlers = [];
    var rejectHandlers = [];
    var state = STATES.PEND;
    var value;

    self.promise = {
        then: function(onFulfilled, onRejected) {
            var promise2 = new DeferredObject(name + "-then");
            function makeCallback(cbFunc, type) {
                // Return a wrapper for the provided callback function
                return function handleCallback(value) {
                    try {
                        // If it's not a function, we still need to pass it through to promise2
                        if (isFunction(cbFunc)) {
                            var x = cbFunc(value);
                            promise2.resolve(x);
                        } else {
                            promise2[type](value);
                        }
                    } catch (e) {
                        // In case of exception, reject
                        promise2.reject(e);
                    }
                };
            }
            if (state === STATES.RESOLVE) {
                defer(function() { makeCallback(onFulfilled, "resolve")(value); });
            } else if (state === STATES.REJECT) {
                defer(function() { makeCallback(onRejected, "reject")(value); });
            } else {
                fulfillHandlers.push(makeCallback(onFulfilled, "resolve"));
                rejectHandlers.push(makeCallback(onRejected, "reject"));
            }
            return promise2.promise;
        },
        name: name
    };

    function getThen(x) {
        try {
            return isObject(x) ? x.then : null;
        } catch (e) {
            self.reject(e);
            return false;
        }
    }
    self.resolve = function (x, force) {
        // Check for invalid calls
        if (state === STATES.RESOLVE || state === STATES.REJECT) { return; }
        else if (x === self || x === self.promise) {
            return self.reject(new TypeError());
        } else if (state !== STATES.PEND && !force) {
            return; // ignore multiple calls
        }

        // If getting then throws, getThen will call reject
        var then = getThen(x);
        if (then === false) { return; }

        // If then is a function, treat it accordingly
        if (isFunction(then)) {
            var scope_settled = false;
            var resolve = function(x) {
                if (scope_settled) { return; }
                scope_settled = true;
                self.resolve(x, true);
            }, reject = function(x) {
                if (scope_settled) { return; }
                scope_settled = true;
                self.reject(x, true);
            };
            try {
                state = STATES.PROMISE;
                then.call(x, resolve, reject);
            } catch (e) {
                if (!scope_settled) {
                    state = STATES.PEND;
                    self.reject(e);
                }
            }
        } else {
            // For any other value, do a normal resolve
            value = x;
            state = STATES.RESOLVE;
            for (var i = 0; i < fulfillHandlers.length; ++i) {
                defer(fulfillHandlers[i], value);
            }
        }
    };
    self.reject = function(x) {
        // Ignore the call if we're already settled, otherwise reject
        if (state === STATES.RESOLVE || state === STATES.REJECT) { return; }
        else if (x === self || x === self.promise) {
            return self.reject(new TypeError());
        }
        value = x;
        state = STATES.REJECT;
        for (var i = 0; i < rejectHandlers.length; ++i) {
            defer(rejectHandlers[i], value);
        }
    };
}
function makeDeferred(name) {
    return new DeferredObject(name);
}
this.FireBreathPromise = makeDeferred;
}).call((typeof module == 'object' && module.exports) ? module.exports : this);

},{}],3:[function(require,module,exports){
(function (global){
(function(globalScope) {
    var tools = require('./tools');

    module.exports = function(params) {
        params = params || {};
        var browser = {};
        Object.defineProperties(browser, {
            'eval': { value: evalFn, enumerable: true },
            getDocument: { value: getDocument, enumerable: true },
            getWindow: { value: getWindow, enumerable: true },
            invokeWithDelay: { value: invokeWithDelay, enumerable: true },
            readArray: { value: readArray, enumerable: true },
            readObject: { value: readObject, enumerable: true },
        });
        return browser;


        function evalFn(str) {
            try {
                return eval(str); // jshint ignore:line
            } catch(error) {
                var ret = { $type: 'error', data: { error: 'exception thrown', message: error && error.message }};
                if (error.stack) { ret.stack = error.stack; }
                return ret;
            }
        }

        function invokeWithDelay(delay, obj, args, fname) {
            var fnToCall = fname ? (obj && obj[fname]) : obj;
            if (!(tools.isNumber(delay) && tools.isFunction(fnToCall) && tools.isArray(args))) {
                return { $type: 'error', data: { error: 'invalid parameters', message: 'Must provide at least delay (Number), obj (Function or Object), and args (Array)'}};
            }

            var releaseWyrmlings = tools.retainAllWyrmlings(args);
            setTimeout(function() {
                fnToCall.apply(null, args);
                releaseWyrmlings();
            }, delay);

            return null;
        }

        function getDocument() { return globalScope.document; }

        function getWindow() { return globalScope.window; }

        function readArray(arr) {
            if (!arr) {
                return { $type: 'error', data: { error: 'invalid object', message: 'The object does not exist' }};
            }
            if (!tools.isArray(arr)) {
                return { $type: 'error', data: { error: 'invalid object', message: 'Object is not an array' }};
            }
            // special type that will send the object as value, but any of its top-level
            // items are subject to being sent as references -- no nesting
            return { $type: 'one-level', data: arr };
        }

        function readObject(obj) {
            if (!obj) {
                return { $type: 'error', data: { error: 'invalid object', message: 'The object does not exist' }};
            }
            if (!tools.isObject(obj)) {
                return { $type: 'error', data: { error: 'invalid object', message: 'Object is not a plain object' }};
            }
            // special type that will send the object as value, but any of its top-level
            // items are subject to being sent as references -- no nesting
            return { $type: 'one-level', data: obj };
        }
    };
}(typeof(global) !== 'undefined' ? global : this));

}).call(this,typeof global !== "undefined" ? global : typeof self !== "undefined" ? self : typeof window !== "undefined" ? window : {})
},{"./tools":6}],4:[function(require,module,exports){
(function (global){
/* global toString */
(function(globalScope) {
    /**
     * Provides a slightly-decorated FBPromise
     */
    var fbpromise = require('fbpromise');
    var Deferred = globalScope && globalScope.FireBreathPromise || fbpromise.FireBreathPromise;

    module.exports = Deferred;

    Deferred.when = function(val) {
        var dfd = Deferred();
        dfd.resolve(val);
        return dfd.promise;
    };

    Deferred.reject = function(error) {
        var dfd = Deferred();
        dfd.reject(error);
        return dfd.promise;
    };

    /**
     * Recursively turns an array (or object) of promises into a promise for an array or object.
     * If any of the promises gets rejected, the whole thing is rejected immediately.
     * If something other than an object or array is provided, it is returned untouched.
     *
     * @param promises {Array|Object} an array|object (or promise for an array|object)
     *   of values (or promises for values)
     * @returns {promise} a promise for an array of the corresponding values
     */
    // Inspired by Q, concept by Mark Miller
    // http://wiki.ecmascript.org/doku.php?id=strawman:concurrency&rev=1308776521#allfulfilled
    Deferred.all = function(promises) {
        return Deferred.when(promises).then(function(promises) {
            if (!(isArray(promises) || isObject(promises))) { return promises; }
            var resolved = isArray(promises) ? [] : {};
            var pendingCount = 0;
            var dfd = Deferred();
            for (var prop in promises) {
                if (promises.hasOwnProperty(prop)) {
                    resolved[prop] = (void 0); // just to try preserving order of insertion
                    pendingCount++;
                    Deferred.all(promises[prop]).then(thenFn(prop), failFn);
                }
            }

            function thenFn(prop) {
                return function(val) {
                    pendingCount--;
                    resolved[prop] = val;
                    resolveIfDone();
                };
            }
            function failFn(error) { dfd.reject(error); }
            function resolveIfDone() {
                if (pendingCount === 0) {
                    dfd.resolve(resolved);
                }
            }
            resolveIfDone();
            return dfd.promise;
        });
    };

    function isArray(val) {
        return Array.isArray ? Array.isArray(val) : toString.call(val) === '[object Array]';
    }
    function isObject(val) {
        // match plain objects, not special things like ArrayBuffer
        return val && toString.call(val) === '[object Object]';
    }

    // Converts a function that takes a callback as the last argument to a function that returns a
    // deferred object that is resolved to the callback value.
    Deferred.fn = function(obj, method) {
        return function() {
            var args = Array.prototype.slice.call(arguments, 0);
            var dfd = Deferred();
            var callback = function(status, resp) {
                if (status === 'success') { dfd.resolve(resp); }
                else { dfd.reject(resp); }
            };
            args.push(callback);
            obj[method].apply(obj, args);
            return dfd.promise;
        };
    };

    Deferred.always = function(thennable, fn) {
        if (!(thennable && thennable.then && fn)) { return; }
        return thennable.then(function (success){
            fn();
            return success;
        }, function(failure) {
            fn();
            return Deferred.reject(failure);
        });
    };

    return Deferred;
}(typeof(global) !== 'undefined' ? global : this));

}).call(this,typeof global !== "undefined" ? global : typeof self !== "undefined" ? self : typeof window !== "undefined" ? window : {})
},{"fbpromise":2}],5:[function(require,module,exports){
(function (global){
(function(globalScope) {
'use strict';
var Deferred = require('./deferred');
var tools = require('./tools');
var browser = require('./browser');
FireWyrmJS.asVal = tools.asVal;

module.exports = globalScope.FireWyrmJS = FireWyrmJS;

function FireWyrmJS(wyrmhole) {
    var self = this;
    tools.defineProperties(self, {
        asVal: tools.asVal,
        create: create,
        registerObjectType: register,
        wyrmhole: wyrmhole
    });
    var supportedTypes = {}; // things for which we'll respond to 'New'

    var baseWyrmlingStore = {};
    tools.addWyrmlingStore(baseWyrmlingStore, 0); // will be used when objects are sent via queenling (e.g. queenling.doSomethingWith(myObj))

    wyrmhole.onMessage(function(msg, cb) {
        tools.handleMessage(wyrmhole, baseWyrmlingStore, supportedTypes, msg, cb);
    });

    function create(mimetype, args) {
        args = args || {};
        var wyrmlingStore = baseWyrmlingStore[0];

        // Create and resolve the queenling
        var send = Deferred.fn(wyrmhole, 'sendMessage');
        
        // Register the 'browser' object before bailing
        return register('browser', browser).then(function() {
        	return send(['New', mimetype, args]);
        }).then(function(spawnId) {
            return tools.wrapAlienWyrmling(wyrmhole, wyrmlingStore, spawnId, 0);
        }).then(function(queenling) {
            tools.defineProperties(queenling, {
                destroy: function() {
                    return send(['Destroy', queenling.spawnId]);
                }
            });
            return queenling;
        }, function(error) {
            //console.warn("CREATE ERROR:", error);
            return Deferred.reject(error);
        });
    }

    /**
     * Returns a promise that resolves to undefined if the registration was
     * successful or rejects if invalid parameters were provided.
     *
     * @param type {String} which 'New' messages should be handled, e.g. 'application/myApp'
     * @param factoryFn {Function} Called when a matching 'New' is received over the Wyrmhole.
     *   Invoked with a parameters object; must return object.
     *
     *   The object returned may optionally specify an `_onDestroy` function, which will
     *   be called when we receive 'Destroy' from the Wyrmhole.
     */
    function register(type, factoryFn) {
        return Deferred.when(factoryFn).then(function(factory) {
            if (!type || typeof(type) !== 'string') {
                return Deferred.reject('Must provide a valid object type, e.g. application/myApp');
            } else if (!tools.isFunction(factory)) {
                return Deferred.reject('Must provide a function to invoke when a new instance is requested');
            }
            supportedTypes[type] = factory;
        });
    }
}
}(typeof(global) !== 'undefined' ? global : this));

}).call(this,typeof global !== "undefined" ? global : typeof self !== "undefined" ? self : typeof window !== "undefined" ? window : {})
},{"./browser":3,"./deferred":4,"./tools":6}],6:[function(require,module,exports){
/* global toString */
'use strict';
var Deferred = require('./deferred');
var base64 = require('base64-arraybuffer');
var validMessages = {'New':true, 'Destroy':true, 'RelObj':true, 'Enum':true, 'DelP':true, 'GetP':true, 'SetP':true, 'Invoke':true};

var LocalRelObjDelay = 1000;
var AutoReleaseWindow = 5000;

module.exports = {
    addWyrmlingStore: addWyrmlingStore,
    asVal: asVal,
    defineProperties: defineProperties,
    handleMessage: handleMessage,
    isArray: isArray,
    isFunction: isFunction,
    isNumber: isNumber,
    isObject: isObject,
    retainAllWyrmlings: retainAllWyrmlings,
    wrapAlienWyrmling: wrapAlienWyrmling,
};

function addWyrmlingStore(baseStore, spawnId, rootObject) {
    var nextId = 1;
    var newStore = {};
    Object.defineProperties(newStore, {
        baseStore: { value: baseStore },
        spawnId: { value: spawnId },
        destroy: { value: function() {
            Object.keys(newStore).forEach(function(objectId) {
                newStore.releaseObject(objectId);
            });
            delete baseStore[spawnId];
        }},
        getObject: { value: function(objectId) {
            return newStore[objectId] && newStore[objectId][0];
        }},
        putObject: { value: function(obj) {
            var id = nextId++;
            newStore[id] = [obj];
            return id;
        }},
        releaseObject: { value: function(objectId) {
            var wyrmlingProperties = newStore.getWyrmlingProperties(objectId);
            Object.keys(wyrmlingProperties).forEach(function(prop) {
                wyrmlingProperties[prop].release();
            });
            clearInterval(wyrmlingProperties.__timer);
            wyrmlingProperties.__timer = null;
            delete newStore[objectId];
        }},
        setObjectProperty: { value: function(objectId, prop, val) {
            var obj = newStore.getObject(objectId);
            var wyrmlingProperties = newStore.getWyrmlingProperties(objectId);
            if (wyrmlingProperties[prop]) {
                wyrmlingProperties[prop].release();
                delete wyrmlingProperties[prop];
            }
            obj[prop] = val;
            if (isWyrmling(val)) {
                wyrmlingProperties[prop] = val;
                val.retain();
            }
            var wyrmPropKeys = Object.keys(wyrmlingProperties).length;
            if (wyrmPropKeys === 0 && wyrmlingProperties.__timer) {
                clearInterval(wyrmlingProperties.__timer);
                wyrmlingProperties.__timer = null;
            } else if (wyrmPropKeys && !wyrmlingProperties.__timer) {
                wyrmlingProperties.__timer = setInterval(function() {
                    for (var prop in wyrmlingProperties) {
                        if (wyrmlingProperties.hasOwnProperty(prop)) {
                            if (obj[prop] !== wyrmlingProperties[prop]) {
                                newStore.setObjectProperty(objectId, prop, obj[prop]);
                            }
                        }
                    }
                }, AutoReleaseWindow);
            }
        }},
        getWyrmlingProperties: { value: function(objectId) {
            var arr = newStore[objectId];
            if (!isArray(arr)) { return {}; }
            if (isObject(arr[1])) { return arr[1]; }
            var wyrmlingProperties = {};
            Object.defineProperty(wyrmlingProperties, '__timer', {
                value: null,
                writable: true
            });
            arr[1] = wyrmlingProperties;
            return wyrmlingProperties;
        }}
    });
    Object.defineProperty(baseStore, spawnId, { value: newStore, configurable: true });
    if (rootObject !== (void 0)) {
        newStore[0] = [rootObject];
    }
    return newStore;
}

function asVal(obj) {
    if (isPrimitive(obj)) { return obj; }
    if (obj instanceof ArrayBuffer) {
        return { $type: 'binary', data: base64.encode(obj) };
    }
    return { $type: 'json', data: obj };
}

// defines provided properties as non-configurable, non-enumerable, non-writable values
function defineProperties(obj, props) {
    for (var prop in props) {
        if (props.hasOwnProperty(prop)) {
            Object.defineProperty(obj, prop, { value: props[prop] });
        }
    }
}

// performs Enum and creates the getters / setters / etc.
function wrapAlienWyrmling(wyrmhole, wyrmlingStore, spawnId, objectId) {
    var refCount = 0, released = false;
    var releasedFailure = Deferred.reject({
        error: 'invalid object',
        message: 'The object has been released'
    });
    var send = function(args) {
        wyrmling.retain();
        var dfd = Deferred();
        var callback = function(status, resp) {
            if (status === 'success') { dfd.resolve(resp); }
            else { dfd.reject(resp); }
            wyrmling.release();
        };
        wyrmhole.sendMessage(args, callback);
        return dfd.promise;
    };
    var wyrmling = function() {
        var args = [''].concat(Array.prototype.slice.call(arguments, 0));
        return wyrmling.invoke.apply(wyrmling, args);
    };
    var wyrmlingProperties = {};
    Object.defineProperty(wyrmlingProperties, '__timer', { value: null, writable: true });
    // Add our helper properties
    defineProperties(wyrmling, {
        spawnId: spawnId,
        objectId: objectId,
        getProperty: function(prop) {
            var propValue;
            var getPromise = released ? releasedFailure : send(['GetP', spawnId, objectId, prop]).then(function(val) {
                return prepInboundValue(wyrmhole, wyrmlingStore, val);
            }).then(function(val) {
                propValue = val;
                return val;
            });
            function magicalFn() {
                var args = Array.prototype.slice.call(arguments, 0);
                return getPromise.then(function() {
                    if (isWyrmling(propValue)) {
                        propValue.retain();
                        var invokePromise = propValue.apply(null, args);
                        Deferred.always(invokePromise, function() { propValue.release(); });
                        return invokePromise;
                    } else {
                        return Deferred.reject({ error: 'could not invoke', message: 'The object is not invokable' });
                    }
                });
            }
            magicalFn.then = getPromise.then;
            return magicalFn;
        },
        setProperty: function(prop, val) {
            if (released) { return releasedFailure; }
            return prepOutboundValue(wyrmlingStore, val).then(function(v) {
                return send(['SetP', spawnId, objectId, prop, v]);
            });
        },
        invoke: function(prop) {
            if (released) { return releasedFailure; }
            var args = Array.prototype.slice.call(arguments, 1);
            return prepOutboundArguments(wyrmlingStore, args).then(function(args) {
                return send(['Invoke', spawnId, objectId, prop, args]);
            }).then(function(val) {
                return prepInboundValue(wyrmhole, wyrmlingStore, val);
            });
        },
        retain: function() {
            refCount++;
        },
        release: function() {
            refCount--;
            if (objectId === 0) { return; } // queenlings must be manually destroyed
            setTimeout(function() { if (!refCount && !released) {
                send(['RelObj', spawnId, objectId]);
                released = true;
            }}, 5000);
        }
    });
    return send(['Enum', spawnId, objectId]).then(function(props) {
        for (var i = 0; i < props.length; i++) {
            try {
                createProperty(wyrmling, props[i]);
            } catch(e) {
                //console.warn("Could not create property " + props[i] + ":", e);
            }
        }
        return wyrmling;
    });
}

function createProperty(wyrmling, prop) {
    Object.defineProperty(wyrmling, prop, {
        enumerable: true,
        configurable: false, // don't allow it to be deleted (it isn't ours)
        get: function() {
            return wyrmling.getProperty(prop);
        },
        set: function(val) {
            return wyrmling.setProperty(prop, val);
        }
    });
}

// stores this as a localWyrmling, if necessary
function prepOutboundValue(wyrmlingStore, val) {
    return Deferred.when(val).then(function(v) {
        if (isPrimitive(v) || v.$type === 'json' || v.$type === 'binary' || v.$type === 'error') {
            return v;
        }
        if (v.$type === 'one-level') {
            for (var prop in v.data) {
                if (v.data.hasOwnProperty(prop)) {
                    v.data[prop] = prepOutboundValue(wyrmlingStore, v.data[prop]);
                }
            }
            return Deferred.all(v.data);
        }
        // this is an object we need to send by reference; store and send
        var objectId = wyrmlingStore.putObject(v);
        return { $type: 'ref', data: [wyrmlingStore.spawnId, objectId] };
    });
}
// returns after prepOutboundValue has resolved for each arg
function prepOutboundArguments(wyrmlingStore, args) {
    return Deferred.when(args).then(function(rargs) {
        if (!isArray(rargs) || !rargs.length) { return []; }
        var toResolve = args.map(function(val) {
            return prepOutboundValue(wyrmlingStore, val).then(function(v) {
                return v;
            });
        });
        return Deferred.all(toResolve);
    });
}
function prepInboundValue(wyrmhole, wyrmlingStore, val) {
    return Deferred.when(val).then(function() {
        if (isPrimitive(val)) { return val; }
        if (val.$type === 'local-ref') {
            var store = wyrmlingStore.baseStore;
            if (store[val.data[0]] && val.data[1] in store[val.data[0]]) {
                return store[val.data[0]].getObject(val.data[1]);
            }
            return (void 0); // bad local-ref, receiver has to just deal with it
        }
        if (val.$type === 'ref') {
            return wrapAlienWyrmling(wyrmhole, wyrmlingStore, val.data[0], val.data[1]);
        }
        if (val.$type === 'json') {
            return val.data;
        }
        if (val.$type === 'binary') {
            return base64.decode(val.data);
        }

        // This must be an object, so recursively make it magical. Since any property could
        // be a wyrmling, retain them until everything is ready so autorelease doesn't kick
        // in if another wyrmling happens to take a long time to get back from Enum, etc.
        var wyrmlings = [];
        function retainIfWyrmling(v) {
            if (isWyrmling(v)) {
                v.retain();
                wyrmlings.push(v);
            }
            return v;
        }
        for (var prop in val) {
            if (val.hasOwnProperty(prop)) {
                val[prop] = prepInboundValue(wyrmhole, wyrmlingStore, val[prop]).then(retainIfWyrmling);
            }
        }
        var allFinishedPromise = Deferred.all(val);
        Deferred.always(allFinishedPromise, function() {
            // everything is ready now, so get the release flow back on track
            wyrmlings.forEach(function(ling) { ling.release(); });
        });
        return allFinishedPromise;
    });
}

function isValidMessage(msg) {
    if (!isArray(msg) || !validMessages[msg[0]]) {
        return false;
    }
    switch (msg[0]) {
        case 'Destroy':
            return msg.length === 2 && isNumber(msg[1]);
        case 'New':
            return msg.length === 3 && msg[1] && isString(msg[1]);
        case 'Enum':
        case 'RelObj':
            return msg.length === 3 && isNumber(msg[1]) && isNumber(msg[2]);
        case 'DelP':
        case 'GetP':
            return msg.length === 4 && isNumber(msg[1]) && isNumber(msg[2]) && isString(msg[3]);
        case 'SetP':
            return msg.length === 5 && isNumber(msg[1]) && isNumber(msg[2]) && isString(msg[3]);
        case 'Invoke':
            return msg.length === 5 && isNumber(msg[1]) && isNumber(msg[2]) && isString(msg[3]) && isArray(msg[4]);
    }
}
function getWyrmlingStoreForMessage(baseWyrmlingStore, msg) {
    return msg[1] in baseWyrmlingStore ? baseWyrmlingStore[msg[1]] : {};
}
function getObject(wyrmlingStore, msg) {
    return msg[2] in wyrmlingStore ? wyrmlingStore.getObject(msg[2]) : null;
}
function handleMessage(wyrmhole, baseWyrmlingStore, supportedTypes, msg, cb) {
    if (!isValidMessage(msg)) {
        return cb('error', { error: 'invalid message', message: 'Message was malformed'});
    }
    if (msg[0] === 'New') {
        return handleNew(baseWyrmlingStore, supportedTypes, msg, cb);
    } else if (msg[0] === 'Destroy') {
        return handleDestroy(baseWyrmlingStore, supportedTypes, msg, cb);
    }

    var store = getWyrmlingStoreForMessage(baseWyrmlingStore, msg);
    var obj = getObject(store, msg);
    if (obj === null) {
        return cb('error', { error: 'invalid object', message: 'The object does not exist'});
    }
    switch (msg[0]) {
        case 'Enum': return handleEnum(obj, cb);
        case 'DelP': return handleDelP(store, obj, msg[3], cb);
        case 'GetP': return handleGetP(store, obj, msg[3], cb);
        case 'SetP': return handleSetP(wyrmhole, store, obj, msg[2], msg[3], msg[4], cb);
        case 'RelObj': return handleRelObj(store, msg[2], cb);
        case 'Invoke': return handleInvoke(wyrmhole, store, obj, msg[3], msg[4], cb);
    }
}
function handleNew(baseWyrmlingStore, supportedTypes, msg, cb) {
    if (!(msg[1] in supportedTypes)) {
        return cb('error', { error: 'invalid object type', message: 'Object type ' + msg[1] + ' is not supported' });
    }
    try {
        var princessling = supportedTypes[msg[1]](msg[2] || {});
        baseWyrmlingStore.nextId = baseWyrmlingStore.nextId || 1;
        var spawnId = baseWyrmlingStore.nextId++;
        addWyrmlingStore(baseWyrmlingStore, spawnId, princessling);
        cb('success', spawnId);
    } catch(error) {
        cb('error', { error: 'could not create object', message: error && error.message || 'There was an unidentified error creating the object'});
    }
}
function handleDestroy(baseWyrmlingStore, supportedTypes, msg, cb) {
    var spawnId = msg[1];
    if (!baseWyrmlingStore[spawnId] || !baseWyrmlingStore[spawnId].getObject(0)) {
        return cb('error', { error: 'could not destroy object', message: 'The object does not exist' });
    }
    try {
        var princessling = baseWyrmlingStore[spawnId].getObject(0);
        baseWyrmlingStore[spawnId].destroy();
        if (isFunction(princessling._onDestroy)) {
            princessling._onDestroy();
        }
        cb('success', spawnId);
    } catch(error) {
        cb('error', { error: 'could not destroy object', message: error && error.message || 'There was an unidentified error creating the object'});
    }
}
function handleEnum(obj, cb) {
    var props = [];
    for (var prop in obj) {
        if (obj.hasOwnProperty(prop) && prop[0] !== '_') {
            props.push(prop);
        }
    }
    // add special "length" property for arrays and functions
    if (isArray(obj) || isFunction(obj)) {
        props.push('length');
    }
    return cb('success', props);
}
function handleDelP(wyrmlingStore, obj, prop, cb) {
    if (!obj.hasOwnProperty(prop)) {
        return cb('error', { error: 'could not delete property', message: 'Property does not exist on this object' });
    }
    try {
        delete obj[prop];
        cb('success', null);
    } catch(error) {
        cb('error', { error: 'could not delete property', message: error && error.message || 'There was an unidentified error deleting the property'});
    }
}
function handleGetP(wyrmlingStore, obj, prop, cb) {
    if (!obj.hasOwnProperty(prop) && !(prop === 'length' && (isArray(obj) || isFunction(obj)))) {
        return cb('error', { error: 'could not get property', message: 'Property does not exist on this object' });
    }
    prepOutboundValue(wyrmlingStore, obj[prop]).then(function(val) {
        cb('success', val);
    });
}
function handleSetP(wyrmhole, wyrmlingStore, obj, objectId, prop, val, cb) {
    if (!obj.hasOwnProperty(prop)) {
        return cb('error', { error: 'could not set property', message: 'Property does not exist on this object' });
    }
    prepInboundValue(wyrmhole, wyrmlingStore, val).then(function(v) {
        try {
            wyrmlingStore.setObjectProperty(objectId, prop, v);
            cb('success', null);
        } catch(error) {
            cb('error', { error: 'could not set property', message: error && error.message || 'There was an unidentified error deleting the property'});
        }
    }, function(error) {
        cb('error', { error: 'could not set property', message: error || 'There was an unidentified error setting the property' });
    });
}
function handleRelObj(wyrmlingStore, objectId, cb) {
    if (objectId === 0) { return; } // root objects cannot be released, they must be destroyed
    cb('success', null);
    // In order to make sure that async timing issues don't cause this to be released
    // before an object which is returned from the browser, add a short delay before this completes
    setTimeout(function() {
        wyrmlingStore.releaseObject(objectId);
    }, LocalRelObjDelay);
}
function handleInvoke(wyrmhole, wyrmlingStore, obj, prop, args, cb) {
    var retVal, promises = [];
    if (prop) {
        if (!obj.hasOwnProperty(prop)) {
            return cb('error', { error: 'could not invoke property', message: 'Property does not exist on this object' });
        } else if (!isFunction(obj[prop])) {
            return cb('error', { error: 'could not invoke property', message: 'Property is not callable' });
        }
        args.forEach(function(arg) {
            promises.push(prepInboundValue(wyrmhole, wyrmlingStore, arg));
        });
        retVal = Deferred.all(promises).then(function(args) {
            return obj[prop].apply(obj, args);
        });
    }
    else {
        if (!isFunction(obj)) {
            return cb('error', { error: 'could not invoke object', message: 'Object is not callable' });
        }
        args.forEach(function(arg) {
            promises.push(prepInboundValue(wyrmhole, wyrmlingStore, arg));
        });
        retVal = Deferred.all(promises).then(function(args) {
            return obj.apply(null, args);
        });
    }
    return Deferred.when(retVal).then(function(val) {
        return prepOutboundValue(wyrmlingStore, val).then(function(v) {
            if (v && v.$type === 'error') {
                return cb('error', v.data);
            }
            return cb('success', v);
        });
    }, function(error) {
        var type = prop ? 'property' : 'object';
        return cb('error', { error: 'could not invoke ' + type, message: error || 'There was an unidentified error calling the ' + type });
    });
}

function findWyrmlings(thing) {
    var wyrmlings = [];
    if (isWyrmling(thing)) {
        return [thing];
    } else if (isArray(thing) || isObject(thing)) {
        for (var prop in thing) {
            if (thing.hasOwnProperty(prop)) {
                wyrmlings = wyrmlings.concat(findWyrmlings(thing[prop]));
            }
        }
    }
    return wyrmlings;
}
function retainAllWyrmlings(thing) {
    var wyrmlings = findWyrmlings(thing);
    wyrmlings.forEach(function(ling) { ling.retain(); });
    return function() {
        wyrmlings.forEach(function(ling) { ling.release(); });
    };
}

function isPrimitive(val) {
    if (val === null) { return true; } // to avoid false positive on typeof 'object' test below
    switch (typeof val) {
        case 'object':
        case 'function':
        case 'symbol':
            return false;
        default:
            return true;
    }
}

// adapted from underscore.js
var nativeIsArray = Array.isArray;
function isArray(obj) {
    return nativeIsArray ? Array.isArray(obj) : toString.call(obj) === '[object Array]';
}

// Optimize `isFunction` if appropriate. Work around some typeof bugs in old v8,
// IE 11 (#1621), and in Safari 8 (#1929).
var optimizeIsFunction = typeof /./ !== 'function' && typeof Int8Array !== 'object';
function isFunction(obj) {
    if (optimizeIsFunction) {
        return typeof obj === 'function' || false;
    } else {
        return toString.call(obj) === '[object Function]';
    }
}

function isNumber(val) { return toString.call(val) === '[object Number]' && !isNaN(val); }
// match plain objects, not special things like null or ArrayBuffer
function isObject(val) { return val && toString.call(val) === '[object Object]'; }
function isString(val) { return typeof(val) === 'string'; }
function isWyrmling(val) {
    return isFunction(val) &&
           val.hasOwnProperty('spawnId') &&
           val.hasOwnProperty('objectId') &&
           val.hasOwnProperty('getProperty') &&
           val.hasOwnProperty('setProperty') &&
           val.hasOwnProperty('invoke');
}

},{"./deferred":4,"base64-arraybuffer":1}]},{},[5]);
