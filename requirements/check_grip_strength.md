# check grip position

**Pre-condition: System is initialised**

### (Controller)
1. check grip position

### (AccelerationControl)
2. retrieve grip position
3. call ROS publisher
4. a. robot is stationary

   b. robot is moving

`[route a]`

5. light matrix BLUE

`[route b]`

6.  a. arm is folded

    b. arm is **not** folded

### (LEDControl)
`[route ba]`

7. light matrix BLUE

`[route bb]`

8. light matrix RED

### (Controller)
9. wait for next period

**Post-condition: Matrix is lit in appropriate color**
