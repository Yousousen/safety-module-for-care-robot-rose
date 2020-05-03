# check acceleration

**Pre-condition: System is initialised**

### (Controller)
1. check acceleration

### (AccelerationControl)
2. retrieve acceleration
3. call sense hat driver
4. calculate velocity from acceleration
5. add initial velocity to retrieved velocity
6. calculate kinetic energy
7. a. robot is stationary

   b. robot is moving

`[route a]`

8.  a. kinetic energy is lower than max for stationary

    b. kinetic energy is higher than max for stationary

`[route b]`

8.  a. kinetic energy is lower than max for moving

    b. kinetic energy is higher than max for moving

### (LEDControl)
`[route aa]` and `[route ba]`

Pre-condition: matrix is **not** RED


9. light matrix BLUE

`[route ab]` and `[route bb]`

9. light matrix RED

### (Controller)
10. wait for next period

**Post-condition: Matrix is lit in appropriate color**
