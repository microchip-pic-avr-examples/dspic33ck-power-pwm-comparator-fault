# dspic33ck-power-pwm-comparator-fault, release v1.0.0

### Release Highlights
<b><u>This is the initial release demonstrating the basic configuration of Comparator and high-resolution PWM generator of the dsPIC33C family of devices for PWM shutdown during fault condition. This example demonstrates the peripheral configuration of the the PCI Fault Source coming from the Comparator output. The PCI Fault Logic is also set to shut down the PWM in latched mode during the fault event. The fault event is cleared once the fault signal is removed and the terminate fault pushbutton.


### Features Added\Updated
In this initial version the on-board LED of the dsPIC33CK Digital Power Plug-In Module is toggled with a interval period of 300 ms, when the controller is running at maximum speed of 100 MIPS.
After startup, PWM generator #1 is generating a complementary pair of 200 kHz and 20% duty cycle PWM waveforms at the PWM1H/PWM1L pins. The potentiometer P2 voltage which acts as the fault source is adjusted from 0 to 3.3V. In a condition that the fault signal is removed, the PG1 remains on shutdown and the complementary PWM waveforms returns only upon pressing on-board push button USER on the Digital Power Development Board.
