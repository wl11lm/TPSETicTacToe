#include "interrupt.h"

bool flag_gpio[32][4] = {false};

// vetor com as 8 interrupcoes dos modulos, 0 se estiver desativado e pin se ativado
ucPinNumber pins[8] = {0};

void gpioInterruptInit(gpioMod gpio, ucPinNumber pin, endnameInterrupt name)
{
    gpioIsrConfig(gpio, name);
    gpioSetInterrupt(gpio, pin);
    gpioEnableInterrupt(gpio, pin);

    switch (gpio)
    {
    case GPIO0:
        if (name == A)
            pins[0] = pin;
        else if (name == B)
            pins[1] = pin;
        break;
    case GPIO1:
        if (name == A)
            pins[2] = pin;
        else if (name == B)
            pins[3] = pin;
        break;
    case GPIO2:
        if (name == A)
            pins[4] = pin;
        else if (name == B)
            pins[5] = pin;
        break;
    case GPIO3:
        if (name == A)
            pins[6] = pin;
        else if (name == B)
            pins[7] = pin;
        break;
    default:
        break;
    }
}

void gpioIsrConfig(gpioMod mod, endnameInterrupt name)
{
    switch (mod)
    {
    case GPIO0:
        if (name == A)
            HWREG(INTCPS + INTC_MIR_CLEAR3) |= (1 << 0);
        else if (name == B)
            HWREG(INTCPS + INTC_MIR_CLEAR3) |= (1 << 1);
        break;

    case GPIO1:
        if (name == A)
            HWREG(INTCPS + INTC_MIR_CLEAR3) |= (1 << 2);
        else if (name == B)
            HWREG(INTCPS + INTC_MIR_CLEAR3) |= (1 << 3);
        break;

    case GPIO2:
        if (name == A)
            HWREG(INTCPS + INTC_MIR_CLEAR1) |= (1 << 0);
        else if (name == B)
            HWREG(INTCPS + INTC_MIR_CLEAR1) |= (1 << 1);
        break;

    case GPIO3:
        if (name == A)
            HWREG(INTCPS + INTC_MIR_CLEAR1) |= (1 << 30);
        else if (name == B)
            HWREG(INTCPS + INTC_MIR_CLEAR1) |= (1 << 31);
        break;

    default:
        break;
    }
}

void gpioSetInterrupt(gpioMod gpio, ucPinNumber pin)
{
    /* Setting interrupt GPIO pin. */
    switch (gpio)
    {
    case GPIO0:
        HWREG(SOC_GPIO_0_REGS + GPIO_IRQSTATUS_SET_0) |= 1 << pin;
        break;
    case GPIO1:
        HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_SET_0) |= 1 << pin;
        break;
    case GPIO2:
        HWREG(SOC_GPIO_2_REGS + GPIO_IRQSTATUS_SET_0) |= 1 << pin;
        break;
    case GPIO3:
        HWREG(SOC_GPIO_3_REGS + GPIO_IRQSTATUS_SET_0) |= 1 << pin;
        break;
    default:
        break;
    }
}

void gpioEnableInterrupt(gpioMod gpio, ucPinNumber pin)
{
    /* Enable interrupt generation on detection of a rising edge.*/
    switch (gpio)
    {
    case GPIO0:
        HWREG(SOC_GPIO_0_REGS + GPIO_RISINGDETECT) |= 1 << pin;
        HWREG(SOC_GPIO_0_REGS + GPIO_FALLINGDETECT) |= (1 << pin);
        break;
    case GPIO1:
        HWREG(SOC_GPIO_1_REGS + GPIO_RISINGDETECT) |= 1 << pin;
        HWREG(SOC_GPIO_1_REGS + GPIO_FALLINGDETECT) |= (1 << pin);
        break;
    case GPIO2:
        HWREG(SOC_GPIO_2_REGS + GPIO_RISINGDETECT) |= 1 << pin;
        HWREG(SOC_GPIO_2_REGS + GPIO_FALLINGDETECT) |= (1 << pin);
        break;
    case GPIO3:
        HWREG(SOC_GPIO_3_REGS + GPIO_RISINGDETECT) |= 1 << pin;
        HWREG(SOC_GPIO_3_REGS + GPIO_FALLINGDETECT) |= (1 << pin);
        break;
    default:
        break;
    }
}

void gpioIsrHandler(gpioMod mod, ucPinNumber pin)
{
    /* Clear the status of the interrupt flags */
    HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_0) |= (0x1 << 28);
    flag_gpio[pin][mod] = true;

    switch (mod)
    {
    case GPIO0:
        HWREG(SOC_GPIO_0_REGS + GPIO_IRQSTATUS_0) = 1 << pin;
        break;

    case GPIO1:
        HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_0) = 1 << pin;
        flag_gpio[pin][mod] = true;
        break;

    case GPIO2:
        HWREG(SOC_GPIO_2_REGS + GPIO_IRQSTATUS_0) = 1 << pin;
        break;

    case GPIO3:
        HWREG(SOC_GPIO_3_REGS + GPIO_IRQSTATUS_0) = 1 << pin;
        break;

    default:
        break;
    }
}

void timerIrqHandler(void)
{
    /* Clear the status of the interrupt flags */
    HWREG(SOC_DMTIMER_7_REGS + IRQSTATUS) = 0x2;

    timerSetFlag(true);

    /* Stop the DMTimer */
    timerDisable(SOC_DMTIMER_7_REGS);
}

// void ISR_Handler(void)
// {
//     /* Verify active IRQ number */
//     unsigned int irq_number = HWREG(INTCPS + INTC_SIR_IRQ) & 0x7f;

//     for (int i = 0; i < 8; i++)
//     {
//         if (pins[i] != 0 && ((i == 0 && irq_number == 96) || (i == 1 && irq_number == 97)))
//             gpioIsrHandler(GPIO0, pins[i]);
//         if (pins[i] != 0 && ((i == 2 && irq_number == 98) || (i == 3 && irq_number == 99)))
//             gpioIsrHandler(GPIO1, pins[i]);
//         if (pins[i] != 0 && ((i == 4 && irq_number == 32) || (i == 5 && irq_number == 33)))
//             gpioIsrHandler(GPIO2, pins[i]);
//         if (pins[i] != 0 && ((i == 6 && irq_number == 62) || (i == 7 && irq_number == 63)))
//             gpioIsrHandler(GPIO3, pins[i]);
//     }

//     if (irq_number == 95)
//         timerIrqHandler();

//     /* acknowledge IRQ */
//     HWREG(INTCPS + INTC_CONTROL) = 0x1;
// }

void ISR_Handler(void)
{
    /* Verify active IRQ number */
    unsigned int irq_number = HWREG(INTCPS + INTC_SIR_IRQ) & 0x7f;

    if (irq_number == 98)
    {
        if (HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_RAW_0) & 1 << 6)
        {
            gpioIsrHandler(GPIO1, 6);
        }
        else if (HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_RAW_0) & 1 << 7)
        {
            gpioIsrHandler(GPIO1, 7);
        }
        else if (HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_RAW_0) & 1 << 2)
        {
            gpioIsrHandler(GPIO1, 2);
        }
        else if (HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_RAW_0) & 1 << 14)
        {
            gpioIsrHandler(GPIO1, 14);
        }
        else if (HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_RAW_0) & 1 << 28)
        {
            gpioIsrHandler(GPIO1, 28);
        }
        else if (HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_RAW_0) & 1 << 19)
        {
            gpioIsrHandler(GPIO1, 19);
        }
        else if (HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_RAW_0) & 1 << 18)
        {
            gpioIsrHandler(GPIO1, 18);
        }
        else if (HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_RAW_0) & 1 << 16)
        {
            gpioIsrHandler(GPIO1, 16);
        }
        else if (HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_RAW_0) & 1 << 17)
        {
            gpioIsrHandler(GPIO1, 17);
        }
    }
    if (irq_number == 95)
        timerIrqHandler();

    /* acknowledge IRQ */
    HWREG(INTCPS + INTC_CONTROL) = 0x1;
}

bool flagGet(gpioMod mod, ucPinNumber pin)
{
    return flag_gpio[pin][mod];
}

void flagSetFalse(gpioMod mod, ucPinNumber pin)
{
    flag_gpio[pin][mod] = false;
}

///////////////////////////////////////////////////
