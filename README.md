# The IQ Solver

This code is an implementation of the IQ solver: designed by Tom Lee, Viktor Toth and Sean Holden.
The code uses some of the code of the QFUN implemetation written by Mikolas Janota, which can be 
found at http://sat.inesc-id.pt/~mikolas/sw/qfun/. Where code has been copied the top of the code's
file will have a comment displaying such.

# Build

To build the IQ solver, you need to have the zlib package installed. Then, run the following comands.

```bash
git clone https://github.com/Tlee43/IQ.git
cd IQ
make
```

# Run

The IQ solver takes QBF in either the qcir or qdimacs format, to run it on a given formula do:

```
./iq yourQBF.qcir
```

This will return an array of four numbers corresponding to the calculated truth value of the QBF, time 
taken to solve the QBF, the number of top level refinements and the number of top level strategy 
refinements.

# Licence

IQ is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or 
(at your option) any later version. 

IQ is distributed in the hope that it will be useful,              
but WITHOUT ANY WARRANTY; without even the implied warranty of     
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      
GNU General Public License for more details.                       





