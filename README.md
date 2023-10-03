# STM32 Overclocking Challenge

This repository contains my solutions to [a challenge on Sisoog website](https://sisoog.com/2023/09/28/%d9%85%d8%b3%d8%a7%d8%a8%d9%82%d9%87-%d9%87%d9%81%d8%aa%d9%85-%d8%b3%db%8c%d8%b3%d9%88%da%af/).

## Challenge Statement

**Write a program for the STM32F103C8T6 microcontroller to toggle the PC14 pin at the highest possible frequency.**

![pc14](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/164b9463-871c-4aa6-9023-f01f0d98a446)

The program should meet the following criteria:

- The program that achieves the highest frequency on PC14 will be considered the winner.
- The generated signal on PC14 should maintain the same frequency for a minimum of 10 minutes.
- It is recommended to use a [Blue-Pill board](https://stm32world.com/wiki/Blue_Pill) as the hardware for this challenge. If required, you are allowed to replace the crystal on the board.

## Solution 1

We start the process of overclocking the STM32F103C8T8 microcontroller with an external crystal of 8 MHz and setting the clock speed to 72 MHz.

We set the PC14 pin as a Push-Pull output without Pull-Up and connect it to the oscilloscope to see the output pulse frequency.

![unnamed](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/e9cd48f6-4b72-4660-af47-110bf5e82bf9)

We write the program in such a way that the toggling of PC14 pin is done as quickly as possible. That's why we do it with assembly instructions.

The fastest way to toggle a bit in a register is to use the XOR command. For this reason, before the while loop, we store the address of the `GPIOC_ODR` register and the mask needed to change the value of bit 14 of the PORTC in the general-purpose registers:

```c
asm volatile(
    "ldr r0, =0x4001100C\n" // GPIOC_ODR address
    "ldr r1, [r0]\n"        // Load GPIOC_ODR value
    "ldr r2, =0x00004000\n" // Bit 14 mask
);
```

Then, inside the while loop, we first perform the XOR operation between the mask and the current value of the GPIOC_ODR register with the help of the `EOR` instruction, and then rewrite the resulting value in the `GPIOC_ODR` register with the `STR` instruction:

```c
asm volatile(
    "eor r1, r1, r2\n" // XOR the value with the mask
    "str r1, [r0]\n"   // Store the new value back to GPIOC_ODR
);
```

In this way, in the shortest possible time, the PC14 pin is toggled in the while loop, and the highest output pulse frequency will be obtained.

By setting the clock to 72 MHz, the output pulse frequency on PC14 reaches about 5.13 MHz:

![unnamed-1](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/3f8946e6-e471-4bcd-942c-365c4f05b9d1)

Now we step by step increase the clock of the microcontroller, until it eventually reaches a point where the output signal on PC14 becomes unstable.

When the clock is set to 80 MHz, the frequency of the output pulse on PC14 reaches approximately 5.71 MHz:

![unnamed](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/5b7ed9fc-c93c-4111-9049-84f94bee5608)

Setting the clock to 88 MHz, the output pulse frequency on PC14 reaches about 6.25 MHz:

![unnamed](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/467b4e38-9a2f-4574-8eb9-4f4194cc15c5)

By setting the clock to 96 MHz, the output pulse frequency on PC14 reaches about 6.85 MHz:

![unnamed-1](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/91792af6-8377-4fcd-adf8-3c8715c8dfa8)

By setting the clock to 104 MHz, the output pulse frequency on PC14 reaches about 7.4 MHz:

![unnamed](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/172efa03-de96-4679-b139-10cb421c8ed8)

By setting the clock to 112 MHz, the output pulse frequency on PC14 reaches about 8.0 MHz:

![unnamed-1](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/cb7b5caf-7e21-4bfa-a6c6-c5d41f34b8a6)

By setting the clock to 120 MHz, the output pulse frequency on PC14 reaches about 8.5 MHz:

![unnamed](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/e707a410-fef1-4941-82ba-628c6b53bb7d)

By setting the clock to 128 MHz, the output pulse frequency on PC14 reaches about 9.1 MHz:

![unnamed-1](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/d005036b-e9ed-40c7-8c18-ef32f3d984f1)

128 MHz was the highest clock frequency achievable with an 8 MHz crystal. Therefore, we change the external crystal to 16 MHz so that we can reach higher clock frequencies.

Now we set the clock frequency to 144 MHz. In this case, the output pulse frequency on PC14 reaches about 10.3 MHz:
![unnamed](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/58dff154-c042-475d-a193-3547b6c880b5)

The frequency of 10.3 MHz is the highest frequency that the output remains stable for at least 30 minutes.

## Solution 2

Now we set the clock frequency to 160 MHz. In this case, the output pulse frequency on PC14 reaches about 11.5 MHz, but after about 20 seconds, it becomes unstable and the output is cut off.

![unnamed-1](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/4c23c660-227e-40a5-a059-5ee8ab06594d)

From now on, by increasing the clock frequency, we do not see any pulses in the output of PC14, which indicates that the microcontroller is hanging and it is not possible to increase its working frequency anymore.
