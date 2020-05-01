# check grip position

Pre-condition: System is initialised

###### (Controller)
1. check grip position

###### (GripArmControl)
2. retrieve grip position
3. call ROS publisher
7. compare calculation with max grip position
8.  a. grip position is lower than max

    b. grip position is higher than max

###### (LEDControl)
[route a]
9a. light matrix BLUE

[route b]
9b. light matrix RED

###### (Controller)
10. wait for next period

Post-condition: Matrix is lit in appropriate color
