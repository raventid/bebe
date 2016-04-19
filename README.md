# bebe
My own LISP dialect, crafted with love.

Inspired by Scheme and SICP. Simple, fast, extensible, it requires less code to complete simple task or computation. 

Now language support only basic arithmetic operations.

See wiki for installation instructions.

Enjoy!

## Installation

At the moment Bebe available and well tested at Debian based linux machines.

To compile Bebe from source you have to clone this repo, and then run make, from project folder.

After compilation you should run bebe repl as ./repl from source folder or you might create a link to this file and put it into /usr/bin/

Good luck!

## Operators
``` clojure
+ 4 5  #=> 9
- 2 1  #=> 1
* 2 2  #=> 4
/ 4 2  #=> 2
% 10 6 #=> 4
^ 2 3  #=> 8
```
You can easily nest any number of operations using parenthesis. And pay some attention to the fact that first action do not require you to use parenthesis like ``` + 3 4 ``` and ``` (+ 3 4) ``` is wrong, that makes language less nested.

``` clojure
+ (/ 
     (/ 4 2) 
     (+ 1 1) 
  )
  (- 
     (/ 8 2) 
     (- 3 2) 
  )
```
