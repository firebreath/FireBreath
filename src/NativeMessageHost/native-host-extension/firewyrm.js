(function(wrapdefine) { wrapdefine(['module'], function(req_module) {/**
 * @license almond 0.3.1 Copyright (c) 2011-2014, The Dojo Foundation All Rights Reserved.
 * Available via the MIT or new BSD license.
 * see: http://github.com/jrburke/almond for details
 */
//Going sloppy to avoid 'use strict' string cost, but strict practices should
//be followed.
/*jslint sloppy: true */
/*global setTimeout: false */

var requirejs, require, define;
(function (undef) {
    var main, req, makeMap, handlers,
        defined = {},
        waiting = {},
        config = {},
        defining = {},
        hasOwn = Object.prototype.hasOwnProperty,
        aps = [].slice,
        jsSuffixRegExp = /\.js$/;

    function hasProp(obj, prop) {
        return hasOwn.call(obj, prop);
    }

    /**
     * Given a relative module name, like ./something, normalize it to
     * a real name that can be mapped to a path.
     * @param {String} name the relative name
     * @param {String} baseName a real name that the name arg is relative
     * to.
     * @returns {String} normalized name
     */
    function normalize(name, baseName) {
        var nameParts, nameSegment, mapValue, foundMap, lastIndex,
            foundI, foundStarMap, starI, i, j, part,
            baseParts = baseName && baseName.split("/"),
            map = config.map,
            starMap = (map && map['*']) || {};

        //Adjust any relative paths.
        if (name && name.charAt(0) === ".") {
            //If have a base name, try to normalize against it,
            //otherwise, assume it is a top-level require that will
            //be relative to baseUrl in the end.
            if (baseName) {
                name = name.split('/');
                lastIndex = name.length - 1;

                // Node .js allowance:
                if (config.nodeIdCompat && jsSuffixRegExp.test(name[lastIndex])) {
                    name[lastIndex] = name[lastIndex].replace(jsSuffixRegExp, '');
                }

                //Lop off the last part of baseParts, so that . matches the
                //"directory" and not name of the baseName's module. For instance,
                //baseName of "one/two/three", maps to "one/two/three.js", but we
                //want the directory, "one/two" for this normalization.
                name = baseParts.slice(0, baseParts.length - 1).concat(name);

                //start trimDots
                for (i = 0; i < name.length; i += 1) {
                    part = name[i];
                    if (part === ".") {
                        name.splice(i, 1);
                        i -= 1;
                    } else if (part === "..") {
                        if (i === 1 && (name[2] === '..' || name[0] === '..')) {
                            //End of the line. Keep at least one non-dot
                            //path segment at the front so it can be mapped
                            //correctly to disk. Otherwise, there is likely
                            //no path mapping for a path starting with '..'.
                            //This can still fail, but catches the most reasonable
                            //uses of ..
                            break;
                        } else if (i > 0) {
                            name.splice(i - 1, 2);
                            i -= 2;
                        }
                    }
                }
                //end trimDots

                name = name.join("/");
            } else if (name.indexOf('./') === 0) {
                // No baseName, so this is ID is resolved relative
                // to baseUrl, pull off the leading dot.
                name = name.substring(2);
            }
        }

        //Apply map config if available.
        if ((baseParts || starMap) && map) {
            nameParts = name.split('/');

            for (i = nameParts.length; i > 0; i -= 1) {
                nameSegment = nameParts.slice(0, i).join("/");

                if (baseParts) {
                    //Find the longest baseName segment match in the config.
                    //So, do joins on the biggest to smallest lengths of baseParts.
                    for (j = baseParts.length; j > 0; j -= 1) {
                        mapValue = map[baseParts.slice(0, j).join('/')];

                        //baseName segment has  config, find if it has one for
                        //this name.
                        if (mapValue) {
                            mapValue = mapValue[nameSegment];
                            if (mapValue) {
                                //Match, update name to the new value.
                                foundMap = mapValue;
                                foundI = i;
                                break;
                            }
                        }
                    }
                }

                if (foundMap) {
                    break;
                }

                //Check for a star map match, but just hold on to it,
                //if there is a shorter segment match later in a matching
                //config, then favor over this star map.
                if (!foundStarMap && starMap && starMap[nameSegment]) {
                    foundStarMap = starMap[nameSegment];
                    starI = i;
                }
            }

            if (!foundMap && foundStarMap) {
                foundMap = foundStarMap;
                foundI = starI;
            }

            if (foundMap) {
                nameParts.splice(0, foundI, foundMap);
                name = nameParts.join('/');
            }
        }

        return name;
    }

    function makeRequire(relName, forceSync) {
        return function () {
            //A version of a require function that passes a moduleName
            //value for items that may need to
            //look up paths relative to the moduleName
            var args = aps.call(arguments, 0);

            //If first arg is not require('string'), and there is only
            //one arg, it is the array form without a callback. Insert
            //a null so that the following concat is correct.
            if (typeof args[0] !== 'string' && args.length === 1) {
                args.push(null);
            }
            return req.apply(undef, args.concat([relName, forceSync]));
        };
    }

    function makeNormalize(relName) {
        return function (name) {
            return normalize(name, relName);
        };
    }

    function makeLoad(depName) {
        return function (value) {
            defined[depName] = value;
        };
    }

    function callDep(name) {
        if (hasProp(waiting, name)) {
            var args = waiting[name];
            delete waiting[name];
            defining[name] = true;
            main.apply(undef, args);
        }

        if (!hasProp(defined, name) && !hasProp(defining, name)) {
            throw new Error('No ' + name);
        }
        return defined[name];
    }

    //Turns a plugin!resource to [plugin, resource]
    //with the plugin being undefined if the name
    //did not have a plugin prefix.
    function splitPrefix(name) {
        var prefix,
            index = name ? name.indexOf('!') : -1;
        if (index > -1) {
            prefix = name.substring(0, index);
            name = name.substring(index + 1, name.length);
        }
        return [prefix, name];
    }

    /**
     * Makes a name map, normalizing the name, and using a plugin
     * for normalization if necessary. Grabs a ref to plugin
     * too, as an optimization.
     */
    makeMap = function (name, relName) {
        var plugin,
            parts = splitPrefix(name),
            prefix = parts[0];

        name = parts[1];

        if (prefix) {
            prefix = normalize(prefix, relName);
            plugin = callDep(prefix);
        }

        //Normalize according
        if (prefix) {
            if (plugin && plugin.normalize) {
                name = plugin.normalize(name, makeNormalize(relName));
            } else {
                name = normalize(name, relName);
            }
        } else {
            name = normalize(name, relName);
            parts = splitPrefix(name);
            prefix = parts[0];
            name = parts[1];
            if (prefix) {
                plugin = callDep(prefix);
            }
        }

        //Using ridiculous property names for space reasons
        return {
            f: prefix ? prefix + '!' + name : name, //fullName
            n: name,
            pr: prefix,
            p: plugin
        };
    };

    function makeConfig(name) {
        return function () {
            return (config && config.config && config.config[name]) || {};
        };
    }

    handlers = {
        require: function (name) {
            return makeRequire(name);
        },
        exports: function (name) {
            var e = defined[name];
            if (typeof e !== 'undefined') {
                return e;
            } else {
                return (defined[name] = {});
            }
        },
        module: function (name) {
            return {
                id: name,
                uri: '',
                exports: defined[name],
                config: makeConfig(name)
            };
        }
    };

    main = function (name, deps, callback, relName) {
        var cjsModule, depName, ret, map, i,
            args = [],
            callbackType = typeof callback,
            usingExports;

        //Use name if no relName
        relName = relName || name;

        //Call the callback to define the module, if necessary.
        if (callbackType === 'undefined' || callbackType === 'function') {
            //Pull out the defined dependencies and pass the ordered
            //values to the callback.
            //Default to [require, exports, module] if no deps
            deps = !deps.length && callback.length ? ['require', 'exports', 'module'] : deps;
            for (i = 0; i < deps.length; i += 1) {
                map = makeMap(deps[i], relName);
                depName = map.f;

                //Fast path CommonJS standard dependencies.
                if (depName === "require") {
                    args[i] = handlers.require(name);
                } else if (depName === "exports") {
                    //CommonJS module spec 1.1
                    args[i] = handlers.exports(name);
                    usingExports = true;
                } else if (depName === "module") {
                    //CommonJS module spec 1.1
                    cjsModule = args[i] = handlers.module(name);
                } else if (hasProp(defined, depName) ||
                           hasProp(waiting, depName) ||
                           hasProp(defining, depName)) {
                    args[i] = callDep(depName);
                } else if (map.p) {
                    map.p.load(map.n, makeRequire(relName, true), makeLoad(depName), {});
                    args[i] = defined[depName];
                } else {
                    throw new Error(name + ' missing ' + depName);
                }
            }

            ret = callback ? callback.apply(defined[name], args) : undefined;

            if (name) {
                //If setting exports via "module" is in play,
                //favor that over return value and exports. After that,
                //favor a non-undefined return value over exports use.
                if (cjsModule && cjsModule.exports !== undef &&
                        cjsModule.exports !== defined[name]) {
                    defined[name] = cjsModule.exports;
                } else if (ret !== undef || !usingExports) {
                    //Use the return value from the function.
                    defined[name] = ret;
                }
            }
        } else if (name) {
            //May just be an object definition for the module. Only
            //worry about defining if have a module name.
            defined[name] = callback;
        }
    };

    requirejs = require = req = function (deps, callback, relName, forceSync, alt) {
        if (typeof deps === "string") {
            if (handlers[deps]) {
                //callback in this case is really relName
                return handlers[deps](callback);
            }
            //Just return the module wanted. In this scenario, the
            //deps arg is the module name, and second arg (if passed)
            //is just the relName.
            //Normalize module name, if it contains . or ..
            return callDep(makeMap(deps, callback).f);
        } else if (!deps.splice) {
            //deps is a config object, not an array.
            config = deps;
            if (config.deps) {
                req(config.deps, config.callback);
            }
            if (!callback) {
                return;
            }

            if (callback.splice) {
                //callback is an array, which means it is a dependency list.
                //Adjust args if there are dependencies
                deps = callback;
                callback = relName;
                relName = null;
            } else {
                deps = undef;
            }
        }

        //Support require(['a'])
        callback = callback || function () {};

        //If relName is a function, it is an errback handler,
        //so remove it.
        if (typeof relName === 'function') {
            relName = forceSync;
            forceSync = alt;
        }

        //Simulate async callback;
        if (forceSync) {
            main(undef, deps, callback, relName);
        } else {
            //Using a non-zero value because of concern for what old browsers
            //do, and latest browsers "upgrade" to 4 if lower value is used:
            //http://www.whatwg.org/specs/web-apps/current-work/multipage/timers.html#dom-windowtimers-settimeout:
            //If want a value immediately, use require('id') instead -- something
            //that works in almond on the global level, but not guaranteed and
            //unlikely to work in other AMD implementations.
            setTimeout(function () {
                main(undef, deps, callback, relName);
            }, 4);
        }

        return req;
    };

    /**
     * Just drops the config on the floor, but returns req in case
     * the config return value is used.
     */
    req.config = function (cfg) {
        return req(cfg);
    };

    /**
     * Expose module registry for debugging and tooling
     */
    requirejs._defined = defined;

    define = function (name, deps, callback) {
        if (typeof name !== 'string') {
            throw new Error('See almond README: incorrect module build, no module name');
        }

        //This module may not have dependencies
        if (!deps.splice) {
            //deps is not an array, so probably means
            //an object literal or factory function for
            //the value. Adjust args.
            callback = deps;
            deps = [];
        }

        if (!hasProp(defined, name) && !hasProp(waiting, name)) {
            waiting[name] = [name, deps, callback];
        }
    };

    define.amd = {
        jQuery: true
    };
}());

define("node_modules/almond/almond", function(){});

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

define("node_modules/fbpromise/FireBreathPromise", function(){});

/* global toString */
/**
 * Provides a slightly-decorated FBPromise
 */

define('src/deferred',['../node_modules/fbpromise/FireBreathPromise'], function(fbpromise) {
    var Deferred = window.FireBreathPromise;

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
});


define('src/base64',[], function() {

    function base64ArrayBuffer(arrayBuffer) {
        var base64    = '';
        var encodings = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';

        var bytes         = new Uint8Array(arrayBuffer);
        var byteLength    = bytes.byteLength;
        var byteRemainder = byteLength % 3;
        var mainLength    = byteLength - byteRemainder;

        var a, b, c, d;
        var chunk;

        // Main loop deals with bytes in chunks of 3
        for (var i = 0; i < mainLength; i = i + 3) {
            // Combine the three bytes into a single integer
            chunk = (bytes[i] << 16) | (bytes[i + 1] << 8) | bytes[i + 2];

            // Use bitmasks to extract 6-bit segments from the triplet
            a = (chunk & 16515072) >> 18; // 16515072 = (2^6 - 1) << 18
            b = (chunk & 258048)   >> 12; // 258048   = (2^6 - 1) << 12
            c = (chunk & 4032)     >>  6; // 4032     = (2^6 - 1) << 6
            d = chunk & 63;               // 63       = 2^6 - 1

            // Convert the raw binary segments to the appropriate ASCII encoding
            base64 += encodings[a] + encodings[b] + encodings[c] + encodings[d];
        }

        // Deal with the remaining bytes and padding
        if (byteRemainder == 1) {
            chunk = bytes[mainLength];

            a = (chunk & 252) >> 2; // 252 = (2^6 - 1) << 2

            // Set the 4 least significant bits to zero
            b = (chunk & 3)   << 4; // 3   = 2^2 - 1

            base64 += encodings[a] + encodings[b] + '==';
        } else if (byteRemainder == 2) {
            chunk = (bytes[mainLength] << 8) | bytes[mainLength + 1];

            a = (chunk & 64512) >> 10; // 64512 = (2^6 - 1) << 10
            b = (chunk & 1008)  >>  4; // 1008  = (2^6 - 1) << 4

            // Set the 2 least significant bits to zero
            c = (chunk & 15)    <<  2; // 15    = 2^4 - 1

            base64 += encodings[a] + encodings[b] + encodings[c] + '=';
        }

        return base64;
    }

    return {
        encode: base64ArrayBuffer
    };

});

/* global toString */


define('src/tools',['./deferred', './base64'], function(Deferred, base64) {
    var validMessages = {'New':true, 'Destroy':true, 'RelObj':true, 'Enum':true, 'DelP':true, 'GetP':true, 'SetP':true, 'Invoke':true};

    return {
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
                                newStore.setObjectProperty(objectId, prop, obj[prop]);
                            }
                        }
                    }, 5000);
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
                if (released) { return; }
                var getPropVal;
                var getPromise = send(['GetP', spawnId, objectId, prop]).then(function(val) {
                    return prepInboundValue(wyrmhole, wyrmlingStore, val);
                }).then(function(val) {
                    getPropVal = val;
                    return val;
                });
                function magicalFn() {
                    var args = Array.prototype.slice.call(arguments, 0);
                    return getPromise.then(function() {
                        if (isWyrmling(getPropVal)) {
                            getPropVal.retain();
                            var invokePromise = getPropVal.apply(null, args);
                            Deferred.always(invokePromise, function() { getPropVal.release(); });
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
                if (released) { return; }
                return prepOutboundValue(wyrmlingStore, val).then(function(v) {
                    return send(['SetP', spawnId, objectId, prop, v]);
                });
            },
            invoke: function(prop) {
                if (released) { return; }
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
                return b64Buffer.decode(val.data);
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
        wyrmlingStore.releaseObject(objectId);
        cb('success', null);
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
});


(function(globalScope) {
    define('src/browser',['./tools'], function(tools) {
        return function(params) {
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

            function invokeWithDelay(delay, fn, args, context) {
                context = context || null;
                if (!(tools.isNumber(delay) && tools.isFunction(fn) && tools.isArray(args))) {
                    return { $type: 'error', data: { error: 'invalid parameters', message: 'Must provide at least delay (Number), fn (Function), and args (Array)'}};
                }

                var releaseWyrmlings = tools.retainAllWyrmlings(args);
                setTimeout(function() {
                    fn.apply(context, args);
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
    });
}(typeof(global) !== 'undefined' ? global : this));



define('src/firewyrm.js',['./deferred', './tools', './browser'], function(Deferred, tools, browser) {
    FireWyrmJS.asVal = tools.asVal;
    return FireWyrmJS;

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
            var wyrmlingStore = baseWyrmlingStore[0];

            // Create and resolve the queenling
            var send = Deferred.fn(wyrmhole, 'sendMessage');
            return send(['New', mimetype, args]).then(function(spawnId) {
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

        // Register the 'browser' object before bailing
        register('browser', browser);
    }
});

    return require('src/firewyrm.js'); 
});})(typeof define !== 'undefined' ? define 
    // try to define as a CommonJS module instead
    : typeof module !== 'undefined' ? function(deps, factory) {
        module.exports = factory();
       }
    // nothing good exists, just define on current context (ie window)
       : function(deps, factory) { this.FireWyrmJS = factory(null); }
);