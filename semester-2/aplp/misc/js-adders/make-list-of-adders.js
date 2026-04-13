makeAdder = (x) => (y) => x + y;

makeListOfAdders = (nums) => {
  let result = [];
  for (var i = 0; i < nums.length; i++) {
    result[i] = makeAdder(nums[i]);
  }

  return result;
};

a = makeListOfAdders([1, 5]);
console.log(a[0](42));
console.log(a[1](42));
