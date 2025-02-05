# Jack Compiler

## Overview
This repository contains an implementation of a **Jack Compiler**, which translates Jack source code into VM (Virtual Machine) code. Jack is a simple, high-level programming language used in the **Nand2Tetris** course and project. 
This program creates two output files for a supplied .jack file, a .vm(Virtual Machine code) and a .xml file which is the abstract-syntax tree of the program.

## Features
- **Lexical Analysis**: Tokenizes Jack source code.
- **Syntax Analysis**: Parses Jack code based on its grammar.
- **Code Generation**: Converts parsed code into VM commands.
- **Error Handling**: Detects syntax errors and invalid tokens.
- **Modular Design**: Components are structured for readability and maintainability.

## References
- [The Elements of Computing Systems (Nand2Tetris)](https://www.nand2tetris.org/)
