function Wizard(name) {
    this.name = name;
}

var g = new Wizard("Gandalf");
var d = new Wizard("Dumbledore");
var r = new Wizard("Rincewind");

r.cast = function() {
    console.log(this.name + "says forget that, run away!");
};

Wizard.prototype.cast = function(spell) {
    console.log(this.name + " casts " + spell);
};

g.cast("fireball");
r.cast("fireball");
delete g.cast;

g.cast("lightning");
delete g.__proto__.cast;

d.cast("bunny out of hat");
r.cast("bunny out of hat");