#include "ti_msp_dl_config.h"
#include "drv_i2c.h"

#define DELAY (32000)
void _delay_ms(uint16_t ms)
{
    while(ms --)
        delay_cycles(DELAY);
}

void nop() {
  __builtin_arm_nop();
}



uint8_t i2c0_write_n_byte(uint8_t DevAddr, uint8_t RegAddr, uint8_t *buf, uint8_t nBytes)
{
		static uint8_t temp_reg_dddr=0;
    uint8_t n;
    uint32_t Byte4Fill;
    temp_reg_dddr = RegAddr;
    DL_I2C_fillControllerTXFIFO(I2C_1_INST, &buf[0], nBytes);

    /* Wait for I2C to be Idle */
    while (!(DL_I2C_getControllerStatus(I2C_1_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

    DL_I2C_flushControllerTXFIFO(I2C_1_INST); 
    DL_I2C_fillControllerTXFIFO(I2C_1_INST, &temp_reg_dddr, 1);

    /* Send the packet to the controller.
     * This function will send Start + Stop automatically.
     */
    DL_I2C_startControllerTransfer(I2C_1_INST, DevAddr,DL_I2C_CONTROLLER_DIRECTION_TX, (nBytes+1));
    n = 0;
    do {
        Byte4Fill = DL_I2C_getControllerTXFIFOCounter(I2C_1_INST);
        if(Byte4Fill > 1)
        {
            DL_I2C_fillControllerTXFIFO(I2C_1_INST, &buf[n], 1);
            n++;
        }
    }while(n<nBytes);

    /* Poll until the Controller writes all bytes */
    while (DL_I2C_getControllerStatus(I2C_1_INST) &DL_I2C_CONTROLLER_STATUS_BUSY_BUS);

    /* Trap if there was an error */
    if (DL_I2C_getControllerStatus(I2C_1_INST) &DL_I2C_CONTROLLER_STATUS_ERROR) 
		{
        /* LED will remain high if there is an error */
        __NOP();//__BKPT(0);
    }

    /* Add delay between transfers */
    delay_cycles(1000);

    return nBytes;
}


void  i2c0_read_n_byte(uint8_t DevAddr, uint8_t RegAddr, uint8_t *buf, uint8_t nBytes)
{
		static uint8_t temp_reg_dddr=0;
    uint8_t n;
    uint32_t Byte4Fill;
    temp_reg_dddr = RegAddr;
    DL_I2C_fillControllerTXFIFO(I2C_1_INST, &buf[0], nBytes);

    /* Wait for I2C to be Idle */
    while (!(DL_I2C_getControllerStatus(I2C_1_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

    DL_I2C_flushControllerRXFIFO(I2C_1_INST); 
    DL_I2C_fillControllerTXFIFO(I2C_1_INST, &temp_reg_dddr, 1);

    /* Send the packet to the controller.
     * This function will send Start + Stop automatically.
     */
    DL_I2C_startControllerTransfer(I2C_1_INST, DevAddr,DL_I2C_CONTROLLER_DIRECTION_TX, (nBytes+1));
    n = 0;
    do {
        Byte4Fill = DL_I2C_getControllerTXFIFOCounter(I2C_1_INST);
        if(Byte4Fill > 1)
        {
            DL_I2C_fillControllerTXFIFO(I2C_1_INST, &buf[n], 1);
            n++;
        }
    }while(n<nBytes);

    /* Poll until the Controller writes all bytes */
    while (DL_I2C_getControllerStatus(I2C_1_INST) &DL_I2C_CONTROLLER_STATUS_BUSY_BUS);

    /* Trap if there was an error */
    if (DL_I2C_getControllerStatus(I2C_1_INST) &DL_I2C_CONTROLLER_STATUS_ERROR) 
		{
        /* LED will remain high if there is an error */
        __NOP();//__BKPT(0);
    }
}





//************I2C write register **********************
void I2C_WriteReg(uint8_t DevAddr,uint8_t reg_addr, uint8_t *reg_data, uint8_t count)
{
    unsigned char I2Ctxbuff[8] = {0x00};

    I2Ctxbuff[0] = reg_addr;
    unsigned char i, j = 1;

    for (i = 0; i < count; i++) {
        I2Ctxbuff[j] = reg_data[i];
        j++;
    }

    //    DL_I2C_flushControllerTXFIFO(I2C_1_INST);
    DL_I2C_fillControllerTXFIFO(I2C_1_INST, &I2Ctxbuff[0], count + 1);

    /* Wait for I2C to be Idle */
    while (!(DL_I2C_getControllerStatus(I2C_1_INST) &
             DL_I2C_CONTROLLER_STATUS_IDLE))
        ;

    DL_I2C_startControllerTransfer(I2C_1_INST, DevAddr,
        DL_I2C_CONTROLLER_DIRECTION_TX, count + 1);

    while (DL_I2C_getControllerStatus(I2C_1_INST) &
           DL_I2C_CONTROLLER_STATUS_BUSY_BUS)
        ;
    /* Wait for I2C to be Idle */
    while (!(DL_I2C_getControllerStatus(I2C_1_INST) &
             DL_I2C_CONTROLLER_STATUS_IDLE))
        ;

    //Avoid BQ769x2 to stretch the SCLK too long and generate a timeout interrupt at 400kHz because of low power mode
    // if(DL_I2C_getRawInterruptStatus(I2C_1_INST,DL_I2C_INTERRUPT_CONTROLLER_CLOCK_TIMEOUT))
    // {
    //     DL_I2C_flushControllerTXFIFO(I2C_1_INST);
    //     DL_I2C_clearInterruptStatus(I2C_1_INST,DL_I2C_INTERRUPT_CONTROLLER_CLOCK_TIMEOUT);
    //     I2C_WriteReg(reg_addr, reg_data, count);
    // }
    DL_I2C_flushControllerTXFIFO(I2C_1_INST);
}

//************I2C read register **********************
void I2C_ReadReg(uint8_t DevAddr,uint8_t reg_addr, uint8_t *reg_data, uint8_t count)
{
    DL_I2C_fillControllerTXFIFO(I2C_1_INST, &reg_addr, count);

    /* Wait for I2C to be Idle */
    while (!(DL_I2C_getControllerStatus(I2C_1_INST) &
             DL_I2C_CONTROLLER_STATUS_IDLE))
        ;

    DL_I2C_startControllerTransfer(
        I2C_1_INST, DevAddr, DL_I2C_CONTROLLER_DIRECTION_TX, 1);

    while (DL_I2C_getControllerStatus(I2C_1_INST) &
           DL_I2C_CONTROLLER_STATUS_BUSY_BUS)
        ;
    /* Wait for I2C to be Idle */
    while (!(DL_I2C_getControllerStatus(I2C_1_INST) &
             DL_I2C_CONTROLLER_STATUS_IDLE))
        ;

    DL_I2C_flushControllerTXFIFO(I2C_1_INST);

    /* Send a read request to Target */
    DL_I2C_startControllerTransfer(
        I2C_1_INST, DevAddr, DL_I2C_CONTROLLER_DIRECTION_RX, count);

    for (uint8_t i = 0; i < count; i++) {
        while (DL_I2C_isControllerRXFIFOEmpty(I2C_1_INST))
            ;
        reg_data[i] = DL_I2C_receiveControllerData(I2C_1_INST);
    }
}








