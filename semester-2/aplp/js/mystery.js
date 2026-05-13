function mystery(n) {
    var curPos = 2, lst = [], i;
    for (i = curPos; i < n; i++) lst[i] = i; /* loop1 */
    return function() {
        var x;
        while (lst[curPos] == undefined) {
            if (curPos > n) throw new Error("Nothing left");
            curPos++;
        }
        x = curPos;
        while (x < n) {
            delete lst[x];
            x += curPos;
        }
        return curPos;
    }
}

var next = mystery(25);
for (i = 0; i < 7; i++) {
    console.log(next());
}

// what are anonymous functions?
// what are inner functions?
// what are closures?
// what is the scoping of var?
// var vs let vs const
// this keyword
// what is the property of var, this, in anonymous functions?