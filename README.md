# Smart Farm Management System (C++)

## Project Overview
The Smart Farm Management System is a C++ console-based application designed to manage crop information and help farmers analyze cultivation details and expected yields.

## Features
- Manage different types of crops.
- Store crop name, cultivated area, season, and yield per acre.
- Use inheritance with CerealCrop and VegetableCrop classes.
- Demonstrate constructor overloading and virtual destructors.
- Calculate and display total crop yield using runtime polymorphism.
- Compare crops using operator overloading.
- Provide crop-specific farming advice using an abstract Advisory class.
- Save and load crop data using file handling.
- Use a static member to count crop objects.
- Handle invalid input and exceptions.

## Object-Oriented Programming Concepts
- Classes and Objects
- Encapsulation
- Inheritance
- Polymorphism
- Abstraction
- Constructor Overloading
- Operator Overloading
- Virtual Functions
- Static Data Members
- File Handling
- Exception Handling

## Files
- `main.cpp` — Main C++ source code.
- `README.md` — Project documentation.
- `crops.txt` — Crop data file created when saving crop records.

## How to Compile
Use a C++14-compatible compiler.

```bash
g++ -std=c++14 -Wall -Wextra -pedantic main.cpp -o smart_farm
```

## How to Run

Linux/macOS:
```bash
./smart_farm
```

Windows:
```bash
smart_farm.exe
```

## How to Use
1. Run the program.
2. Add cereal or vegetable crop records.
3. Display crop details and total yields.
4. View farming advice.
5. Compare the total yields of two crops.
6. Save crop data to a file.
7. Load previously saved crop data.

## Purpose
This project demonstrates object-oriented programming concepts in C++ through a practical farm management application.

## Author
Created as an academic C++ programming project.
