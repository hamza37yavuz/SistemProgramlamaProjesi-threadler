[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/40vXwnsI)
[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-718a45dd9cf7e7f842a935f5ebbe5719a5e09af4491e668f4dbf3b35d5cca122.svg)](https://classroom.github.com/online_ide?assignment_repo_id=11065509&assignment_repo_type=AssignmentRepo)
# Project 2 Moving Animals and Hunters

## Learning Objectives
* Concurrency, 
* Posix threads

## Details
### Grid of sites:
In this project, your task is to simulate migrating animals and hunters in 2D grid-sites:
Each site in this grid is good for a different living activity for the animals (randomly assigned):
<table>
  <tr><td>WINTERING </td> <td>FEEDING</td><td>NESTING</td></tr>
  <tr><td>FEEDING</td><td>WINTERING</td><td>WINTERING</td></tr>
  <tr><td>NESTING</td><td>NESTING</td><td>FEEDING</td></tr>
</table>

That means grid[0][0] is good for wintering. 
The initilization of the grid  and type of sites are determined randomly by the following function:
```
Grid initgrid(int xlength, int ylength)
```
### Animals
In the simulation, we have the following three animals 
* a bird, 
* a bear, 
* a panda 

Every  simulation time unit, if any of these animals are 
* at the NESTING site, then it populates 1 of its kind at that site.
* at the WINTERING site, 
    * All die with 0.5 probability or live and move to a neighboring random location.
* at the FEEDING site, 
    * All stay there with 0.8 probability or move to a neighboring random location.


### Hunter
In the simulation, there are also randomly moving hunters, if they move to a site, then they kill all the animals in that site. The number of kills adds to their points. 
```
typedef struct{
   int points;
   Location location;
} Hunter;
```

The number of hunters are given at the beginning of the program as a command line argument:
``./main 2`` creates 2 hunters.


### (This is given) printgrid

This prints the grid with live animal counts and hunter as follows:
| Bear: 1, Bird: 2,  Panda: 0, Hunters: 1| Bear: 0, Bird: 2,  Panda: 10, Hunters: 2|  

### Simulation (main thread)
Simulation creates 1 thread for each animal, 1 thread for each hunter. **Each thread makes a move then sleeps for 1 millisecond**. 
The simulation ends after 1 second.

## What you need to do
Basically what you need to do is:
  - complete the main function, so that it creates 1 thread for each animal, 1 thread for each hunter
  - complete the function ``void *simulateanimal(void *args);`` that is run by any of the animal threads
  - complete the function ``void *simulatehunter(void *args);`` that is run by any of the hunter threads.

### Dying of an animal, 

if an animal status is made DEAD by itself or by hunters, then its associated thread should free all its resources and terminate its running. 

Note: You can do this by letting the thread check its status before running/moving.

## Grading
~33 main
~33 hunter
~33 animal

I may add some tests later for auto grading.. You may need to submit multiple times.

I have a limited github, so it is best you complete the assignment on your computer then submit it from github, if it is possible.. 

