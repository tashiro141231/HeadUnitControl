#include <Arduino.h>

#define MAX_BUFFER 64

typedef uint8_t (*CallBackFunc)(uint8_t, uint16_t, uint16_t);

class HUCtrl{
  public:
    HUCtrl(uint8_t unitID, int ctrlPin);
    HUCtrl(Stream &serial, uint8_t unitID, int ctrlPin);
    void begin(unsigned long baud);
    int poll();
    void data_visualizer();

  private:
    uint16_t calcCRC(uint8_t *buf, int length);

    Stream &serial;
    uint32_t timeout;
    uint32_t last_receive_time;
    uint16_t last_receive_len;
    uint8_t unitID;
    int ctrlPin;
    uint8_t bufIn[MAX_BUFFER];
    uint8_t bufOut[MAX_BUFFER];
};

