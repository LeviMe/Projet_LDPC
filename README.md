The goal of this project is to implement a full LDPC (Low-Density Parity-Check) codec with an FPGA. To prepare for the FPGA implementation, a simulation of the LDPC codec was created using Python as the programming language.

Then the Python code was rewritten in C++, to accelerate the simulations and improve performance.
To maximize computational efficiency and using processor resources to their full potential, the simulations were parallelized in both Python and C++. Processes were used in Python, while POSIX threads were implemented in C++ to ensure more efficient and effective processing.

It is worth noting that the FPGA code is incomplete, and only the encoder has been implemented so far. This is due to various problems with timing constraints which have to be adressed.
