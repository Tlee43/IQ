# The IQ Solver

This code is an implementation of the IQ solver: designed by Tom Lee, Viktor Toth and Sean Holden.
The code uses some of the code of the QFUN implemetation written by Mikolas Janota, which can be 
found at http://sat.inesc-id.pt/~mikolas/sw/qfun/. Where code is copied the top of the file will
have a comment displaying it has been copied.

# Build

To download the IQ solver run the following comands.

```bash
git clone 
cd IQ
make
```

# Run

The IQ solver takes QBF in either the qcir or qdimacs format, to run it on a given formula do:

```
./iq yourQBF.qcir
```

This will return a array of four numbers corresponding to the calculated truth value of the QBF, time 
taken to solve the QBF, the number of top level refinments and the number of top level strategy 
refinements.

# Licence

Todo




