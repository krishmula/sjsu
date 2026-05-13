var o = { x: 7, y: 10};

let p = new Proxy(o, {
    set: function(obj, prop, val) {
        console.log("Sorry, this is a readonly object");
    },
    get: function(obj, prop) {
        console.log("getting " + prop);
        return obj[prop];
    }
});

console.log(p.x);

p.x = 42;

o.z = o.x + o.y;

console.log(p.z);