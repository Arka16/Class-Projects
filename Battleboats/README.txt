Battle Boats: (C)


Description:  Hasbro board game Battleship designed for Uno32 microcontrollers; Game is single-player,turn-based, and an artificial-intelligence agent opponent; utilizes random number generation to select turns



Main files:

-AGENT.C: responsible for overall game functionality and structure

-MESSAGE.C: responsible for handling/parsing different types of messages used in the game

-NEGOTIATION.C: Contains functions that use random number generation and hashing to decide who goes first, select turns of ai agent, and detect instances of cheating 



Test Harnesses:

-AGENTTEST.C

-MESSAGETEST.C

-NEGOTIATIONTEST.C


