# Voltage Switch

Originally this code was written as a voltage switch for a lead acid batter, connected to a solar pannel, that would turn on when the voltage is high, and off when low. For years it powered a pond pump, but my wife got super annoyed by the large 12v battery in the flower bed.

This is 2023, and there are lithium batters.
Acer laptop battery packs are 11.1v at 7K Ah with a TI BQ4050 Lipo state of charge chip. The chip is on I2C so the code has 
been rewitten to use this chip basing the turn on and turn off points on the state of chage. On above 70% provided the solar volate is > 12V, and off when < 30% or if the solar volate drops below 11.5v (ie its dark)

The LED flashes once the pump is on and 2x when its off.


# Battery

I am using a 9x18650 Acer batter pack with a BMS controlled by a TI BQ4050 chip. (chip data sheet. https://www.ti.com/lit/ug/sluuaq3a/sluuaq3a.pdf)

The BMS is probably a cheap clone. With the packs feet on the ground looking at the pack the pins are

     |   | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |   |
     -----------------------------------------

1,2 BatV+
3   ID  - Currently not known
4   ID  - Currently purpose not known
5   SDA
6   SCL
7   0V
8   0V

