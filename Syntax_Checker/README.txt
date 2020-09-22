Syntax Checker(Mips 32 Assembly)

---------
Description: In this lab, the user enters a file as an input using a program argument. The program prints the name of the file the user has entered and checks if the file name is valid. The program prints an invalid error and closes the program if the file name is invalid or opens and reads the file if the file name is valid. When the program reads the file, the Program either prints a success match where it outputs the number of matching braces, a brace mismatch error printing mismatched braces and their indices if an opening brace doesn't correspond to it's closing brace/closing brace with no opening brace, or a Stack error with a list of opening braces in reverse with no closing brace. 
closing braces. 
  


---------
FILES

-
Lab4.lgi

This file includes the assembly code of the lab. 



------------
INSTRUCTIONS

This program is intended to be run using MIPS Assembler and Runtime Simulator(MARS). 
Click on run/assemble to make sure the program assembles without error. Enter the name of
the name of the file under the program arguments box, and click the green arrow to run the program. The program will print the name of the file. If the file name doesn't start with a letter, or contains a character that is not a letter, number, underscore, or period, the program will print an invalid file name error. If the file name is valid, the program will print a success message with the number of pairs of braces or an error depending the type of brace syntax error. 
