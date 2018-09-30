'''Flow Free routines.

    A) Class FlowFreeState

    A specializion of the StateSpace Class that is tailored to the game of Flow Free.

    B) class Direction

    An encoding of the directions of movement that are possible for robots in Flow Free.

    Code also contains a list of 40 Flow Free problems for the purpose of testing.
'''

from search import *
import copy

class FlowFreeState(StateSpace):
#FlowFreeState(str(key) + " " + direction.name, self.gval + transition_cost, self, self.width, self.height, new_robots, self.goals, new_paths, new_filled)
    def __init__(self, action, gval, parent, width, height, robots, goals, paths, filled):
        '''
        Creates a new Flow Free state.
        @param width: The room's X dimension (excluding walls).
        @param height: The room's Y dimension (excluding walls).
        @param robots: A dictionary of all robots with the sink
        @param paths: A dictionary of all walked paths.
        @param filled: A set of indicies used.
        '''
        StateSpace.__init__(self, action, gval, parent)
        self.width = width
        self.height = height
        self.robots = robots
        self.goals = goals
        self.paths = paths
        self.filled = filled     
        for key in robots.keys():
            if key.lower() not in self.paths.keys():
                self.paths[key.lower()] = set()
        
    def __cmp__(self, other):
        '''
        This comparison method must be implemented to ensure deterministic results.
        @return: Negative if self < other, zero if self == other and strictly 
        positive if self > other.
        '''
        return cmp((self.robots, self.paths), (other.robots, other.paths))        

    def successors(self):
        '''
        Generates all the actions that can be performed from this state, and the states those actions will create.        
        '''
        successors = []
        transition_cost = 1

        for key in self.robots:
            for direction in (UP, RIGHT, DOWN, LEFT):
                new_location = direction.move(self.robots[key])
                if new_location[0] < 0 or new_location[0] >= self.width:
                    continue
                if new_location[1] < 0 or new_location[1] >= self.height:
                    continue
                if new_location in self.filled or new_location in self.robots.values():
                    continue 
                if new_location in self.goals.values():
                    if self.goals[key] != new_location:
                        continue
                new_paths = copy.deepcopy(self.paths)
                new_paths[key.lower()].add(self.robots[key])
                new_robots = copy.deepcopy(self.robots)
                new_robots[key] = new_location
                new_filled = copy.deepcopy(self.filled)
                new_filled.add(self.robots[key])
                new_state = FlowFreeState(str(key) + " " + direction.name, self.gval + transition_cost, self, self.width, self.height, new_robots, self.goals, new_paths, new_filled)
                successors.append(new_state)
                
            ## these are the codes for deciding to move the other side cost should be 0 we did not use it because it affect the efficiency of the search by too much
            #new_robot_location = self.goals[key]
            #new_goal_location = self.robots[key]
            #new_robots = copy.deepcopy(self.robots)         
            #new_robots[key] = new_robot_location       
            #new_goals = copy.deepcopy(self.goals)       
            #new_goals[key] = new_goal_location      
            #new_state = FlowFreeState(str(key) + " " + direction.name, self.gval, self, self.width, self.height, new_robots, new_goals, self.paths, self.filled)
            #successors.append(new_state)
            
        return successors

    def hashable_state(self):
        '''Return a data item that can be used as a dictionary key to UNIQUELY represent a state.'''
        return hash(self.state_string())       

    def state_string(self):
        '''Returns a string representation fo a state that can be printed to stdout.'''        
        map = []
        for y in range(0, self.height):
            row = []
            for x in range(0, self.width):
                row += [' ']
            map += [row]
        
        for key in self.paths.keys():
            for point in self.paths[key]:
                map[point[1]][point[0]] = key
        for key in self.robots.keys():
            map[self.robots[key][1]][self.robots[key][0]] = key
        for key in self.goals.keys():
            map[self.goals[key][1]][self.goals[key][0]] = key
        
        for y in range(0, self.height):
            map[y] = ['#'] + map[y]
            map[y] = map[y] + ['#']
        map = ['#' * (self.width + 2)] + map
        map = map + ['#' * (self.width + 2)]

        s = ''
        for row in map:
            for char in row:
                s += char
            s += '\n'

        return s        

    def print_state(self):
        '''
        Prints the string representation of the state. ASCII art FTW!
        '''        
        print("ACTION was " + self.action)      
        print(self.state_string())


def flow_free_goal_state(state):
    '''Returns True if we have reached a goal state'''
    '''INPUT: a flow free state'''
    '''OUTPUT: True (if goal) or False (if not)'''  
    for key in state.robots:
        if state.robots[key] != state.goals[key]: 
            return False
    return True

'''
Flow Free Problem Set, for testing
'''
PROBLEMS = (
    # PROBLEM 0
    FlowFreeState("START", 0, None, 3, 3, # dimensions
                  {'A': (0, 0),'B': (0, 2)}, #robots
                  {'A': (2, 0),'B': (2, 2)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 1
    FlowFreeState("START", 0, None, 3, 3, # dimensions
                  {'A': (1, 0),'B': (0, 1)}, #robots
                  {'A': (1, 1),'B': (2, 1)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 2
    FlowFreeState("START", 0, None, 3, 3, # dimensions
                  {'A': (0, 0),'B': (0, 1),'C':(2,0)}, #robots
                  {'A': (1, 0),'B': (1, 1),'C':(0,2)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 3
    FlowFreeState("START", 0, None, 5, 5, # dimensions
                  {'A': (0, 0),'B': (0, 1),'C': (0, 2),'D': (0, 3)}, #robots
                  {'A': (4, 0),'B': (4, 1),'C': (4, 2),'D': (4, 3)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 4
    FlowFreeState("START", 0, None, 5, 5, # dimensions
                  {'A': (0, 0),'B': (0, 4)}, #robots
                  {'A': (4, 0),'B': (4, 4)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 5
    FlowFreeState("START", 0, None, 5, 5, # dimensions
                  {'A': (1, 1),'B': (0, 0),'C': (2, 2)}, #robots
                  {'A': (1, 3),'B': (0, 4),'C': (3, 3)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 6
    FlowFreeState("START", 0, None, 5, 5, # dimensions
                  {'A': (0, 0),'B': (3, 3),'C': (1, 1)}, #robots
                  {'A': (4, 4),'B': (2, 2),'C': (1, 4)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 7
    FlowFreeState("START", 0, None, 5, 5, # dimensions
                  {'A': (0, 0),'B': (0, 1)}, #robots
                  {'A': (4, 0),'B': (1, 1)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 8
    FlowFreeState("START", 0, None, 5, 5, # dimensions
                     {'A': (0, 0),'B': (1, 0),'C': (0,3)}, #robots
                     {'A': (2, 0),'B': (1, 1),'C': (3,0)}, #boxes 
                     {}, # paths
                     set() # filled
                     ),
    # PROBLEM 9
    FlowFreeState("START", 0, None, 5, 5, # dimensions
                  {'A': (0, 0),'B': (0, 1),'C': (3, 0)}, #robots
                  {'A': (4, 0),'B': (1, 1),'C': (3, 3)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 10
    FlowFreeState("START", 0, None, 5, 5, # dimensions
                  {'A': (0, 0),'B': (0, 1),'C': (3, 0),'D': (1,3)}, #robots
                  {'A': (4, 0),'B': (1, 1),'C': (3, 3),'D': (2,3)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 11
    FlowFreeState("START", 0, None, 5, 5, # dimensions
                  {'A': (0, 0),'B': (3, 1),'C': (3, 0),'D': (1,3)}, #robots
                  {'A': (4, 0),'B': (2, 1),'C': (3, 3),'D': (2,3)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 12
    FlowFreeState("START", 0, None, 6, 6, # dimensions
                  {'A': (0, 0),'B': (1, 0)}, #robots
                  {'A': (5, 5),'B': (1, 1)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 13
    FlowFreeState("START", 0, None, 6, 6, # dimensions
                  {'A': (0, 0),'B': (3, 1),'C': (3, 0),'D': (1,3),'E': (0,5)}, #robots
                  {'A': (4, 0),'B': (2, 1),'C': (3, 3),'D': (2,3),'E': (5,5)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 14
    FlowFreeState("START", 0, None, 6, 6, # dimensions
                  {'A': (0, 0),'B': (3, 1),'C': (3, 0),'D': (1,3),'E': (0,5),'F': (5,0)}, #robots
                  {'A': (4, 0),'B': (2, 1),'C': (3, 3),'D': (2,3),'E': (5,5),'F': (5,4)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 15
    FlowFreeState("START", 0, None, 8, 8, # dimensions
                  {'A': (1, 1),'B': (0, 0),'C': (2, 2)}, #robots
                  {'A': (1, 6),'B': (0, 7),'C': (3, 3)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 16
    FlowFreeState("START", 0, None, 8, 8, # dimensions
                  {'A': (0, 0),'B':(1,1),'C':(5,1)}, #robots
                  {'A': (4, 4),'B':(2,2),'C':(5,5)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 17
    FlowFreeState("START", 0, None, 8, 8, # dimensions
                  {'A': (0, 0),'B':(1,1),'C':(5,1),'D':(5,7)}, #robots
                  {'A': (4, 4),'B':(2,2),'C':(5,5),'D':(7,5)}, #boxes 
                  {}, # paths
                  set() # filled
                  ),
    # PROBLEM 18
    FlowFreeState("START", 0, None, 5, 5, # dimensions
                      {'Y': (0, 0), 'G': (0, 1), 'R': (0, 4), 'B': (4, 0), 'P': (3, 1)},
                      {'Y': (1, 2), 'G': (2, 3), 'R': (3, 2), 'B': (2, 2), 'P': (4, 4)},
                      {}, # paths
                      set() # filled
                    ),
    
        # PROBLEM 19    
        FlowFreeState("START", 0, None, 5, 5, # dimensions
                      {'Y': (3, 0), 'G': (0, 4), 'R': (1, 3), 'B': (2, 1), 'P': (4, 0)},
                      {'Y': (1, 1), 'G': (2, 4), 'R': (4, 4), 'B': (0, 3), 'P': (4, 3)},
                      {}, # paths
                      set() # filled
                     ),
    # PROBLEM 20
        FlowFreeState("START", 0, None, 5, 5, # dimensions
                      {'Y': (1, 1), 'G': (4, 0), 'R': (0, 1), 'B': (2, 4), 'P': (1, 4)},
                      {'Y': (1, 3), 'G': (3, 3), 'R': (3, 1), 'B': (4, 3), 'P': (1, 2)},
                      {}, # paths
                      set() # filled
                     ),
    # PROBLEM 21
        FlowFreeState("START", 0, None, 5, 5, # dimensions
                      {'Y': (0, 4), 'G': (1, 0), 'R': (3, 1), 'B': (2, 3), 'P': (2, 0)},
                      {'Y': (1, 2), 'G': (2, 1), 'R': (4, 4), 'B': (0, 0), 'P': (4, 0)},
                      {}, # paths
                      set() # filled
                     ),
    # PROBLEM 22
        FlowFreeState("START", 0, None, 5, 5, # dimensions
                      {'Y': (0, 4), 'G': (3, 0), 'R': (0, 3), 'B': (2, 2), 'P': (3, 1)},
                      {'Y': (2, 1), 'G': (4, 4), 'R': (2, 0), 'B': (3, 4), 'P': (3, 3)},
                      {}, # paths
                      set() # filled
                     ),
    # PROBLEM 23
        FlowFreeState("START", 0, None, 5, 5, # dimensions
                      {'Y': (2, 0), 'G': (1, 2), 'R': (0, 1), 'B': (0, 0), 'P': (2, 4)},
                      {'Y': (2, 2), 'G': (4, 3), 'R': (1, 4), 'B': (3, 1), 'P': (4, 4)},
                      {}, # paths
                      set() # filled
                     ),
    # PROBLEM 24
        FlowFreeState("START", 0, None, 5, 5, # dimensions
                      {'Y': (0, 3), 'G': (4, 3), 'R': (0, 2), 'B': (3, 2), 'P': (2, 1)},
                      {'Y': (1, 1), 'G': (0, 4), 'R': (3, 1), 'B': (4, 0), 'P': (3, 3)},
                      {}, # paths
                      set() # filled
                     ),
    # PROBLEM 25
        FlowFreeState("START", 0, None, 5, 5, # dimensions
                      {'Y': (0, 0), 'G': (3, 4), 'R': (1, 0), 'B': (2, 0), 'P': (1, 4)},
                      {'Y': (0, 4), 'G': (4, 0), 'R': (3, 0), 'B': (2, 2), 'P': (2, 4)},
                      {}, # paths
                      set() # filled
                     ),
    # PROBLEM 26
        FlowFreeState("START", 0, None, 5, 5, # dimensions
                      {'Y': (1, 3), 'G': (0, 3), 'R': (0, 4), 'B': (4, 4), 'P': (2, 3)},
                      {'Y': (1, 1), 'G': (3, 0), 'R': (3, 2), 'B': (4, 2), 'P': (4, 0)},
                      {}, # paths
                      set() # filled
                     ),    
    # PROBLEM 27
        FlowFreeState("START", 0, None, 5, 5, # dimensions
                      {'Y': (3, 1), 'G': (3, 3), 'R': (2, 3), 'B': (3, 4), 'P': (0, 3)},
                      {'Y': (1, 3), 'G': (4, 4), 'R': (2, 3), 'B': (0, 4), 'P': (4, 2)},
                      {}, # paths
                      set() # filled
                     ),
    # PROBLEM 28
        FlowFreeState("START", 0, None, 6, 6, # dimensions
                      {'Y': (2, 0), 'G': (3, 1), 'R': (1, 0), 'B': (5, 5), 'P': (3, 5), 'D': (4, 1)},
                      {'Y': (5, 2), 'G': (0, 5), 'R': (0, 4), 'B': (3, 2), 'P': (2, 2), 'D': (5, 4)},
                      {}, # paths
                      set() # filled
                     ),
    # PROBLEM 29
        FlowFreeState("START", 0, None, 6, 6, # dimensions
                      {'Y': (0, 0), 'G': (4, 5), 'R': (1, 1), 'B': (3, 5), 'P': (1, 3), 'D': (2, 1)},
                      {'Y': (2, 3), 'G': (3, 2), 'R': (5, 1), 'B': (0, 3), 'P': (4, 4), 'D': (5, 2)},
                      {}, # paths
                      set() # filled
                     ),
    # PROBLEM 30
        FlowFreeState("START", 0, None, 6, 6, # dimensions
                      {'Y': (1, 4), 'G': (4, 4), 'R': (4, 5), 'B': (0, 3), 'P': (5, 1)},
                      {'Y': (4, 1), 'G': (0, 4), 'R': (2, 4), 'B': (5, 0), 'P': (2, 2)},
                      {}, # paths
                      set() # filled
                     ),
    # PROBLEM 31
        FlowFreeState("START", 0, None, 6, 6, # dimensions
                      {'Y': (1, 2), 'G': (5, 5), 'R': (1, 4), 'B': (4, 1), 'P': (0, 0), 'D': (0, 4)},
                      {'Y': (5, 2), 'G': (3, 2), 'R': (4, 5), 'B': (2, 2), 'P': (3, 3), 'D': (3, 5)},
                      {}, # paths
                      set() # filled
                     ),
    # PROBLEM 32
        FlowFreeState("START", 0, None, 7, 7, # dimensions
                      {'Y': (4, 4), 'G': (0, 0), 'R': (5, 2), 'B': (5, 5), 'P': (0, 6), 'D': (2, 6)},
                      {'Y': (4, 6), 'G': (2, 3), 'R': (3, 4), 'B': (3, 6), 'P': (5, 1), 'D': (1, 6)},
                      {}, # paths
                      set() # filled
                     ),
    # PROBLEM 33
        FlowFreeState("START", 0, None, 7, 7, # dimensions
                      {'Y': (1, 0), 'G': (1, 2), 'R': (6, 0), 'B': (1, 4), 'P': (5, 2), 'D': (0, 0)},
                      {'Y': (3, 3), 'G': (2, 3), 'R': (0, 6), 'B': (5, 3), 'P': (2, 0), 'D': (5, 5)},
                      {}, # paths
                      set() # filled
                     ),
    # PROBLEM 34
        FlowFreeState("START", 0, None, 7, 7, # dimensions
                      {'Y': (6, 0), 'G': (1, 4), 'R': (1, 2), 'B': (5, 3), 'P': (0, 4), 'D': (4, 6), 'O': (0, 0)},
                      {'Y': (2, 4), 'G': (5, 5), 'R': (5, 2), 'B': (2, 2), 'P': (3, 6), 'D': (6, 5), 'O': (3, 2)},
                      {}, # paths
                      set() # filled
                     )    
)


'''
FlowFree Directions: encodes directions of movement that are possible for each robot.
'''
class Direction():
    '''
    A direction of movement.
    '''
    
    def __init__(self, name, delta):
        '''
        Creates a new direction.
        @param name: The direction's name.
        @param delta: The coordinate modification needed for moving in the specified direction.
        '''
        self.name = name
        self.delta = delta
    
    def __cmp__(self, other):
        '''
        The comparison method must be implemented to ensure deterministic results.
        @return: Negative if self < other, zero if self == other and strictly 
        positive if self > other.
        '''
        return cmp(self.name, other.name)
    
    def __hash__(self):
        '''
        The hash method must be implemented for actions to be inserted into sets 
        and dictionaries.
        @return: The hash value of the action.
        '''
        return hash(self.name)
    
    def __str__(self):
        '''
        @return: The string representation of this object when *str* is called.
        '''
        return str(self.name)
    
    def __repr__(self):
        return self.__str__()
    
    def move(self, location):
        '''
        @return: Moving from the given location in this direction will result in the returned location.
        '''
        return (location[0] + self.delta[0], location[1] + self.delta[1])


#Global Directions
UP = Direction("up", (0, -1))
RIGHT = Direction("right", (1, 0))
DOWN = Direction("down", (0, 1))
LEFT = Direction("left", (-1, 0))



def print_all_problems():
    n = 0
    for problem in PROBLEMS:
        print("PROBLEM " + str(n) + ":")
        problem.print_state()
        n += 1
