#include "lte.h"

uint8_t LTE_Rx_Buffer[LTE_RX_BUFFER_SIZE];
uint16_t LTE_Rx_Counter;
uint8_t dummy_lte;

uint8_t LTE_Status;
uint8_t LTE_ID[] = "SN_TEST_FM04_001";
uint16_t LTE_TEMP_I = 23;
uint16_t LTE_TEMP_D = 30;
uint16_t LTE_HUMI_I = 36;
uint32_t LTE_HUMI_D = 42;
uint16_t LTE_DIST_I = 158;
uint32_t LTE_DIST_D = 00;
uint16_t LTE_CHAR_I = 3;
uint32_t LTE_CHAR_D = 87;
uint16_t LTE_GPS1_I = 36;
uint32_t LTE_GPS1_D = 80914;
uint16_t LTE_GPS2_I = 127;
uint32_t LTE_GPS2_D = 14504;
uint8_t LTE_DOOR = 1;

uint8_t LTE_Test_Buffer[LTE_RX_BUFFER_SIZE];

void LTE_Init(void)
{
    if ( HAL_UART_Receive_IT(&huart1, &dummy_lte, 1) != HAL_OK)
    {
        Error_Handler();
    }
    
	//SARA Power On
	HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_RESET);
    HAL_Delay(1000);
	//SARA Reset Off
	HAL_GPIO_WritePin(RESET_ONOFF_GPIO_Port, RESET_ONOFF_Pin, GPIO_PIN_RESET);
    LTE_MQTT_Convert_Buffer(LTE_Test_Buffer);
}

void LTE_Transmit(uint8_t * buf)
{
    HAL_UART_Transmit(&huart1,buf,strlen(buf),100);                    
    HAL_UART_Transmit(&huart3,buf,strlen(buf),100);
}

uint8_t LTE_OK_Check(void)
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
        }
    }

    return retval;
}

uint8_t LTE_Delay(uint16_t time)
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

uint8_t LTE_Receive_Check(uint8_t status)
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

void LTE_MQTT_Convert_Buffer(uint8_t *buf)
{
    uint8_t ascii_buf[1024] = {0,};    
    uint8_t convert_buf[1024] = {0,};
    uint8_t header[] = "AT+UMQTTSNC=4,1,0,1,0,\"1\",\"";
    uint8_t end[] = "\"\r\n";
    sprintf(ascii_buf,"{\"_ID\":\"%s\",\"_TEMP\":\"%d.%d\",\"_HUMI\":\"%d.%d\",\"_DIST\":\"%d.%d\",\"_CHAR\":\"%d.%d\",\"_GPS\":\"%d.%d,%d.%d\",\"_DOOR\":\"%d\"}",
            LTE_ID,LTE_TEMP_I,LTE_TEMP_D,LTE_HUMI_I,LTE_HUMI_D,LTE_DIST_I,LTE_DIST_D,LTE_CHAR_I,LTE_CHAR_D,LTE_GPS1_I,LTE_GPS1_D,LTE_GPS2_I,LTE_GPS2_D,LTE_DOOR);
    for(uint16_t i = 0; i < strlen(ascii_buf); i++)
    {
        sprintf(&convert_buf[2*i],"%x",ascii_buf[i]);
    }

    memcpy(buf,header,strlen(header));
    memcpy(&buf[strlen(header)],convert_buf,strlen(convert_buf));
    memcpy(&buf[strlen(buf)],end,strlen(end));
}

void LTE_Connect(void)
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
                sprintf(Tx_Buf,"AT+CGDCONT=1\r\n");
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
                    //LTE_Status = LTE_Receive_Check(LTE_STEP_MQTT_PUBLISH);
                    stDelay = FAIL;
                    uart_status = LTE_UART_STATUS_TRANSMIT;
                }
                else
                {
                    stDelay = LTE_Delay(10000);
                }
            }
        break;
    }
}
