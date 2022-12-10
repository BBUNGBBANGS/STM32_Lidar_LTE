#include "lte.h"
#include "dht11.h"
#include "lidar.h"
#include "device.h"

uint8_t LTE_Rx_Buffer[LTE_RX_BUFFER_SIZE];
uint8_t LTE_ASCII_Buffer[LTE_RX_BUFFER_SIZE/2];
uint8_t LTE_GPS_Buffer[100];
uint16_t LTE_Rx_Counter;
uint8_t dummy_lte;

uint8_t LTE_Status;
uint8_t LTE_GPS_Status;// = LTE_GPS_STEP_FINISH;
uint8_t LTE_ID[] = "SN_FM4_TEST_002";
uint16_t LTE_TEMP_I,LTE_TEMP_D,LTE_HUMI_I,LTE_HUMI_D,LTE_DIST_I,LTE_DIST_D;
uint16_t LTE_CHAR_I,LTE_CHAR_D;
uint16_t LTE_GPS_LAT_I,LTE_GPS_LONG_I;
uint32_t LTE_GPS_LAT_D,LTE_GPS_LONG_D;
uint8_t LTE_DOOR;


static void LTE_Transmit(uint8_t * buf);
static uint8_t LTE_OK_Check(void);
static uint8_t LTE_Delay(uint16_t time);
static uint8_t LTE_Receive_Check(uint8_t status);
static void LTE_MQTT_Convert_Buffer(uint8_t *buf);
static uint8_t LTE_GPS_Receive_Data(uint8_t status);
static void LTE_Update_Data(void);
static void LTE_Connect(void);
static void LTE_GPS_Connect(void);
static uint8_t LTE_GPS_Update_Data(uint8_t status);

void LTE_Init(void)
{
    if ( HAL_UART_Receive_IT(&huart1, &dummy_lte, 1) != HAL_OK)
    {
        Error_Handler();
    }
    
	//SARA Power On
	HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_RESET);
    HAL_Delay(1500);
	//SARA Reset Off
	HAL_GPIO_WritePin(RESET_ONOFF_GPIO_Port, RESET_ONOFF_Pin, GPIO_PIN_RESET);
}

void LTE_Control(void)
{
    LTE_Update_Data();
    if(LTE_GPS_STEP_FINISH == LTE_GPS_Status)
    {
        //LTE_Connect();
    }
    else
    {
        LTE_GPS_Connect();
    }
}

static void LTE_Update_Data(void)
{
    DHT_Data_t data;
    data = DHT_getData(DHT22);
    if(data.Temperature<0)
    {
        LTE_TEMP_I = (uint16_t)(data.Temperature * -1);
        LTE_TEMP_D = ((uint16_t)((data.Temperature * -1) * 100))%100;        
    }
    else
    {
        LTE_TEMP_I = (uint16_t)data.Temperature;
        LTE_TEMP_D = ((uint16_t)(data.Temperature * 100))%100;
    }

    LTE_HUMI_I = (uint16_t)data.Humidity;
    LTE_HUMI_D = ((uint16_t)(data.Humidity * 100))%100;
    LTE_DIST_I = Lidar_Distance;
    LTE_CHAR_I = (Device_Batt_Voltage)/1000;
    LTE_CHAR_D = (Device_Batt_Voltage/10)%100;
    LTE_DOOR = Device_Sensor_Signal;
    
    if(data.Temperature<0)
    {
        sprintf(LTE_ASCII_Buffer,"{\"_ID\":\"%s\",\"_TEMP\":\"-%d.%02d\",\"_HUMI\":\"%d.%02d\",\"_DIST\":\"%d.%02d\",\"_CHAR\":\"%d.%02d\",\"_GPS\":\"%d.%05d,%d.%05d\",\"_DOOR\":\"%d\"}",
            LTE_ID,LTE_TEMP_I,LTE_TEMP_D,LTE_HUMI_I,LTE_HUMI_D,LTE_DIST_I,LTE_DIST_D,LTE_CHAR_I,LTE_CHAR_D,LTE_GPS_LAT_I,LTE_GPS_LAT_D,LTE_GPS_LONG_I,LTE_GPS_LONG_D,LTE_DOOR);
    }
    else
    {
        sprintf(LTE_ASCII_Buffer,"{\"_ID\":\"%s\",\"_TEMP\":\"%d.%02d\",\"_HUMI\":\"%d.%02d\",\"_DIST\":\"%d.%02d\",\"_CHAR\":\"%d.%02d\",\"_GPS\":\"%d.%05d,%d.%05d\",\"_DOOR\":\"%d\"}",
            LTE_ID,LTE_TEMP_I,LTE_TEMP_D,LTE_HUMI_I,LTE_HUMI_D,LTE_DIST_I,LTE_DIST_D,LTE_CHAR_I,LTE_CHAR_D,LTE_GPS_LAT_I,LTE_GPS_LAT_D,LTE_GPS_LONG_I,LTE_GPS_LONG_D,LTE_DOOR);
    }
    
}

static void LTE_Connect(void)
{
    char Tx_Buf[1024] = {0,};
    static LTE_Status_t stDelay;
    static uint8_t uart_status;

    switch(LTE_Status)
    {
        case LTE_STEP_ATE0 :
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"ATE0\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_ATE0);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(100);
                }
            }
        break;
        case LTE_STEP_CMEE : 
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+CMEE=2\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_CMEE);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(100);
                }
            }
        break;
        case LTE_STEP_CFUN_DISABLE :
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+CFUN=0\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_CFUN_DISABLE);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(3000);
                }
            }        
        break;
        case LTE_STEP_APN :
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+CGDCONT=1,\"IP\",\"tsudp\"\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_APN);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(500);
                }
            }
        break;   
        case LTE_STEP_CFUN_ENABLE :
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+CFUN=1\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {   
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_CFUN_ENABLE);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(3000);
                }
            }
        break;
        case LTE_STEP_CESQ :
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+CESQ\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {                
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_CESQ);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(100);
                }
            }
        break;
        case LTE_STEP_CEREG :
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+CEREG=2\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_CEREG);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(100);
                }
            }
        break;
        case LTE_STEP_UPSD_MAP :
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+UPSD=0,100,1\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {                
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_UPSD_MAP);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(1000);
                }
            }
        break;
        case LTE_STEP_UPSD_IPV4 :
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+UPSD=0,0,0\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {                
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_UPSD_IPV4);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(1000);
                }
            }
        break;
        case LTE_STEP_UPSDA :           
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+UPSDA=0,3\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {                
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_UPSDA);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(8000);
                }
            }
        break;
        case LTE_STEP_UPSND :           
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+UPSND=0,0\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {                
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_UPSND);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(1000);
                }
            }
        break;
        case LTE_STEP_MQTT_IP_SET :           
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+UMQTTSN=2,\"10.7.0.55\",2442\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {                
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_MQTT_IP_SET);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(50);
                }
            }
        break;
        case LTE_STEP_MQTT_DURATION :           
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+UMQTTSN=8,600\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {                
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_MQTT_DURATION);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(50);
                }
            }
        break;
        case LTE_STEP_MQTT_SNNV :           
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+UMQTTSNNV=2\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {                
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_MQTT_SNNV);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(2000);
                }
            }
        break;
        case LTE_STEP_MQTT_CONNECT :           
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+UMQTTSNC=1\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {               
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_MQTT_CONNECT);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(2000);
                }
            }
        break;
        case LTE_STEP_MQTT_REGISTER :           
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+UMQTTSNC=2,\"mqtt_test\"\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {                
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_MQTT_REGISTER);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(2000);
                }
            }
        break;
        case LTE_STEP_MQTT_SUBSCRIBE :           
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+UMQTTSNC=5,1,0,\"mqtt_test\"\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {                
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_MQTT_SUBSCRIBE);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(2000);
                }
            }
        break;
        case LTE_STEP_MQTT_PUBLISH :           
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                LTE_MQTT_Convert_Buffer(Tx_Buf);
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_MQTT_PUBLISH);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(10000);
                }
            }
        break;
        case LTE_STEP_MQTT_DISCONNECT :
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+UMQTTSNC=0\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {
                if(OK == stDelay)
                {
                    LTE_Status = LTE_Receive_Check(LTE_STEP_MQTT_DISCONNECT);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(1000);
                }
            }
        break;
        case LTE_STEP_MQTT_FINISH :
        break;
    }
}

static void LTE_GPS_Connect(void)
{
    char Tx_Buf[100] = {0,};
    static LTE_Status_t stDelay;
    static uint8_t uart_status;

    switch(LTE_GPS_Status)
    {
        case LTE_GPS_STEP_READY : 
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {
                if(OK == stDelay)
                {
                    LTE_GPS_Status = LTE_Receive_Check(LTE_GPS_STEP_READY);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(1000);
                }
            }        
        break;
        case LTE_GPS_STEP_ULOCCELL :
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+ULOCCELL=0\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {
                if(OK == stDelay)
                {
                    LTE_GPS_Status = LTE_Receive_Check(LTE_GPS_STEP_ULOCCELL);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(1000);
                }
            }
        break;        
        case LTE_GPS_STEP_ULOC :
            if(LTE_UART_STATUS_TRANSMIT == uart_status)
            {
                sprintf(Tx_Buf,"AT+ULOC=2,3,1,100,5000\r\n");
                LTE_Transmit(Tx_Buf);
                uart_status = LTE_UART_STATUS_RECEIVE;
            }
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {
                if(OK == stDelay)
                {
                    LTE_GPS_Status = LTE_GPS_Receive_Data(LTE_GPS_STEP_ULOC);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(1000);
                }
            }
        break;        
        case LTE_GPS_STEP_RECEIVEDATA :
            uart_status = LTE_UART_STATUS_RECEIVE;
            if(LTE_UART_STATUS_RECEIVE == uart_status)
            {
                if(OK == stDelay)
                {
                    LTE_GPS_Status = LTE_GPS_Receive_Data(LTE_GPS_STEP_RECEIVEDATA);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(5000);
                }
            }
        break;    
        case LTE_GPS_STEP_UPDATEDATA :
            LTE_GPS_Status = LTE_GPS_Update_Data(LTE_GPS_STEP_UPDATEDATA);
        break;
        case LTE_GPS_STEP_FINISH :
        break;
    }
}

static void LTE_Transmit(uint8_t * buf)
{
    HAL_UART_Transmit(&huart1,buf,strlen(buf),100);                    
    HAL_UART_Transmit(&huart3,buf,strlen(buf),100);
}

static uint8_t LTE_OK_Check(void)
{
    uint8_t retval = FAIL;
    if(LTE_Rx_Counter>3)
    {
        for(uint16_t i = 0; i < LTE_RX_BUFFER_SIZE; i++)
        {
            if((LTE_Rx_Buffer[i] == 'O') && (LTE_Rx_Buffer[i+1] == 'K') && (LTE_Rx_Buffer[i+2] == '\r') && (LTE_Rx_Buffer[i+3] == '\n'))
            {
                HAL_UART_Transmit(&huart3,&LTE_Rx_Buffer[i],strlen(&LTE_Rx_Buffer[i]),100);
                memset(LTE_Rx_Buffer,0x00,LTE_Rx_Counter);
                LTE_Rx_Counter = 0;
                retval = OK;
                return retval;
            }
            
            if(LTE_Rx_Buffer[i] == 0x00)
            {
                memset(LTE_Rx_Buffer,0x00,LTE_Rx_Counter);
                LTE_Rx_Counter = 0;
                break;
            }
        }
    }

    return retval;
}

static uint8_t LTE_Delay(uint16_t time)
{
    static uint16_t timer;
    LTE_Status_t retval = FAIL;
    time = time / 100; // [ms] to 100[ms]
    timer++;
    if(timer>time)
    {
        timer = 0;
        retval = OK;
    }

    return retval;
}

static uint8_t LTE_Receive_Check(uint8_t status)
{
    LTE_Status_t stReceive = FAIL;
    static uint8_t fail_counter;
    uint8_t retval = status;

    stReceive = LTE_OK_Check();

    if(OK == stReceive)    
    {
        retval++;
    }
    else
    {
        /* OK 실패 시, 현재 status 머무름 */
        fail_counter++;
        if(fail_counter > 10)
        {
            HAL_NVIC_SystemReset();
        }
    }

    return retval;
}

static void LTE_MQTT_Convert_Buffer(uint8_t *buf)
{
    uint8_t convert_buf[1024] = {0,};
    uint8_t header[] = "AT+UMQTTSNC=4,1,0,1,0,\"1\",\"";
    uint8_t end[] = "\"\r\n";
    
    for(uint16_t i = 0; i < strlen(LTE_ASCII_Buffer); i++)
    {
        sprintf(&convert_buf[2*i],"%x",LTE_ASCII_Buffer[i]);
    }

    memcpy(buf,header,strlen(header));
    memcpy(&buf[strlen(header)],convert_buf,strlen(convert_buf));
    memcpy(&buf[strlen(buf)],end,strlen(end));
}

static uint8_t LTE_GPS_Receive_Data(uint8_t status)
{
    uint8_t retval = status;

    if(LTE_GPS_STEP_ULOC == status)
    {
        if(LTE_Rx_Counter>0)
        {
            for(uint16_t i = 0; i < LTE_RX_BUFFER_SIZE; i++)
            {
                if((LTE_Rx_Buffer[i] == 'O') && (LTE_Rx_Buffer[i+1] == 'K') && (LTE_Rx_Buffer[i+2] == '\r') && (LTE_Rx_Buffer[i+3] == '\n'))
                {
                    HAL_UART_Transmit(&huart3,&LTE_Rx_Buffer[i],4,100);
                    retval++;
                    break;
                }
            }    
        }
    }
    else
    {
        if(LTE_Rx_Counter>0)
        {
            for(uint16_t i = 0; i < LTE_RX_BUFFER_SIZE; i++)
            {
                if((LTE_Rx_Buffer[i] == '+') && (LTE_Rx_Buffer[i+1] == 'U') && (LTE_Rx_Buffer[i+2] == 'U') && (LTE_Rx_Buffer[i+3] == 'L') && (LTE_Rx_Buffer[i+4] == 'O') && (LTE_Rx_Buffer[i+5] == 'C') && (LTE_Rx_Buffer[i+6] == ':'))
                {
                    HAL_UART_Transmit(&huart3,&LTE_Rx_Buffer[i],strlen(&LTE_Rx_Buffer[i]),100);
                    memcpy(LTE_GPS_Buffer,&LTE_Rx_Buffer[i+8],(LTE_Rx_Counter - i));
                    memset(LTE_Rx_Buffer,0x00,LTE_Rx_Counter);
                    LTE_Rx_Counter = 0;
                    if((LTE_GPS_Buffer[6] == '2') && (LTE_GPS_Buffer[7] == '0') && (LTE_GPS_Buffer[8] == '1') && (LTE_GPS_Buffer[9] == '5'))
                    {
                        retval = 0;
                    }
                    else
                    {
                        retval++;
                    }
                    break;
                }
            }    
        }
    }

    return retval;
}

static uint8_t LTE_GPS_Update_Data(uint8_t status)
{
    uint8_t retval = status;
    uint8_t lat_length=0;
    uint8_t long_length=0;
    RTC_TimeTypeDef Time = {0};
    RTC_DateTypeDef Date = {0};

    Date.Date = ((LTE_GPS_Buffer[0] - 0x30) * 10 + (LTE_GPS_Buffer[1] - 0x30));
    Date.Month = ((LTE_GPS_Buffer[3] - 0x30) * 10 + (LTE_GPS_Buffer[4] - 0x30));
    Date.Year = ((LTE_GPS_Buffer[8] - 0x30) * 10 + (LTE_GPS_Buffer[9] - 0x30));
    Time.Hours = ((LTE_GPS_Buffer[11] - 0x30) * 10 + (LTE_GPS_Buffer[12] - 0x30));
    Time.Minutes = ((LTE_GPS_Buffer[14] - 0x30) * 10 + (LTE_GPS_Buffer[15] - 0x30));
    Time.Seconds = ((LTE_GPS_Buffer[17] - 0x30) * 10 + (LTE_GPS_Buffer[18] - 0x30));
    HAL_RTC_SetTime(&hrtc, &Time, FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &Date, FORMAT_BIN);

    for(uint8_t i = 0; i < 3; i++)
    {
        if(LTE_GPS_Buffer[i+24] != '.')
        {
            lat_length++;
        }
        else
        {
            break;
        }
    }

    if(1 == lat_length)
    {
        LTE_GPS_LAT_I = (LTE_GPS_Buffer[24] - 0x30);
    }
    else if(2 == lat_length)
    {
        LTE_GPS_LAT_I = (LTE_GPS_Buffer[24] - 0x30) * 10 + (LTE_GPS_Buffer[25] - 0x30);  
    }    
    else if(3 == lat_length)
    {
        LTE_GPS_LAT_I = (LTE_GPS_Buffer[24] - 0x30) * 100 + (LTE_GPS_Buffer[25] - 0x30) * 10 + (LTE_GPS_Buffer[26] - 0x30);      
    }
    else
    {
        LTE_GPS_LAT_I = 0;
    }
    LTE_GPS_LAT_D = (LTE_GPS_Buffer[25+lat_length] - 0x30) * 1000000 + (LTE_GPS_Buffer[26+lat_length] - 0x30) * 100000 + (LTE_GPS_Buffer[27+lat_length] - 0x30) * 10000 + 
                    (LTE_GPS_Buffer[28+lat_length] - 0x30) * 1000+ (LTE_GPS_Buffer[29+lat_length] - 0x30) * 100 + (LTE_GPS_Buffer[30+lat_length] - 0x30) * 10 + (LTE_GPS_Buffer[31+lat_length] - 0x30);

    for(uint8_t i = 0; i < 3; i++)
    {
        if(LTE_GPS_Buffer[i+33+lat_length] != '.')
        {
            long_length++;
        }
        else
        {
            break;
        }
    }

    if(1 == long_length)
    {
        LTE_GPS_LONG_I = (LTE_GPS_Buffer[33+lat_length] - 0x30);
    }
    else if(2 == long_length)
    {
        LTE_GPS_LONG_I = (LTE_GPS_Buffer[33+lat_length] - 0x30) * 10 + (LTE_GPS_Buffer[34+lat_length] - 0x30);  
    }    
    else if(3 == long_length)
    {
        LTE_GPS_LONG_I = (LTE_GPS_Buffer[33+lat_length] - 0x30) * 100 + (LTE_GPS_Buffer[34+lat_length] - 0x30) * 10 + (LTE_GPS_Buffer[35+lat_length] - 0x30);      
    }
    else
    {
        LTE_GPS_LONG_I = 0;
    }

    LTE_GPS_LONG_D = (LTE_GPS_Buffer[34+lat_length+long_length] - 0x30) * 1000000 + (LTE_GPS_Buffer[35+lat_length+long_length] - 0x30) * 100000 + (LTE_GPS_Buffer[36+lat_length+long_length] - 0x30) * 10000 +
                     (LTE_GPS_Buffer[37+lat_length+long_length] - 0x30) * 1000 + (LTE_GPS_Buffer[38+lat_length+long_length] - 0x30) * 100 + (LTE_GPS_Buffer[39+lat_length+long_length] - 0x30) * 10 + (LTE_GPS_Buffer[40+lat_length+long_length] - 0x30);
    retval++;
    
    return retval;
}