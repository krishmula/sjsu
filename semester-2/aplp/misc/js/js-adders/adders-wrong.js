function makeListOfAdders(lst) {
  var arr = [];
  for (var i = 0; i < lst.length; i++) {
    var n = lst[i];
    arr[i] = function (x) {
      return x + n;
    };
  }
  return arr;
}
var adders = makeListOfAdders([1, 3, 99, 21]);
adders.forEach(function (adder) {
  console.log(adder(100));
});
