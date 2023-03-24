# Voltage Switch

Originally this code was written as a voltage switch for a lead acid batter, connected to a solar pannel, that would turn on when the voltage is high, and off when low. For years it powered a pond pump, but my wife got super annoyed by the large 12v battery in the flower bed.

This is 2023, and there are lithium batters.
Acer laptop battery packs are 11.1v at 7K Ah with a TI BQ4050 Lipo state of charge chip. The chip is on I2C so the code has 
been rewitten to use this chip basing the turn on and turn off points on the state of chage. On above 70% provided the solar volate is > 12V, and off when < 30% or if the solar volate drops below 11.5v (ie its dark)

The LED flashes once the pump is on and 2x when its off.