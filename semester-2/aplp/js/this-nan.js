function Timer(seconds) {
  this.seconds = seconds;
  this.remaining = seconds;
}

Timer.prototype.start = function () {
  setInterval(function () {
    this.remaining--;
    console.log(this.remaining);
  }, 1000);
};

var t = new Timer(5);
t.start();
