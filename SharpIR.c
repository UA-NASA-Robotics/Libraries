#include <SharpIR.h>
//#include <DistanceGPY0A21YK_LUTs.h>

struct GPY0A21YK {
    //private:
    uint16_t _receiveReg;
    uint16_t _average;
    uint16_t _VCC;
    
}

void GP0A21YK_IOInit(uint16_t receiveReg)
{
    TRISA = 0xff;
    PORTA = receiveReg;
    ADCON1 = 0b11110000;
    return;
}

uint16_t captureAI(struct GPY0A21YK *sensor)
{
    uint8_t delayTime = 20; //20ms acquasition delay
    __delay_ms(delayTime);
    ADCON0 = 0x01; //Turn ADC on
    ADCON0 |= 1 << sensor->receiveReg; //set b[1] "go" bit,VAR |= 1 << 3 sets bit 3 fyi
    uint8_t doneBit;
    do
    {
        //wait for ADC to complete (go bit switches to 0 automatically when done)
        doneBit = (ADCON0 >> 1) & 1;
    } while(doneBit); //while go bit is on (AD conversion in progress)
    uint16_t result = (ADRESH << 8) | ADRESL; //combine two 8bit values into a 16bit val
    ADCON0 = 0x00; //Turn ADC off return;
    return result;
}


/*
 * Begin [Distance Pin Specified]
*/
void distanceBegin(struct GPY0A21YK *sensor, uint16_t receiveReg)//string port)
{
    setEnabled(&sensor, true);

    ANSELA = receiveReg;
    sensor->_receiveReg=receiveReg;
    GP0A21YK_IOInit(receiveReg);
    setAveraging(&sensor, 1);
}

/*
 * Begin [Pins specified]
*/
void distanceBegin(struct GPY0A21YK *sensor, uint16_t receiveReg, uint16_t VCC)
{
    sensor->_VCC=VCC;
    begin(&sensor, receiveReg);
    setEnabled(&sensor, true);
}

/*
 * getDistanceRaw
*/
uint16_t getDistanceRaw(struct GPY0A21YK *sensor)
{
    if(sensor->_enabled == 1)
    {
        return (captureAI(&sensor));
    }
    else
    {
        return (1023);
    }
}

/*
 * getDistanceVolt
*/
uint16_t getDistanceVolt(struct GPY0A21YK *sensor)
{
    return getDistanceRaw(&sensor);
}

/*
 * getDistanceCent
*/
uint16_t getDistanceCent(struct GPY0A21YK *sensor)
{
    uint16_t distance = (getDistanceRaw(&sensor)/23.345787526); 
    return pow(distance, (-1/0.9292192118));
}

/*
 * setEnabled
*/
void setEnabled(struct GPY0A21YK *sensor, bool status)
{
    sensor->_enabled = status;
    if (sensor->_enabled)
    {
        //**digitalWrite(_vccPin, HIGH);
    }
    else
    {
        //**digitalWrite(_vccPin, LOW);
    }
}