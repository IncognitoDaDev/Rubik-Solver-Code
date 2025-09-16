This is a c++ project made to simulate and solve Rubik Cubes in console.

# Backstory:
The idea of creating this project was firstly suggested to me by a mentor during a robotics competition named FTC organized by the organizators of FIRST
as an attempt to improve my coding capabilities and have something to show during my time in robotics. Two years later of hesistation, I have given it a try.

# The concept:
The code will scan a rubik cube utilizing a webcam, translate it into data composed out of a series of 2D matrix and be able to rotate
the digitalized rubik cube in the console throught the well-known rubik cube notations (notations being the ones found in this website: https://jperm.net/3x3/moves).
And relay the moves to the rubik cube in the real world.
Due to many issues the way compiler handles external libraries, the released project will only contain the simulator and solver.

# The code is able to:
- Digitalize a rubik cube utilizing a method of choice: throught the webcam or throught importing a file
- Take input and translate them into moves for the rubik cube
- Relay to the real rubik cube throught an arduino UNO's serial port
- Solve the Rubik Cube using IDA* algorithm

# The code is not able:
- Not solve a properly scrambled rubik cube (unless the rubik cube can be solved in 10-12 moves)
    
