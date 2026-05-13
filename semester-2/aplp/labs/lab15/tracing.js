let obj = { foo: 'bar' };

let o = new Proxy(obj, {
  set: (target, name, val) => {
    console.log(`Setting ${name} to ${val}`);
    //target[name] = val;
    //return true;
    //return Reflect.set(target,name,val);
    return Reflect.set(...arguments);
  },
  get: (target, name) => {
    console.log(`Getting ${name}`);
    return Reflect.get(...arguments);
  }
});

o.foo = "fighters";

let ff = "foo" + o.foo;

