# check grip strength

**Pre-condition: System is initialised**

### (Controller)
1. check grip strength

### (GripArmControl)
2. retrieve grip strength
3. call ROS publisher
4. compare calculation with max grip strength
5.  a. grip strength is lower than max

    b. grip strength is higher than max

### (LEDControl)
`[route a]`

Pre-condition: matrix is **not** RED


6. light matrix BLUE

`[route b]`

6. light matrix RED

### (Controller)
7. wait for next period

**Post-condition: Matrix is lit in appropriate color**
