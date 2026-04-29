function first(name) {
  this.name = "Krishna";

  function inner(name) {
    this.name = "Nitish";
  }

  inner();
}

first();
