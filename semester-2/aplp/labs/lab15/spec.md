In this lab, we will experiment with the Proxy API.

We wish to make a special "SmartArray", which will add extra functionality to arrays.

1) A SmartArray can accept a 'range', in which case it will return a subarray.  For instance,
  `smartArray['2-4']` returns a subarray of elements at positions 2, 3, and 4.
  (This one is done for you.)

2) A SmartArray can accept a negative index, which will get or set the value counting backwards from
  the length of the array.  So `smartArray[-1]` will return the last element in the array.

3) A SmartArray will not accept index values that are not integers, and will raise an exception in this case.

If you have extra time, try specifying the 'deleteProperty' trap so that it handles negative index values
in the same manner that get and set do.