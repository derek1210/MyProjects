#Look for #IMPLEMENT tags in this file. These tags indicate what has
#to be implemented to complete the Sokoban warehouse domain.

#   You may add only standard python imports---i.e., ones that are automatically
#   available on TEACH.CS
#   You may not remove any imports.
#   You may not import or otherwise source any of your own files

#import os for time functions
from search import * #for search engines
from FlowFree import FlowFreeState, Direction, PROBLEMS, flow_free_goal_state #for Sokoban specific classes and problems
import math

UP = Direction("up", (0, -1))
RIGHT = Direction("right", (1, 0))
DOWN = Direction("down", (0, 1))
LEFT = Direction("left", (-1, 0))

def heur_manhattan_distance(state):
#IMPLEMENT
    '''admissible sokoban heuristic: manhattan distance'''
    '''INPUT: a sokoban state'''
    '''OUTPUT: a numeric value that serves as an estimate of the distance of the state to the goal.'''      
    #We want an admissible heuristic, which is an optimistic heuristic. 
    #It must always underestimate the cost to get from the current state to the goal.
    #The sum Manhattan distance of the boxes to their closest storage spaces is such a heuristic.  
    total = 0
    for key in state.robots.keys():
      robot = state.robots[key]
      goal = state.goals[key]      
      total += abs(robot[0]-goal[0]) + abs(robot[1]-goal[1])
    return total

def heur_alternate(state):
    '''flow free heuristic'''
    '''INPUT: a flow free state'''
    '''OUTPUT: a numeric value'''      
    # This hueristic  is our main heuristic which uses 4 strategies,
    # 1. Calculating distance from robot to goal
    # 2. Prioritize robots walking on the side
    # 3. Ruling out dead state
    # 4. Solving robots 1 by 1
    # details on how those heuristic helps are in the report
    total = 0
    for key in state.robots.keys():
      robot = state.robots[key]
      goal = state.goals[key]      
      if state.robots[key] != state.goals[key]:
        total += 500            
        # here we add heuristic of strategy 4
      total += math.log1p(abs(robot[0]-goal[0]) + abs(robot[1]-goal[1]))
      # here we add heuristic of strategy 1
      if state.goals[key] in state.filled or state.goals[key] in state.robots.values():
        if state.robots[key] != state.goals[key]:
          total += 10000      
          # here we add heuristic of strategy 3
      if state.robots[key] != state.goals[key]:
        assume = True
        all_filled = True      
        on_side = False
        for direction in (UP, RIGHT, DOWN, LEFT):
          if direction.move(state.robots[key]) in state.filled or direction.move(state.robots[key]) in state.robots.values():
            assume = False
          else:
            all_filled = False
            
          if direction.move(state.goals[key]) in state.filled or direction.move(state.goals[key]) in state.robots.values():
            assume = False       
          else:
            all_filled = False
            
          if (direction.move(state.robots[key])[0] < 0 or
              direction.move(state.robots[key])[0] >= state.width or
              direction.move(state.robots[key])[1] < 0 or
              direction.move(state.robots[key])[1] >= state.height):
            on_side = False
            
        if not assume:
          total += 15
        # here we add heuristic of strategy 4
          
        if not on_side:
          total += 2
        # here we add heuristic of strategy 2

        if all_filled:
          total += 10000
        # here we add heuristic of strategy 3
    return total

if __name__ == "__main__":
  #TEST CODE
  solved = 0; unsolved = []; counter = 0; percent = 0; timebound = 4; # 4 second time limit for each problem
  print("*************************************")  
  print("Running A-star")     

  for i in range(0,35): 

    print("*************************************")  
    print("PROBLEM {}".format(i))
    
    s0 = PROBLEMS[i] #Problems will get harder as i gets bigger

    se = SearchEngine('astar', 'full')
    final = se.search(s0, flow_free_goal_state, heur_alternate, timebound)

    if final:
      final.print_path()
      solved += 1
    else:
      unsolved.append(i)    
    counter += 1

  if counter > 0:  
    percent = (solved/counter)*100

  print("*************************************")  
  print("{} of {} problems ({} %) solved in less than {} seconds.".format(solved, counter, percent, timebound))  
  print("Problems that remain unsolved in the set are Problems: {}".format(unsolved))      
  print("*************************************") 