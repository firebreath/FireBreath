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
                return function handleCallback() {
                    try {
                        // If it's not a function, we still need to pass it through to promise2
                        if (isFunction(cbFunc)) {
                            var x = cbFunc.apply(this, arguments);
                            promise2.resolve(x);
                        } else {
                            promise2[type].apply(this, arguments);
                        }
                    } catch (e) {
                        // In case of exception, reject
                        promise2.reject(e);
                    }
                };
            }
            if (state === STATES.RESOLVE) {
            	setTimeout(function() { makeCallback(onFulfilled, "resolve").apply(self, value) }, 0);
            } else if (state === STATES.REJECT) {
            	setTimeout(function() { makeCallback(onRejected, "reject").apply(self, value) }, 0);
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
            value = Array.prototype.slice.call(arguments);
            state = STATES.RESOLVE;
            for (var i = 0; i < fulfillHandlers.length; ++i) {
            	setTimeout(function(handler) { return function() {
            		handler.apply(self, value);
            	}}(fulfillHandlers[i]), 0);
            }
        }
    };
    self.reject = function(x) {
        // Ignore the call if we're already settled, otherwise reject
        if (state === STATES.RESOLVE || state === STATES.REJECT) { return; }
        else if (x === self || x === self.promise) {
            return self.reject(new TypeError());
        }
        value = Array.prototype.slice.call(arguments);
        state = STATES.REJECT;
        for (var i = 0; i < rejectHandlers.length; ++i) {
        	setTimeout(function(handler) { return function() {
        		handler.apply(self, value);
        	}}(rejectHandlers[i]), 0);
        }
    };
}
function makeDeferred(name) {
    return new DeferredObject(name);
}
global.FireBreathPromise = makeDeferred;
}).call(this);

