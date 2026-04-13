const rabbitName: string = "Monty";

class Rabbit {
    public name: string;

    public constructor(name: string) {
        this.name = name;
    }
}

const r: Rabbit = new Rabbit("Python");

console.log(r.name);
console.log(rabbitName);