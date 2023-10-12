# STM32 Overclocking Challenge

This repository contains my solutions to [a challenge on Sisoog website](https://sisoog.com/2023/09/28/%d9%85%d8%b3%d8%a7%d8%a8%d9%82%d9%87-%d9%87%d9%81%d8%aa%d9%85-%d8%b3%db%8c%d8%b3%d9%88%da%af/).

**⚠️ Disclaimer:** Overclocking may cause the microcontroller to overheat and result in damage. Therefore, please exercise caution and use the material in this repository **AT YOUR OWN RISK! ⚠️**

## Challenge Statement

**Write a program for the STM32F103C8T6 microcontroller to toggle the PC14 pin at the highest possible frequency.**

![272326556-164b9463-871c-4aa6-9023-f01f0d98a446](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/114b060f-806e-4e4a-8941-5108e9b533d2)

The program should meet the following criteria:

- The program that achieves the highest frequency on PC14 will be considered the winner.
- The generated signal on PC14 should maintain the same frequency for a minimum of 10 minutes.
- It is recommended to use a [Blue-Pill board](https://stm32world.com/wiki/Blue_Pill) as the hardware for this challenge. If required, you are allowed to replace the crystal on the board.

## My Solutions

The challenge statement forces us to generate the output signal using only PC14. This is because PC14 cannot be used as the MCO, PWM, or timer outputs, so we need to find alternative methods to create a high-frequency pulse on PC14.

![image](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/05ac6b93-2cb2-453f-8a15-fe77cf780374)

![image](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/6b9128d0-1c74-4d3f-a261-b26cebb90b91)

### Solution 1

We start the process of overclocking the STM32F103C8T8 microcontroller with an external crystal of 8 MHz and setting the clock speed to 72 MHz.

![image](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/f0a820c3-2395-46ac-b918-b859f24ebb72)

We set the PC14 pin as a Push-Pull output without Pull-Up and connect it to the oscilloscope to see the output pulse frequency.

![image](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/d6454220-fd6c-4657-be15-5e7ad93566f2)

![272318635-e9cd48f6-4b72-4660-af47-110bf5e82bf9](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/3f35106d-5dea-48ea-bd1c-421198756413)

We write the program in such a way that the toggling of PC14 pin is done as quickly as possible. That's why we are using assembly instructions.

The fastest way to toggle a bit in a register is to use the XOR operation. So, before the `while` loop, we store the address of the `GPIOC_ODR` register and the mask needed to change the value of 14th bit of the PORTC in the general-purpose registers:

```c
asm volatile(
    "ldr r0, =0x4001100C\n" // GPIOC_ODR address
    "ldr r1, [r0]\n"        // Load GPIOC_ODR value
    "ldr r2, =0x00004000\n" // Bit 14 mask
);
```

The address of `GPIOC_ODR` is adopted from the [RM0008](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) document.

![image](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/0ebdd1aa-217a-4f8e-ae39-459fe5603a0e)

![image](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/498ad55a-3047-4a6e-88ab-1a1350781670)

Then, inside the `while` loop, we first perform the XOR operation between the mask and the current value of the GPIOC_ODR register using the `EOR` instruction, and then write the resulting value to the `GPIOC_ODR` register using the `STR` instruction:

```c
asm volatile(
    "eor r1, r1, r2\n" // XOR the value with the mask
    "str r1, [r0]\n"   // Store the new value back to GPIOC_ODR
);
```

In this way, in a short time, the PC14 pin is toggled in the `while` loop, and a high frequency output pulse will be obtained.

By setting `SYSCLK = 72 MHz`, the output pulse frequency on PC14 will have a frequency about **5.13 MHz**:

![272320556-3f8946e6-e471-4bcd-942c-365c4f05b9d1](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/62400552-84ea-4607-bd5f-a85c4bef8942)

Now we step by step increase the clock of the microcontroller, until it eventually reaches a point where the output signal on PC14 becomes unstable.

For `SYSCLK = 80 MHz`, the frequency of the output pulse on PC14 reaches approximately **5.71 MHz**:

![272322727-5b7ed9fc-c93c-4111-9049-84f94bee5608](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/2df68053-819c-41ec-b37c-f6608e9e642a)

For `SYSCLK = 88 MHz`, the frequency of PC14 reaches about **6.29 MHz**:

![272323179-467b4e38-9a2f-4574-8eb9-4f4194cc15c5](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/307e6bf3-32f3-4990-ba30-f8877ef58a9d)

For `SYSCLK = 96 MHz`, the output pulse frequency on PC14 reaches about **6.94 MHz**:

![272323240-91792af6-8377-4fcd-adf8-3c8715c8dfa8](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/44e07946-1b4a-4a1f-9a9d-bd6e92e5d88b)

For `SYSCLK = 104 MHz`, the output pulse frequency on PC14 reaches about **7.46 MHz**:

![272323358-172efa03-de96-4679-b139-10cb421c8ed8](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/9e2c803b-ebb7-4e1e-9721-7d95e578afe0)

For `SYSCLK = 112 MHz`, the output pulse frequency on PC14 reaches about **8.0 MHz**:

![272323415-cb7b5caf-7e21-4bfa-a6c6-c5d41f34b8a6](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/e1c393f3-48c4-49eb-a879-10d8ab1752ae)

For `SYSCLK = 120 MHz`, the output pulse frequency on PC14 reaches about **8.62 MHz**:

![272323439-e707a410-fef1-4941-82ba-628c6b53bb7d](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/0db70d6a-492f-46de-bb2f-99d3d9083682)

For `SYSCLK = 128 MHz`, the output pulse frequency on PC14 reaches about **9.17 MHz**:

![272323488-d005036b-e9ed-40c7-8c18-ef32f3d984f1](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/b52199f7-8174-4603-ac12-4aeb6e2ed4ac)

128 MHz was the highest clock frequency achievable with an 8 MHz crystal. Therefore, we change the external crystal to 16 MHz so that we can reach higher clock frequencies.

![272330538-33f3ca02-53d7-43d8-b7e8-4d9710d3b592](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/e4244b24-531a-4b03-af12-02b64dca1da4)

After configuring the project for `SYSCLK = 144 MHz`, the output pulse frequency on PC14 increases to approximately **10.25 MHz**.

![272323571-58dff154-c042-475d-a193-3547b6c880b5](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/72a911da-384e-4c9d-b1a6-83e317b1e375)

**The frequency of 10.25 MHz is the highest frequency that the output remains stable for at least 10 minutes.**

Now we configure the project for `SYSCLK = 160 MHz`. In this case, the output pulse frequency on PC14 reaches about *11.47 MHz*, but after about 20 seconds, it becomes unstable and the output is cut off.

![272323640-4c23c660-227e-40a5-a059-5ee8ab06594d](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/48bf6bc8-f718-4b5e-ae8e-dda39a5a48d1)

From now on, by increasing the clock frequency, we do not see any pulses on PC14, which indicates that the microcontroller is not functioning properly and it is not possible to increase the clock frequency anymore.

### Solution 2

By changing the code and using the `EORS` instruction instead of `EOR`, the output frequency increased slightly. The results are as follows:

```c
asm volatile(
  "eors r1, r2\n"  // XOR the last value of GPIOC_ODR (r1) with the mask (r2)
  "str r1, [r0]\n" // Store the new value (r1) back to the address of GPIOC_ODR
);
```

External crystal = 8 MHz, `SYSCLK = 128 MHz`, output frequency on PC14 = **10.64 MHz** stable:

![272329966-896e1fd8-91a3-48c1-ac06-57b7887ba799](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/7d2e6f3a-1edb-49e7-9e91-783e2df91d13)

**External crystal = 16 MHz, `SYSCLK = 144 MHz`, output frequency on PC14 = 12.05 MHz stable:**

![272329999-6fffa32e-d92c-4eac-a8f7-8cba81678187](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/f8e5ef80-3ae1-418a-a2d7-cb62ceb9c5d9)

External crystal = 16 MHz, `SYSCLK = 160 MHz`, output frequency on PC14 = *13.33 MHz* Unstable (lasts only about 20 seconds):

![272330094-6384566f-a16d-43df-b01d-5dd7a625acd3](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/09b97209-4b9e-47d0-a1b5-fe13cc5c4b4b)

### Solution 3

To increase the pulse frequency, before entering the `while` loop, the corresponding values for GPIOC_ODR to set the PC14 pin to `0` and `1` is stored in two genral-purpose registers r1 and r2:

```c
asm volatile(
  "ldr r0, =0x4001100C\n" // Load GPIOC_ODR address into register r0
  "ldr r1, =0x00000000\n" // Move the value for PC14 = 0 into register r1
  "ldr r2, =0x00004000\n" // Move the value for PC14 = 1 into register r2
);
```

Inside the `while` loop, we use two `STR` commands to set PC14 to zero and one.

```c
asm volatile(
  "str r2, [r0]\n" // Store value of r2 (PC14 = 1) to the address of GPIOC_ODR
  "str r1, [r0]\n" // Store value of r1 (PC14 = 0) to the address of GPIOC_ODR
);
```

Now, with an external crystal of 16 MHz and `SYSCLK = 144 MHz`, a frequency of **20.66 MHz** can be reached on PC14:

![272334031-7472b35f-3f81-4bf9-8d92-e28eec869349](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/a7130412-674d-4416-a69b-e7f3d41b7f2c)

The branch instruction `b` executed at the end of each `while` loop, reduces the frequency of output pulse on PC14;

![272334330-4bdf354d-4d27-4ff4-b20d-91210b51f5e8](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/40d0be88-ea62-4408-bf10-95fc0a7abcef)

To solve this problem, we toggle PC14 multiple times and sequentially in each execution of the loop:

![272334366-a43d2e2e-fb9f-4e71-b1c0-dcd5944fdd60](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/d79b0e2c-738a-4284-bb77-8344dcff051c)

**By removing the delay caused by the branch, we were able to reach a frequency of 36.23 MHz on PC14 with an external crystal of 16 MHz and setting the system clock to 144 MHz:**

![272334416-d7bea841-1a9d-40dc-a2cc-a9fab940eaa7](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/4515991b-693d-4afe-808f-9d6b8d0f30ad)

### Solution 4

The EVENTOUT function on PC14 pin can be used to achieve higher frequency. After enabling the EVENTOUT function, the value of PC14 can be set to `1` only for one clock cycle using the `SEV` instruction. In this way, half of a full pulse is created. Now, with the help of the `NOP` instruction, we create a delay for one clock cycle in the state where the value of PC14 is `0`, so that a complete pulse is created.

![272402690-35acc2ad-6d8f-4805-8509-ccb1999204ec](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/38ea0aa9-a1f4-4417-a8c6-ca6d427d19c0)

```c
asm volatile(
  "sev\n" // Set EVENTOUT pin (PC14) for one clock (and then reset it) => PC14 =1
  "nop\n" // Wait for one clock cycle and do nothing (while EVENTOUT is reset) => PC14 = 0
);
```

Similar to solution 3, it is possible to eliminate the delay caused by branch instruction `b` in the generated signal by sequentially executing `SEV` and `NOP` inside the `while` loop. The results of this program are as follows:

External crystal = 8 MHz, `SYSCLK = 128 MHz`, output signal frequency = **64.10 MHz**:

![272402498-a064a292-957b-4ec5-9bb9-a1f6d6c7f3df](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/60c68545-a74e-44cc-8256-ebd9f27a4cae)

External crystal = 16 MHz, `SYSCLK = 128 MHz`, output signal frequency = **64.10 MHz**:

![272402476-352d359a-ea74-4bd3-ac1b-2fcbf6f8c437](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/84cadb04-b9c8-4d6d-987e-d7b18fc661fb)

**External crystal = 16 MHz, `SYSCLK = 144 MHz`, output signal frequency = 72.46 MHz (highest stable frequency):**

![272402450-ce426b2e-4062-42c0-b00f-563a52a3fde5](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/8b4a3e0d-d961-45d7-acbd-53138cac5fb3)

As you can see, the quality and amplitude of the output signal is still not very good. To increase the quality of the output signal, it is necessary increase clock frequencies of all involving microcontroller units to their highest level. For this purpose, changes were made in the code as follows:

PC14 settings:

![unnamed](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/1eb071be-841b-44ee-aa2b-c2c2ce9b21cd)

Clock settings for 8 MHz crystal:

![unnamed-1](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/caee2f97-344f-465b-a6d7-da275c71af08)

Signal obtained on PC14 with 8 MHz crystal and `SYSCLK = 128 MHz`: (**64.10 MHz**)

![unnamed](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/e179a95a-af53-47c1-96e7-9c9ea371b33e)

Clock settings for 16 MHz crystal:

![unnamed](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/2cb0e150-9ed6-4296-a320-d89630e42426)

Signal obtained on PC14 with 16 MHz crystal and `SYSCLK = 128 MHz`: (**64.10 MHz**)

![unnamed](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/51a2d62a-ca19-470b-b798-dce6af37a18b)

Signal obtained on PC14 with 16 MHz crystal and `SYSCLK = 144 MHz`: (**72.46 MHz - the highest stable frequency achievable**)

![unnamed-1](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/607a5964-7e10-4dce-8cd0-19321258c102)

### Solution 5

I tried to store the whole program or some parts of it in SRAM in order to prevent microcontroller from hanging in higher clock frequencies. The results after storing the whole program in SRAM was not successful, but storing the `toggle_v5()` in SRAM caused to get a stable signal with the frequency of **80.00 MHz** while `SYSCLK = 160 MHz`.

I cannot explain why, but it worked for me in the first attempt and was stable for more than an hour. But in the next day, when I turned on the board, I saw that the output is unstable!

```c
__attribute__((section(".data"))) inline void toggle_v5(void)
{
  // Toggle PC14 using EVENTOUT function
  asm volatile(
      "sev\n" // Set EVENTOUT pin (PC14) for one clock cycle (and then reset it) => PC14 = 1
      "nop\n" // Wait for one clock cycle and do nothing (while EVENTOUT is reset) => PC14 = 0
  );
}
```

![dso_01_01_02_23_55](https://github.com/m3y54m/stm32-overclocking-challenge/assets/1549028/ce59d395-ef07-4bf8-88d2-3a6318bacd78)

## Resources

- [Getting Started with STM32 Programming](https://github.com/m3y54m/start-stm32)
- [STM32F103C8 Datasheet](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf)
- [RM0008 STM32F101xx, STM32F102xx, STM32F103xx, STM32F105xx and STM32F107xx advanced Arm®-based 32-bit MCUs](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [PM0056 STM32F10xxx/20xxx/21xxx/L1xxxx Cortex®-M3 programming manual](https://www.st.com/resource/en/programming_manual/pm0056-stm32f10xxx20xxx21xxxl1xxxx-cortexm3-programming-manual-stmicroelectronics.pdf)
- [STM32H7 (Cortex-M7) GPIO toggling](https://metebalci.com/blog/stm32h7-gpio-toggling/)
- [Executing code from RAM on STM32 ARM Microcontrollers ](https://dev.to/rajeevbharol/executing-code-from-ram-on-stm32-arm-microcontrollers-2k2l)
