# Software PWM user guide

## How to enable the soft pwm
## First check if below directory is present on the board
ls /sys/class/soft_pwm

## Enable PWM for any GPIO using soft_pwm module
echo pin_num > /sys/class/soft_pwm/export

## Disable PWM for any GPIO using soft_pwm module
echo pin_num > /sys/class/soft_pwm/unexport

## After export you will see pwmX directory in the /sys/class/soft_pwm, here X is pin_num, also you can see below files for accessing PWM functionality for X particular pin.

```
root@smartio:/sys/class/soft_pwm/pwm5# ls
consumers  counter    period     power      pulse      pulses     subsystem  suppliers  uevent
```

## Current configuration
cat pulse
cat pulses
cat period
cat counter #this will count the number of timer interrupts		

## how to set some config
echo 100 > period  # this will set the frequency e.g here 100 usec means 10Khz
echo 10 > pulse	   # duty cycle in microseconds
echo 100 > pulses	# this is to send the x number of pulses on the GPIO --- -1 is for inifinte pulses.


