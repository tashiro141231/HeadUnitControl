
#include <assert.h>
#include <string.h>
#include "head_unit_control.h"

HUCtrl::HUCtrl(uint8_t _unitID, int _ctrlPin)
:HUCtrl(Serial, _unitID, ctrlPin)
{}

HUCtrl::HUCtrl(Stream &_serial, uint8_t _unitID, int _ctrlPin)
:serial(_serial)
{
    // set modbus slave unit id
    unitID = _unitID;

    // set control pin for 485 write.
    ctrlPin = _ctrlPin;
}

/**
 * Begin serial port and set timeout.
 *
 * @param boud the serial port boud rate.
 */
void HUCtrl::begin(unsigned long boud) {
    // set control pin
    if (ctrlPin) {
        pinMode(ctrlPin, OUTPUT);
    }

    // set the timeout for 3.5 chars.
    serial.setTimeout(0);

    // set the T35 interframe timeout
    if (boud > 19200) {
        timeout = 1750;
    }
    else {
        timeout = 35000000 / boud; // 1T * 3.5 = T3.5
    }

    // init last received values
    last_receive_len = 0;
    last_receive_time = 0;
}

/**
 * Calculate buffer CRC16
 *
 * @param buf the data buffer.
 * @param length the length of the buffer without CRC.
 *
 * @return the calculated CRC16.
 */
uint16_t HUCtrl::calcCRC(uint8_t *buf, int length) {
    int i, j;
    uint16_t crc = 0xFFFF;
    uint16_t tmp;

    // calculate crc16
    for (i = 0; i < length; i++) {
        crc = crc ^ buf[i];

        for (j = 0; j < 8; j++) {
            tmp = crc & 0x0001;
            crc = crc >> 1;
            if (tmp) {
              crc = crc ^ 0xA001;
            }
        }
    }

    return crc;
}

int HUCtrl::poll(){
    int lengthIn;
    int lengthOut;
    uint16_t crc;
    uint16_t address;
    uint16_t length;
    uint16_t available_len;
    uint8_t fc;
    uint8_t cb_status;
    // uint8_t error = STATUS_OK;

    /**
     * Read one data frame:
     */
    // check if we have data in buffer.
    available_len = serial.available();
    if (available_len != 0) {
        // if we have new data, update last received time and length.
        if (available_len != last_receive_len) {
            last_receive_len = available_len;
            last_receive_time = micros();
            // Serial.println("11111");

            return 0;
        }

        // if no new data, wait for T35 microseconds.
        if (micros() < (last_receive_time + timeout)) {
            // Serial.println("22222");
            return 0;
        }

        // we waited for the inter-frame timeout, read the frame.
        lengthIn = serial.readBytes(bufIn, MAX_BUFFER);
        last_receive_len = 0;
        last_receive_time = 0;
    } else {
        // Serial.println("333333");
        return 0;
    }

    if(lengthIn < 4){
      // Serial.println("Data is not completed");
      return 0;
    }
    
    int num = bufIn[2];
    data_visualizer();
    serial.flush();
    return num;
}

void HUCtrl::data_visualizer() {
  Serial.print(bufIn[0]);
  Serial.print(",");
  Serial.print(bufIn[1]);
  Serial.print(",");
  Serial.print(bufIn[2]);
  Serial.print(",");
  Serial.print(bufIn[3]);
  Serial.print(",");
  Serial.print(bufIn[4]);
  Serial.print(",");
  Serial.println(bufIn[5]);
}
