#ifndef drv_i2c_h
#define drv_i2c_h



uint8_t i2c0_write_n_byte(uint8_t DevAddr, uint8_t RegAddr, uint8_t *buf, uint8_t nBytes);

#endif /* I2C_H_ */