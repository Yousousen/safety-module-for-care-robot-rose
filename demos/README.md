# KE and RE demos

Here are demos showing the kinetic energy check and rotational energy check at work.

## Kinetic energy check

Let us start with only the kinetic energy check. In this case the other checks returns `safe` by default.

We increase the linear acceleration until the kinetic energy received from a change in acceleration is too high. When that happens the matrix turns red:

![increasing acceleratio](https://github.com/Yousousen/safety-module-for-care-robot-rose/blob/master/demos/acc_only/increasing_acceleration.gif)

We can see it does not matter how high our angular velocity gets if we only check for kinetic energy, only high linear acceleration makes the LED turn red:

![angular velocity does nothing](https://github.com/Yousousen/safety-module-for-care-robot-rose/blob/master/demos/acc_only/demo_showing_linear_velocity_only_angular_velocity_does_nothing.gif)

Acceleration is checked in all Cartesian directions. Here in the Z-direction:

![z-direction linear acceleration](https://github.com/Yousousen/safety-module-for-care-robot-rose/blob/master/demos/acc_only/z_direction_acc.gif)

## Rotational energy check

Here we show the rotational energy check only.Now the kinetic energy check and the other checks return `safe` by default.

![ang vel demo](https://github.com/Yousousen/safety-module-for-care-robot-rose/blob/master/demos/ang_vel_only/ang_vel_good_demo.gif)
![ang vel demo2](https://github.com/Yousousen/safety-module-for-care-robot-rose/blob/master/demos/ang_vel_only/ang_vel_good_demo2.gif)

High linear acceleration does not make the matrix red now:

![linear acceleration does nothing](https://github.com/Yousousen/safety-module-for-care-robot-rose/blob/master/demos/ang_vel_only/linear_velocity_does_nothing.gif)

Sometimes there is a small delay in lighting the matrix. We think it is due to the non-optimal period of the executed checks:

![delay](https://github.com/Yousousen/safety-module-for-care-robot-rose/blob/master/demos/ang_vel_only/ang_vel_there_is_still_some_delay.gif)

## Both checks at work

We can do both check at once as well. When one of the two checks measures excessive energy the matrix needs to be reset.

![both](https://github.com/Yousousen/safety-module-for-care-robot-rose/blob/master/demos/both/both.gif)
![both2](https://github.com/Yousousen/safety-module-for-care-robot-rose/blob/master/demos/both/both2.gif)
