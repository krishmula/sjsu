class Person {
    constructor(name, age, gender) {
        this.name = name;
        this.age = age;
        this.gender = gender;
    }

    getName() {
        return this.name;
    }
    getAge() {
        return this.age;
    }
    getGender() {
        return this.gender;
    }
};



var person = new Person("John", 30, "male");

var p = new Proxy(person, {
    get: function (target, prop, receiver) {
        console.log(`Getting ${prop}`);
        return Reflect.get(target, prop, receiver);
    },
    set: function (target, prop, value, receiver) {
        console.log(`Setting ${prop} to ${value}`);
        return Reflect.set(target, prop, value, receiver);
    },
    deleteProperty: function (target, prop) {
        console.log(`Deleting ${prop} isn't allowed`);
        return false;
    }
});