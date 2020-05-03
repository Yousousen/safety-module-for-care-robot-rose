# check angular acceleration

**Pre-condition: System is initialised**

### (Controller)
1. check angular acceleration

### (AngularAccelerationControl)
2. retrieve angular acceleration
3. call sense hat driver
4. calculate angular velocity from angular acceleration
5. add initial angular velocity to retrieved angular velocity
6. calculate rotational energy
7. a. robot is stationary

   b. robot is moving

`[route a]`

8.  a. rotational energy is lower than max for stationary

    b. rotational energy is higher than max for stationary

`[route b]`

8.  a. rotational energy is lower than max for moving

    b. rotational energy is higher than max for moving

### (LEDControl)
`[route aa]` and `[route ba]`

Pre-condition: matrix is **not** RED


9. light matrix BLUE

`[route ab]` and `[route bb]`

9. light matrix RED

### (Controller)
10. wait for next period

**Post-condition: Matrix is lit in appropriate color**
