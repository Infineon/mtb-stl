# Communication Interface Self-Tests

This guide covers serial SCB self-tests, the UART Data Transfer message protocol, and fieldbus communication checks.

## I2C SCB test

The I2C SCB self-test validates a master/slave communication path using a dedicated pair of I2C instances that are wired together externally. The master periodically sends a byte to the slave; the slave returns its 1's complement, and the master verifies the value read back. A mismatch or a stalled transfer is reported as an error.

### Configuration

In the Device Configurator, enable two I2C SCB instances and wire them together:

- One instance in master mode and one instance in slave mode. Give each an alias of your choice; the application code must reference the same names. This guide uses `CYBSP_DUT_I2C_MASTER` and `CYBSP_DUT_I2C_SLAVE` as examples.
- Set the slave instance address to `0x08`. The self-test always addresses the fixed `I2C_SLAVE_ADDR` (`0x08`, defined in `SelfTest_I2C_SCB.h`) from the master, so the slave must answer at that address.
- The data rate is not constrained by the self-test; use any standard I2C rate. The master drives the bus, and each transfer is a single byte that must complete within the internal guard time.
- Wire the two instances together externally: connect SDA to SDA and SCL to SCL, and add pull-up resistors on both lines.

### Application resources and interrupt handlers

Declare these application-scope resources and interrupt handlers before the application initialization or test function:

```c
/* Test resources: I2C master/slave contexts and the slave read/write buffers. */
static cy_stc_scb_i2c_context_t i2cMasterContext;
static cy_stc_scb_i2c_context_t i2cSlaveContext;
static uint8_t i2cSlaveReadBuffer[PACKET_SIZE];
static uint8_t i2cSlaveWriteBuffer[PACKET_SIZE];

/* Both SCB interrupts are serviced by the PDL I2C interrupt handler. */
static void I2C_Master_Isr(void)
{
    Cy_SCB_I2C_Interrupt(CYBSP_DUT_I2C_MASTER_HW, &i2cMasterContext);
}

static void I2C_Slave_Isr(void)
{
    Cy_SCB_I2C_Interrupt(CYBSP_DUT_I2C_SLAVE_HW, &i2cSlaveContext);
}
```

### Application-side test sequence

The example initializes both instances, installs the shared interrupt handlers, and then calls `SelfTest_I2C_SCB()` repeatedly until it reports completion:

```c
uint8_t status = OK_STATUS;

const cy_stc_sysint_t masterIrqCfg =
{
    .intrSrc      = CYBSP_DUT_I2C_MASTER_IRQ,
    .intrPriority = 3U
};
const cy_stc_sysint_t slaveIrqCfg =
{
    .intrSrc      = CYBSP_DUT_I2C_SLAVE_IRQ,
    .intrPriority = 3U
};

/* Initialize and enable the I2C master, then hook its interrupt. */
if (CY_SCB_I2C_SUCCESS != Cy_SCB_I2C_Init(CYBSP_DUT_I2C_MASTER_HW,
                                          &CYBSP_DUT_I2C_MASTER_config,
                                          &i2cMasterContext))
{
    status = ERROR_STATUS;
}
(void)Cy_SysInt_Init(&masterIrqCfg, &I2C_Master_Isr);
NVIC_EnableIRQ((IRQn_Type)masterIrqCfg.intrSrc);
Cy_SCB_I2C_Enable(CYBSP_DUT_I2C_MASTER_HW);

/* Initialize the I2C slave, configure its read/write buffers, hook interrupt. */
if (CY_SCB_I2C_SUCCESS != Cy_SCB_I2C_Init(CYBSP_DUT_I2C_SLAVE_HW,
                                          &CYBSP_DUT_I2C_SLAVE_config,
                                          &i2cSlaveContext))
{
    status = ERROR_STATUS;
}
Cy_SCB_I2C_SlaveConfigReadBuf(CYBSP_DUT_I2C_SLAVE_HW, i2cSlaveReadBuffer,
                              PACKET_SIZE, &i2cSlaveContext);
Cy_SCB_I2C_SlaveConfigWriteBuf(CYBSP_DUT_I2C_SLAVE_HW, i2cSlaveWriteBuffer,
                               PACKET_SIZE, &i2cSlaveContext);
(void)Cy_SysInt_Init(&slaveIrqCfg, &I2C_Slave_Isr);
NVIC_EnableIRQ((IRQn_Type)slaveIrqCfg.intrSrc);
Cy_SCB_I2C_Enable(CYBSP_DUT_I2C_SLAVE_HW);

__enable_irq();

/* Run the master/slave self-test until it completes. Each call sends one byte
 * and verifies the slave returned its 1's complement. */
while (OK_STATUS == status)
{
    status = SelfTest_I2C_SCB(CYBSP_DUT_I2C_MASTER_HW, &i2cMasterContext,
                              CYBSP_DUT_I2C_SLAVE_HW, &i2cSlaveContext,
                              i2cSlaveReadBuffer, i2cSlaveWriteBuffer);
    if (PASS_COMPLETE_STATUS == status)
    {
        /* I2C self-test passed */
        break;
    }
    else if (PASS_STILL_TESTING_STATUS == status)
    {
        /* Continue testing */
        status = OK_STATUS;
    }
    else
    {
        /* Handle I2C self-test failure (check wiring and pull-up resistors) */
    }
}
```

## SPI SCB test

The SPI SCB self-test writes a byte and expects to read the same byte back over an internal loopback, stepping through all values from `0x00` to `0xFF`. A mismatch, a transmit-FIFO that never empties, or a receive-FIFO that stays empty is reported as an error.

### Configuration

- Configure one SPI SCB instance in master mode, so it generates the clock and slave-select. Give it an alias of your choice; the application code must reference the same name. This guide uses `CYBSP_DUT_SPI` as an example.
- Provide a loopback that feeds MOSI back into MISO. On a device with Smart I/O (such as PSOC™ Control C3), route MOSI to MISO internally through a Smart I/O block (see below). On a device without Smart I/O, connect the MOSI and MISO pins together externally.
- Choose a bit rate high enough that one byte transfers within the self-test guard time (`SPI_TXRX_DATA_TIME` in `SelfTest_SPI_SCB.h`); otherwise the test reports a timeout.

Do not run other SPI traffic on the same instance while the self-test executes: it clears the RX and TX FIFOs and temporarily masks the SPI interrupts for each byte.

### Smart I/O routing pattern

One possible configuration is:

- Select an SPI SCB whose MOSI and MISO signals can both connect to the same Smart I/O block.
- In the Device Configurator, route the SPI MOSI signal into one Smart I/O chip input and route the SPI MISO signal from one Smart I/O chip output.
- In the Smart I/O Configurator, use an asynchronous clock and keep the Data Unit disabled.
- Configure the MOSI-connected chip channel as `Input(Async)` and the MISO-connected chip channel as `Output`.
- Configure one loopback LUT that drives the MISO chip output in Combinational LUT mode (`COMB` / `CY_SMARTIO_LUTOPC_COMB`) with hex truth-table value `0x80`, and set its three trigger inputs (`TR0`, `TR1`, `TR2`) to the MOSI chip input. This creates the internal pass-through path from MOSI to MISO.
- Leave the remaining Smart I/O chip channels and LUTs in `Bypass` or disabled unless another board-specific routing requirement needs them.

For one PSOC™ Control C3 example configuration, the concrete values are:

- Smart I/O clock source: `Asynchronous`
- Smart I/O block alias: `CYBSP_SMARTIO_SPI_LOOPBACK`
- Chip 2: `Input(Async)` connected to `SPI.mosi`
- Chip 3: `Output` connected to `SPI.miso`
- I/O 2 and I/O 3: `None`
- LUT3 (MISO pass-through): `TR0 = TR1 = TR2 = Chip 2`, Combinational LUT mode (`COMB` / `CY_SMARTIO_LUTOPC_COMB`), value `0x80`

On other PSOC™ Control C3 boards, or when you choose a different SPI SCB or Smart I/O block, the chip and I/O indices can change. Reuse the same routing pattern and LUT values, but do not assume the same channel numbers.

### Application resources and interrupt handler

Declare this application-scope SPI context and interrupt handler before the application initialization or test function:

```c
/* SPI context and interrupt handler for the instance under test. */
static cy_stc_scb_spi_context_t spiContext;

static void SPI_Isr(void)
{
    Cy_SCB_SPI_Interrupt(CYBSP_DUT_SPI_HW, &spiContext);
}
```

### Application-side test sequence

The example initializes the SPI master, activates the Smart I/O loopback, and then calls `SelfTest_SPI_SCB()` repeatedly until it reports completion:

```c
uint8_t status = OK_STATUS;

const cy_stc_sysint_t spiIrqCfg =
{
    .intrSrc      = CYBSP_DUT_SPI_IRQ,
    .intrPriority = 0U
};

/* Initialize the SPI master instance and select slave line 0. */
if (CY_SCB_SPI_SUCCESS != Cy_SCB_SPI_Init(CYBSP_DUT_SPI_HW, &CYBSP_DUT_SPI_config,
                                          &spiContext))
{
    status = ERROR_STATUS;
}
Cy_SCB_SPI_SetActiveSlaveSelect(CYBSP_DUT_SPI_HW, CY_SCB_SPI_SLAVE_SELECT0);

/* Hook the SPI interrupt to the PDL handler and enable the block. */
(void)Cy_SysInt_Init(&spiIrqCfg, &SPI_Isr);
NVIC_EnableIRQ((IRQn_Type)spiIrqCfg.intrSrc);
Cy_SCB_SPI_Enable(CYBSP_DUT_SPI_HW);

/* Route MOSI back to MISO through Smart I/O to form the internal loopback.
 * Clearing the channel bypass activates the loopback path. */
if (CY_SMARTIO_SUCCESS != Cy_SmartIO_Init(CYBSP_SMARTIO_SPI_LOOPBACK_HW,
                                          &CYBSP_SMARTIO_SPI_LOOPBACK_config))
{
    status = ERROR_STATUS;
}
(void)Cy_SmartIO_SetChBypass(CYBSP_SMARTIO_SPI_LOOPBACK_HW, CY_SMARTIO_CHANNEL_NONE);
Cy_SmartIO_Enable(CYBSP_SMARTIO_SPI_LOOPBACK_HW);

__enable_irq();

/* Clear any residual data before the first self-test iteration. */
Cy_SCB_SPI_ClearRxFifo(CYBSP_DUT_SPI_HW);
Cy_SCB_SPI_ClearTxFifo(CYBSP_DUT_SPI_HW);

/* Run the loopback self-test until it completes. Each call sends one byte and
 * checks that the same byte is received back over the loopback. */
while (OK_STATUS == status)
{
    status = SelfTest_SPI_SCB(CYBSP_DUT_SPI_HW);
    if (PASS_COMPLETE_STATUS == status)
    {
        /* SPI loopback test passed */
        break;
    }
    else if (PASS_STILL_TESTING_STATUS == status)
    {
        /* Continue testing */
        status = OK_STATUS;
    }
    else
    {
        /* Handle SPI self-test failure: ERROR_STATUS, ERROR_TX_NOT_EMPTY,
         * or ERROR_RX_NOT_EMPTY */
    }
}
```

## UART SCB test

Devices that have Smart I/O can use it for the UART self-test by connecting TX and RX through the same Smart I/O block.

### Smart I/O routing pattern

One possible configuration is:

- Select a UART SCB whose TX and RX signals can both connect to the same Smart I/O block.
- In the Device Configurator, route the UART TX signal into one Smart I/O chip input and route the UART RX signal from one Smart I/O chip output.
- In the Smart I/O Configurator, use an asynchronous clock and keep the Data Unit disabled.
- Configure the UART TX-connected chip channel as `Input(Async)` and the UART RX-connected chip channel as `Output`.
- Enable only the two LUTs used by the loopback path. Set both LUT trigger inputs (`TR0`, `TR1`, `TR2`) to the UART TX chip input.
- Configure LUT2 in Combinational LUT mode (`COMB` / `CY_SMARTIO_LUTOPC_COMB`) with hex truth-table value `0x80`. This creates the TX-to-RX pass-through path.
- Configure LUT3 in Combinational LUT mode (`COMB` / `CY_SMARTIO_LUTOPC_COMB`) with hex truth-table value `0xFF`. This drives the additional Smart I/O output used by the verified routing.
- Leave the remaining Smart I/O chip channels and LUTs in `Bypass` or disabled unless your board routing needs an additional observation or output pin.

For one PSOC™ Control C3 example configuration, the concrete values are:

- Smart I/O clock source: `Asynchronous`
- Chip 3: `Input(Async)` connected to `UART.tx`
- Chip 2: `Output` connected to `UART.rx`
- I/O 3: `Output`
- I/O 2: `None`
- LUT2: `TR0 = TR1 = TR2 = Chip 3`, combinational LUT mode (`COMB` / `CY_SMARTIO_LUTOPC_COMB`), value `0x80`
- LUT3: `TR0 = TR1 = TR2 = Chip 3`, combinational LUT mode (`COMB` / `CY_SMARTIO_LUTOPC_COMB`), value `0xFF`

On other PSOC™ Control C3 boards, or when you choose a different UART SCB or Smart I/O block, the chip and I/O indices can change. Reuse the same routing pattern and LUT values, but do not assume the same channel numbers.

### Application-side enable sequence

Run this code to enable loopback before the UART self-test starts:

```c
uint8_t status = OK_STATUS;

/* Initialize SmartIO for UART loopback (TX routed back to RX) */
cy_en_smartio_status_t initResult =
    Cy_SmartIO_Init(CYBSP_SMARTIO_UART_LOOPBACK_HW, &CYBSP_SMARTIO_UART_LOOPBACK_config);
if (initResult != CY_SMARTIO_SUCCESS)
{
    /* Handle SmartIO init failure */
    status = ERROR_STATUS;
}
else
{
    Cy_SmartIO_Enable(CYBSP_SMARTIO_UART_LOOPBACK_HW);
    Cy_SysLib_DelayUs(1000u);
}

if (OK_STATUS == status)
{
    /* Initialize UART for loopback test */
    cy_en_scb_uart_status_t uartStatus =
        Cy_SCB_UART_Init(CYBSP_DUT_UART_HW, &CYBSP_DUT_UART_config, &CYBSP_DUT_UART_context);
    if (CY_SCB_UART_SUCCESS != uartStatus)
    {
        status = ERROR_STATUS;
    }
    else
    {
        Cy_SCB_UART_Enable(CYBSP_DUT_UART_HW);
        Cy_SCB_UART_ClearRxFifo(CYBSP_DUT_UART_HW);
        Cy_SCB_UART_ClearTxFifo(CYBSP_DUT_UART_HW);
    }
}

/* Run UART loopback test - call repeatedly until complete */
while (OK_STATUS == status)
{
    status = SelfTest_UART_SCB(CYBSP_DUT_UART_HW);
    if (PASS_COMPLETE_STATUS == status)
    {
        /* UART loopback test passed */
        break;
    }
    else if (PASS_STILL_TESTING_STATUS == status)
    {
        /* Continue testing */
        status = OK_STATUS;
    }
    else
    {
        /* Handle UART test failure */
    }
}
```

## UART data transfer tests

The UART transport helper APIs validate framed message exchange between a UART master and UART slave path.

### Configuration

In the Device Configurator, enable three peripherals:

- **Master UART**: one UART SCB instance in UART mode. Give it the alias `CYBSP_DUT_UART_MASTER`. Configure the baud rate, data bits, stop bits, and parity to match the slave instance.
- **Slave UART**: a second UART SCB instance in UART mode. Give it the alias `CYBSP_DUT_UART_SLAVE`. Its baud rate and frame format must match the master.
- **Guard timer**: one TCPWM counter instance. Give it the alias `CYBSP_TIMER_UART_MASTER`. The master protocol uses this timer to detect a slave that does not respond within the guard period.

Wire the two UART instances together externally: connect master TX to slave RX, and slave TX to master RX.

The slave address passed to `UartMesSlave_Init()` must match the address passed to `UartMesMaster_DataProc()`. The example uses address `2`.

Do not share either UART instance with other application traffic while the test runs: the protocol takes full ownership of the TX/RX FIFOs and interrupt masks for the duration of the exchange.

### Application-side test sequence

The example installs the three interrupt handlers, initialises the guard timer and both UART instances, and then polls master and slave states until the exchange completes:

```c
/* Register interrupt handlers required by the master/slave message protocol.
 * The master needs a guard timer ISR (UartMesMaster_Timeout_ISR) and a UART
 * ISR (UartMesMaster_Msg_ISR); the slave needs only a UART ISR
 * (UartMesSlave_Msg_ISR). */
const cy_stc_sysint_t timerIntrCfg =
{
    .intrSrc      = CYBSP_TIMER_UART_MASTER_IRQ,
    .intrPriority = 3UL,
};
const cy_stc_sysint_t masterIntrCfg =
{
    .intrSrc      = CYBSP_DUT_UART_MASTER_IRQ,
    .intrPriority = 3UL,
};
const cy_stc_sysint_t slaveIntrCfg =
{
    .intrSrc      = CYBSP_DUT_UART_SLAVE_IRQ,
    .intrPriority = 3UL,
};

Cy_SysInt_Init(&timerIntrCfg, UartMesMaster_Timeout_ISR);
Cy_SysInt_Init(&masterIntrCfg, UartMesMaster_Msg_ISR);
Cy_SysInt_Init(&slaveIntrCfg, UartMesSlave_Msg_ISR);
NVIC_EnableIRQ((IRQn_Type)CYBSP_TIMER_UART_MASTER_IRQ);
NVIC_EnableIRQ((IRQn_Type)CYBSP_DUT_UART_MASTER_IRQ);
NVIC_EnableIRQ((IRQn_Type)CYBSP_DUT_UART_SLAVE_IRQ);

Cy_TCPWM_Counter_Init(CYBSP_TIMER_UART_MASTER_HW, CYBSP_TIMER_UART_MASTER_NUM,
                      &CYBSP_TIMER_UART_MASTER_config);
Cy_TCPWM_Counter_Enable(CYBSP_TIMER_UART_MASTER_HW, CYBSP_TIMER_UART_MASTER_NUM);

Cy_SCB_UART_Init(CYBSP_DUT_UART_MASTER_HW, &CYBSP_DUT_UART_MASTER_config, NULL);
Cy_SCB_UART_Init(CYBSP_DUT_UART_SLAVE_HW, &CYBSP_DUT_UART_SLAVE_config, NULL);

UartMesMaster_Init(CYBSP_DUT_UART_MASTER_HW, CYBSP_TIMER_UART_MASTER_HW,
                   CYBSP_TIMER_UART_MASTER_NUM);
/* Slave address must match the address passed to UartMesMaster_DataProc() */
UartMesSlave_Init(CYBSP_DUT_UART_SLAVE_HW, 2u);

uint8_t txd[] = "1234567890ABCDEF";
uint8_t rxd[sizeof(txd)];

/* Start the master transaction; slave address 2 must match UartMesSlave_Init() */
(void)UartMesMaster_DataProc(2u, txd, (uint8_t)sizeof(txd), rxd, (uint8_t)sizeof(rxd));

uint8_t status = UM_ERROR;

/* Poll master and slave states in lockstep. The slave echoes received data
 * back as soon as UM_PACKREADY is set; the loop exits when the master
 * finishes (UM_COMPLETE) or times out (UM_ERROR). */
for (;;)
{
    uint8_t mstate = UartMesMaster_State();
    uint8_t sstate = UartMesSlave_State();

    if (UM_PACKREADY == sstate)
    {
        (void)UartMesSlave_Respond((uint8_t*)UartMesSlave_GetDataPtr(),
                                   UartMesSlave_GetDataSize());
    }

    if (UM_BUSY != mstate)
    {
        status = (UM_COMPLETE == mstate) ? OK_STATUS : ERROR_STATUS;
        break;
    }

    Cy_SysLib_Delay(1u);
}

if (OK_STATUS != status)
{
    /* Handle UART data transfer self-test failure */
}

NVIC_DisableIRQ((IRQn_Type)CYBSP_TIMER_UART_MASTER_IRQ);
NVIC_DisableIRQ((IRQn_Type)CYBSP_DUT_UART_MASTER_IRQ);
NVIC_DisableIRQ((IRQn_Type)CYBSP_DUT_UART_SLAVE_IRQ);
Cy_TCPWM_Counter_Disable(CYBSP_TIMER_UART_MASTER_HW, CYBSP_TIMER_UART_MASTER_NUM);
Cy_SCB_UART_Disable(CYBSP_DUT_UART_MASTER_HW, NULL);
Cy_SCB_UART_DeInit(CYBSP_DUT_UART_MASTER_HW);
Cy_SCB_UART_Disable(CYBSP_DUT_UART_SLAVE_HW, NULL);
Cy_SCB_UART_DeInit(CYBSP_DUT_UART_SLAVE_HW);
UartMesMaster_DeInit();
UartMesSlave_DeInit();
```

## CAN FD test

The CAN FD self-test runs the channel in internal loopback. It transmits one frame with a rejected message ID and one frame with an accepted message ID, then confirms that only the accepted frame reaches the receive FIFO. The call returns `OK_STATUS` (0) on success and `ERROR_STATUS` (1) on failure.

### Configuration

- Enable one CAN FD channel. Give it an alias of your choice; the application code must reference the same name. This guide uses `CANFD` as an example. The application passes the channel base, channel number, configuration structure, and a context structure to the self-test.
- Add a standard-ID acceptance filter that stores message IDs `0x50` to `0x55` into receive FIFO 0. The self-test transmits ID `0x52`, which must be accepted, and ID `0x60`, which must be rejected, so this filter is required. Configure non-matching standard and extended frames to be rejected.
- The self-test uses internal loopback test mode (`CY_CANFD_TEST_MODE_INTERNAL_LOOP_BACK`), so it needs no transceiver or external bus. External test mode also drives the external CAN FD pins.

Do not call the self-test while the channel is carrying application bus traffic: it temporarily changes the global filter and the receive callback for the duration of the test and then restores them. The call blocks for about two seconds while it waits for the loopback frames.

### Application resources and interrupt handler

Declare this application-scope CAN FD channel constant, context, and interrupt handler before the application initialization or test function:

```c
/* CAN FD channel index that matches the configured instance. */
#define STL_CANFD_CHANNEL   (0U)

/* CAN FD context and interrupt handler for the channel under test. */
static cy_stc_canfd_context_t canfdContext;

static void CANFD_Isr(void)
{
    Cy_CANFD_IrqHandler(CANFD_HW, STL_CANFD_CHANNEL, &canfdContext);
}
```

### Application-side test sequence

The example enables the channel Message RAM, initializes the channel, installs the interrupt handler, and then calls `SelfTest_CANFD()` once in internal-loopback mode:

```c
uint8_t status = OK_STATUS;

const cy_stc_sysint_t canfdIrqCfg =
{
    .intrSrc      = canfd_0_interrupts0_0_IRQn,
    .intrPriority = 2U
};

/* Enable the channel Message RAM and initialize the CAN FD channel. */
Cy_CANFD_EnableMRAM(CANFD_HW, (1UL << STL_CANFD_CHANNEL), 6U);

if (CY_CANFD_SUCCESS != Cy_CANFD_Init(CANFD_HW, STL_CANFD_CHANNEL,
                                      &CANFD_config, &canfdContext))
{
    status = ERROR_STATUS;
}

/* Hook the CAN FD interrupt to the PDL handler and enable it. */
(void)Cy_SysInt_Init(&canfdIrqCfg, &CANFD_Isr);
NVIC_EnableIRQ((IRQn_Type)canfdIrqCfg.intrSrc);

__enable_irq();

/* Run the internal-loopback self-test once. The call blocks for about two
 * seconds while it waits for the loopback frames. */
if (OK_STATUS == status)
{
    status = SelfTest_CANFD(CANFD_HW, STL_CANFD_CHANNEL, &CANFD_config,
                            &canfdContext,
                            CY_CANFD_TEST_MODE_INTERNAL_LOOP_BACK);
    if (OK_STATUS == status)
    {
        /* CAN FD loopback test passed */
    }
    else
    {
        /* Handle CAN FD self-test failure (ERROR_STATUS) */
    }
}
```
