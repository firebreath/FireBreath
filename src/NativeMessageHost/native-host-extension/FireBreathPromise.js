(function() {
var global=this;
if (global.FireBreathPromise) { return; }
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
global.FireBreathPromise = makeDeferred;
}).call(this);

