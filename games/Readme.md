# Example Games
This folder contains some example games that checks different parts of the MCTS solver. 
They are meant as a blueprint to start off implementing new games

## TicTacToe

Simple 2 player game without any random events happening.

Main characteristics:
- Low number of actions (max 9)
- Relatively low number of states
- Depth of the tree is maximal 9 as every action can only be played once

## 2048
Simple 1 player game where random events could end the game for the player

Main characteristics:
- Very low number of actions (max 4)
- Relatively high number of random events (32, spawning of a cell in any of the 16 cells and then either spawn a 2 or 4)
- Potentially very long sequence of actions possible until game ends
