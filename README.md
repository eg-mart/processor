# processor
A minimal virtual processor with an assembler.

## Assembler
Usage: ```assemble -i input_file -o output_file```

## Processor
Usage: ```processor -i input_file```

## Features
* Label support
* Jump support
* CALL and RET support (function support)
* 5 registers
* Stack with fixed precision numbers
* RAM support
* Robust error handling

## Examples
* ```quadradq.eg``` - a program for solving quadratic equation of any form.
  
  **Input:** three coefficient a, b, c (where the equation is a * x ^ 2 + b * x + c = 0)

  **Output:** roots or a message if there are no (infinite) roots.


* ```circle.eg``` - a program for drawing a circle on the screen.
  
  Generates an image of a circle in RAM and then renders it to the screen.
  
  
* ```fib.eg``` - a program for calculating Fibonacci numbers.
  
  **Input:** n (natural number).

  **Output:** n-th Fibonacci number.
