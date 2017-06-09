/*
  MQTT.h - Library for GSM MQTT Client.
  Created by Nithin K. Kurian, Dhanish Vijayan, Elementz Engineers Guild Pvt. Ltd, July 2, 2016.
  Released into the public domain.
  Modified to STM32 by Attila. E. Franko
*/

#include "edge_comm/GSM_MQTT.h"

extern serialBuff uartBuffer;
extern GSM_MQTT MQTT;

uint8_t GSM_Response = 0;
unsigned long previousMillis = 0;
uint8_t pingSentCounter = 0;
bool stringComplete = false;
volatile uint32_t counter = 0;


/* Users' settings
 * ---------------------------------------------------------------------------
 * */

char MQTT_HOST[] = "mantis1.tmit.bme.hu";	//MQTT broker adress
char MQTT_PORT[] = "80";					//MQTT broker portnumber
char clientName[] = "MANTIS_BOX_3";			//MQTT clientname
char GSM_provider[] = "internet.telekom";	//SIM CARD's GSM Provider
char pingRespMultiplier = 3;				//Below

/*The client waits KeepAliveTime*pingRespMultiplier time to receive PingResponse,
 * if PingResponse didn't show up, the client tries to reconnect to the broker
 */

/* Settings section ends here
 * ---------------------------------------------------------------------------
 * */

/*------------------------------------------------------------------------------*/

/* Users' function section starts here
 * ---------------------------------------------------------------------------
 * */

/* Init UARTbuffer and MQTT structure */
void MQTT_start(UART_HandleTypeDef *huart, unsigned long KeepAlive)
{
	GSM_MQTT_constructor(&MQTT, KeepAlive, huart);
}

/* Autoconnect - automatically connects to MQTT server via TCP */
void AutoConnect(GSM_MQTT *object)
{
	_connect(object, clientName, 0, 0, "", "", 1, 0, 0, 0, "", "");
}

/* When connection is stable */
void OnConnect(GSM_MQTT *object)
{

}

/* When message is received */
void OnMessage(char *Topic, int TopicLength, char *Message, int MessageLength)
{

}

/* Users' function section ends here
 * ---------------------------------------------------------------------------
 * */


/* Arduino's millis() function equivalent (counting starts at initialization GSM_MQTT object - 1st function
 * Call this function in SysTick_Handler_GSM */
void SysTick_Handler_GSM(void)
{
  counter++;
}

/* Arduino's millis() function equivalent - 2nd function */
uint32_t millis()
{
  return counter;
}

void initSerialBuf(serialBuff *buff)
{
	buff->buffer_length = UART_BUFFER_LENGTH;
	buff->buffer_head = 0;
	buff->buffer_tail = 0;
	buff->temp_val = 0;
}

/* Returns a number of elements that serial buffer a contains */
int serialAvailable(GSM_MQTT *object, serialBuff *buff)
{
	if(object->uartFlag == 1)
	{
		object->uartFlag = 0;
		HAL_UART_Receive_IT(object->gsm_uart, &uartBuffer.temp_val, sizeof(char));
	}
	if(buff->buffer_tail > buff->buffer_head)
	{
		return (buff->buffer_length) - (buff->buffer_tail) + (buff->buffer_head);
	}
	else
	{
		return (buff->buffer_head)-(buff->buffer_tail);
	}

}

void serialGet()
{
	uartBuffer.buffer_t[uartBuffer.buffer_head] = uartBuffer.temp_val;
	uartBuffer.buffer_head++;
	if(uartBuffer.buffer_head == uartBuffer.buffer_length)
	{
		uartBuffer.buffer_head = 0;
	}
	if(HAL_UART_Receive_IT(MQTT.gsm_uart, &uartBuffer.temp_val, sizeof(char)) != HAL_OK)
	{
			MQTT.uartFlag = 1 ;
	}
}

char serialRead(serialBuff *buff)
{
	char data = 0;
	data = (char)buff->buffer_t[buff->buffer_tail];
	buff->buffer_tail++;
	if(buff->buffer_tail == buff->buffer_length)
	{
		buff->buffer_tail = 0;
	}
	return data;
}

/*  */
void serialPrint(GSM_MQTT *object, char value)
{
	uint8_t out_val = value;
	HAL_UART_Transmit(object->gsm_uart, &out_val, sizeof(char), HAL_MAX_DELAY);
}

/* Use when you want to send a char string */
void serialWrite(GSM_MQTT *object, char *string)
{
	int string_length = strlen(string);
	uint8_t s_buffer[string_length];
	for(int i = 0; i < string_length; i++)
	{
		s_buffer[i] = (uint8_t)string[i];
	}
	HAL_UART_Transmit(object->gsm_uart, s_buffer, sizeof(s_buffer), HAL_MAX_DELAY);
}

/* Logging function - you can write your code below to log */
void softLog(char *inputString)
{
	/* Insert logging function here */
	//HAL_UART_Transmit(&loguart, inputString, sizeof(inputString), HAL_MAX_DELAY);

}

/* Logging function, it puts after the string a carraige return (/r) and a newline character (/n)
 *  - you can write your code below to log*/
void softLogLn(char *inputString)
{
	int slen = strlen(inputString)+2;
	char outString[slen];
	strcpy(outString, inputString);
	outString[slen-1] = 13;
	outString[slen] = 10;

	/* Insert logging function here */
	//HAL_UART_Transmit(&loguart, outString, sizeof(outString), HAL_MAX_DELAY);
}

/* Print to trace a number as ASCII characters*/
void softLogSin(int value)
{
	char outChar[30];
	itoa(value, outChar, 10);
	softLog(outChar);
}

/* Print to trace a number as ASCII characters with carraige return (/r) and a newline character (/n)*/
void softLogSinLn(int value)
{
	char outChar[30];
	itoa(value, outChar, 10);
	softLogLn(outChar);
}

void GSM_MQTT_constructor(GSM_MQTT *object, unsigned long KeepAlive, UART_HandleTypeDef *huart)
{
	/* Set default values*/
	object->TCP_Flag = false;
	object->pingFlag = false;
	object->tcpATerrorcount = 0;
	object->MQTT_Flag = false;
	object->ConnectionAcknowledgement = NO_ACKNOWLEDGEMENT;
	object->PublishIndex = 0;
	object->TopicLength = 0;
	object->MessageLength = 0;
	object->MessageFlag = false;
	object->modemStatus = 0;
	object->index = 0;
	object->length = 0;
	object->lengthLocal = 0;
	object->_LastMessaseID = 0;
	object->_ProtocolVersion = 4;
	object->_PingPrevMillis = 0;
	object->_tcpStatus = 0;
	object->_tcpStatusPrev = 0;
	object->_KeepAliveTimeOut = KeepAlive;

	/* UART settings */
	object->gsm_uart = huart;
	object->uartFlag = 0;
	object->respFlag = false;

	/*start SysTick - Aurdino "millis" equivalent */
	SysTick_Config(SystemCoreClock / 1000);
}

void begin(GSM_MQTT* object)
{
	/* Enable IT to receive data */
	if(HAL_UART_Receive_IT(object->gsm_uart, &uartBuffer.temp_val, 1) != HAL_OK)
	{
		object->uartFlag = 1 ;
	}

	_sendAT(object, "AT\r\n", 3000);

	//If your simcard havs a pincode, uncomment this line below
	//_sendAT(object, "AT+CPIN=2335\r\n", 5000);

	//HAL_Delay(15000);

	_tcpInit(object);
}

char _sendAT(GSM_MQTT* object, char *command, unsigned long waitms)
{
  unsigned long PrevMillis = millis();
  strcpy(object->reply, "none");
  GSM_Response = 0;
  serialWrite(object, command);
  unsigned long currentMillis = millis();
  while ((GSM_Response == 0) && ((currentMillis - PrevMillis) < waitms))
  {
    serialEvent();
    currentMillis = millis();
  }
  return GSM_Response;
}

char sendATreply(GSM_MQTT *object, char *command, char *replystr, unsigned long waitms)
{
  strcpy(object->reply, replystr);
  unsigned long PrevMillis = millis();
  object->GSM_ReplyFlag = 0;
  serialWrite(object, command);
  unsigned long currentMillis = millis();
  while ((object->GSM_ReplyFlag == 0) && ((currentMillis - PrevMillis) < waitms))
  {
    serialEvent();
    currentMillis = millis();
  }
  return object->GSM_ReplyFlag;
}

void _tcpInit(GSM_MQTT* object)
{
  switch (object->modemStatus)
  {
    case 0:
      {
        _sendAT(object, "+++", 1500);
        if(_sendAT(object, "AT\r\n", 3000) == 1)
        {
          object->modemStatus = 1;
        }
        else
        {
          object->modemStatus = 0;
          break;
        }
      }
      /* no break */
    case 1:
      {
        if(_sendAT(object, "ATE1\r\n", 3000) == 1)
        {
          object->modemStatus = 2;
        }
        else
        {
          object->modemStatus = 1;
          break;
        }
      }
      /* no break */
    case 2:
      {
        if (sendATreply(object, "AT+CREG?\r\n", "0,1", 5000) == 1)
        {
          _sendAT(object, "AT+CIPMUX=0\r\n", 2000);
          _sendAT(object, "AT+CIPMODE=1\r\n", 2000);
          if(sendATreply(object, "AT+CGATT?\r\n", ": 1", 4000) != 1)
          {
            _sendAT(object, "AT+CGATT=1\r\n", 2000);
          }
          object->modemStatus = 3;
          object->_tcpStatus = 2;
        }
        else
        {
          object->modemStatus = 2;
          break;
        }
      }
      /* no break */
    case 3:
      {
        if(object->GSM_ReplyFlag != 7)
        {
          object->_tcpStatus = sendATreply(object, "AT+CIPSTATUS\r\n", "STATE", 5000);
          if(object->_tcpStatusPrev == object->_tcpStatus)
          {
            object->tcpATerrorcount++;
            if(object->tcpATerrorcount >= 10)
            {
              object->tcpATerrorcount = 0;
              object->_tcpStatus = 7;
            }
          }
          else
          {
            object->_tcpStatusPrev = object->_tcpStatus;
            object->tcpATerrorcount = 0;
          }
        }
        object->_tcpStatusPrev = object->_tcpStatus;
        switch (object->_tcpStatus)
        {
          case 2:
            {
              /*_sendAT(object, "AT+CGDCONT=1,\"IP\",\"internet.telekom\"\r\n", 5000); //Only at first use
              serialWrite(object, "CGDCONT=1,\"IP\",\"") ;
              serialWrite(object, GSM_provider) ;
              _sendAT(object, "\"\r\n", 5000); */

              serialWrite(object, "AT+CSTT=\"") ;
              serialWrite(object, GSM_provider) ;
              _sendAT(object, "\"\r\n", 5000);
              break;
            }
          case 3:
            {
              _sendAT(object, "AT+CIICR\r\n", 5000);
              break;
            }
          case 4:
            {
              sendATreply(object, "AT+CIFSR\r\n", ".", 4000) ;
              break;
            }
          case 5:
            {
              serialWrite(object, "AT+CIPSTART=\"TCP\",\"");
              serialWrite(object, MQTT_HOST);
              serialWrite(object, "\",\"");
              serialWrite(object, MQTT_PORT);
              if(_sendAT(object, "\"\r\n", 5000) == 1)
              {
                unsigned long PrevMillis = millis();
                unsigned long currentMillis = millis();
                while ( (GSM_Response != 4) && ((currentMillis - PrevMillis) < 20000) )
                {
                  serialEvent();
                  currentMillis = millis();
                }
              }
              break;
            }
          case 6:
            {
              unsigned long PrevMillis = millis();
              unsigned long currentMillis = millis();
              while ( (GSM_Response != 4) && ((currentMillis - PrevMillis) < 20000) )
              {
                serialEvent();
                currentMillis = millis();
              }
              break;
            }
          case 7:
            {
              sendATreply(object, "AT+CIPSHUT\r\n", "OK", 4000) ;
              object->modemStatus = 0;
              object->_tcpStatus = 2;
              break;
            }
        }
      }
  }
}

void _ping(GSM_MQTT *object)
{
  if (object->pingFlag == true)
  {
    unsigned long currentMillis = millis();
    if ((currentMillis - object->_PingPrevMillis ) >= object->_KeepAliveTimeOut * 1000)
    {
      object->_PingPrevMillis = currentMillis;
      serialPrint(object, PINGREQ * 16);
      _sendLength(object, 0);
      pingSentCounter++;
    }
  }
}

void _pingRespHandler(GSM_MQTT *object)
{
	if(object->respFlag == false)
	{
		//disconnect(object);
		_sendAT(object, "+++", 1500);
		_sendAT(object, "AT\r\n", 1000);
		object->_tcpStatus = sendATreply(object, "AT+CIPSTATUS\r\n", "STATE", 5000);
		object->TCP_Flag = false;
		object->MQTT_Flag = false;
		object->pingFlag = false;
	}
	object->respFlag = false;
}

void _sendUTFString(GSM_MQTT *object, char *string)
{
  int localLength = strlen(string);
  serialPrint(object, localLength / 256);
  serialPrint(object, localLength % 256);
  serialWrite(object, string);
}

void _sendLength(GSM_MQTT *object, int len)
{
  bool length_flag = false;
  while (length_flag == false)
  {
    if ((len / 128) > 0)
    {
      serialPrint(object, len % 128 + 128);
      len /= 128;
    }
    else
    {
      length_flag = true;
      serialPrint(object, len);
    }
  }
}

void _connect(GSM_MQTT *object, char *ClientIdentifier, char UserNameFlag, char PasswordFlag, char *UserName, char *Password, char CleanSession, char WillFlag, char WillQoS, char WillRetain, char *WillTopic, char *WillMessage)
{
  object->ConnectionAcknowledgement = NO_ACKNOWLEDGEMENT ;
  serialPrint(object, CONNECT * 16);
  char ProtocolName[] = "MQTT";
  int localLength = (2 + strlen(ProtocolName)) + 1 + 3 + (2 + strlen(ClientIdentifier));
  if (WillFlag != 0)
  {
    localLength = localLength + 2 + strlen(WillTopic) + 2 + strlen(WillMessage);
  }
  if (UserNameFlag != 0)
  {
    localLength = localLength + 2 + strlen(UserName);

    if (PasswordFlag != 0)
    {
      localLength = localLength + 2 + strlen(Password);
    }
  }
  _sendLength(object, localLength);
  _sendUTFString(object, ProtocolName);
  serialPrint(object, object->_ProtocolVersion);
  serialPrint(object, UserNameFlag * User_Name_Flag_Mask +
		  PasswordFlag * Password_Flag_Mask + WillRetain * Will_Retain_Mask + WillQoS *
		  Will_QoS_Scale + WillFlag * Will_Flag_Mask + CleanSession *
		  Clean_Session_Mask);
  serialPrint(object, object->_KeepAliveTimeOut / 256);
  serialPrint(object, object->_KeepAliveTimeOut % 256);
  _sendUTFString(object, ClientIdentifier);
  if (WillFlag != 0)
  {
    _sendUTFString(object, WillTopic);
    _sendUTFString(object, WillMessage);
  }
  if (UserNameFlag != 0)
  {
    _sendUTFString(object, UserName);
    if (PasswordFlag != 0)
    {
      _sendUTFString(object, Password);
    }
  }
}

void publish(GSM_MQTT *object, char DUP, char Qos, char RETAIN, unsigned int MessageID, char *Topic, char *Message)
{
	serialPrint(object, PUBLISH * 16 + DUP * DUP_Mask + Qos * QoS_Scale + RETAIN);
  int localLength = (2 + strlen(Topic));
  if (Qos > 0)
  {
    localLength += 2;
  }
  localLength += strlen(Message);
  _sendLength(object, localLength);
  _sendUTFString(object, Topic);
  if (Qos > 0)
  {
	serialPrint(object, MessageID / 256);
	serialPrint(object, MessageID % 256);
  }
  serialWrite(object, Message);
}

void publishACK(GSM_MQTT *object, unsigned int MessageID)
{
	serialPrint(object, PUBACK * 16);
    _sendLength(object, 2);
  	serialPrint(object, MessageID / 256);
  	serialPrint(object, MessageID % 256);
}

void publishREC(GSM_MQTT *object, unsigned int MessageID)
{
	serialPrint(object, PUBREC * 16);
	_sendLength(object, 2);
	serialPrint(object, MessageID / 256);
	serialPrint(object, MessageID % 256);
}

void publishREL(GSM_MQTT *object, char DUP, unsigned int MessageID)
{
	serialPrint(object, PUBREL * 16 + DUP * DUP_Mask + 1 * QoS_Scale);
    _sendLength(object, 2);
    serialPrint(object, MessageID / 256);
  	serialPrint(object, MessageID % 256);
}

void publishCOMP(GSM_MQTT *object, unsigned int MessageID)
{
	  serialPrint(object, PUBCOMP * 16);
	  _sendLength(object, 2);
	  serialPrint(object, MessageID / 256);
	  serialPrint(object, MessageID % 256);
}

void subscribe(GSM_MQTT *object, char DUP, unsigned int MessageID, char *SubTopic, char SubQoS)
{
	serialPrint(object, SUBSCRIBE * 16 + DUP * DUP_Mask + 1 * QoS_Scale);
	int localLength = 2 + (2 + strlen(SubTopic)) + 1;
	_sendLength(object, localLength);
	serialPrint(object, MessageID / 256);
	serialPrint(object, MessageID % 256);
	_sendUTFString(object, SubTopic);
	serialPrint(object, SubQoS);
}

void unsubscribe(GSM_MQTT *object, char DUP, unsigned int MessageID, char *SubTopic)
{
	serialPrint(object, UNSUBSCRIBE * 16 + DUP * DUP_Mask + 1 * QoS_Scale);
	int localLength = (2 + strlen(SubTopic)) + 2;
	_sendLength(object, localLength);

  	serialPrint(object, MessageID / 256);
  	serialPrint(object, MessageID % 256);

  	_sendUTFString(object, SubTopic);
}

void disconnect(GSM_MQTT *object)
{
  serialPrint(object, DISCONNECT * 16);
  _sendLength(object, 0);
  object->pingFlag = false;
}

/* Messages */
char CONNECTMessage[] = "Client request to connect to Server\r\n";
char CONNACKMessage[] = "Connect Acknowledgment\r\n";
char PUBLISHMessage[] = "Publish message\r\n";
char PUBACKMessage[]  = "Publish Acknowledgment\r\n";
char PUBRECMessage[]	= "Publish Received (assured delivery part 1)\r\n";
char PUBRELMessage[]	= "Publish Release (assured delivery part 2)\r\n";
char PUBCOMPMessage[] = "Publish Complete (assured delivery part 3)\r\n";
char SUBSCRIBEMessage[]	= "Client Subscribe request\r\n";
char SUBACKMessage[] 		= "Subscribe Acknowledgment\r\n";
char UNSUBSCRIBEMessage[] = "Client Unsubscribe request\r\n";
char UNSUBACKMessage[] 	= "Unsubscribe Acknowledgment\r\n";
char PINGREQMessage[] 	= "PING Request\r\n";
char PINGRESPMessage[]	= "PING Response\r\n";
char DISCONNECTMessage[]	= "Client is Disconnecting\r\n";

void printMessageType(uint8_t Message)
{
  switch (Message)
  {
    case CONNECT:
      {
        //softLog(CONNECTMessage);
        break;
      }
    case CONNACK:
      {
    	//softLog(CONNACKMessage);
        break;
      }
    case PUBLISH:
      {
		//softLog(PUBLISHMessage);
        break;
      }
    case PUBACK:
      {
    	//softLog(PUBACKMessage);
        break;
      }
    case  PUBREC:
      {
    	//softLog(PUBRECMessage);
        break;
      }
    case PUBREL:
      {
    	//softLog(PUBRELMessage);
        break;
      }
    case PUBCOMP:
      {
    	//softLog(PUBCOMPMessage);
        break;
      }
    case SUBSCRIBE:
      {
    	//softLog(SUBSCRIBEMessage);
        break;
      }
    case SUBACK:
      {
    	//softLog(SUBACKMessage);
        break;
      }
    case UNSUBSCRIBE:
      {
    	//softLog(UNSUBSCRIBEMessage);
        break;
      }
    case UNSUBACK:
      {
    	//softLog(UNSUBACKMessage);
        break;
      }
    case PINGREQ:
      {
    	//softLog(PINGREQMessage);
        break;
      }
    case PINGRESP:
      {
    	//softLog(PINGRESPMessage);
        break;
      }
    case DISCONNECT:
      {
    	//softLog(DISCONNECTMessage);
        break;
      }
  }
}

//Connect Ack
char ConnectAck0[]	= "Connection Accepted\r\n";
char ConnectAck1[]	= "Connection Refused: unacceptable protocol version\r\n";
char ConnectAck2[]	= "Connection Refused: identifier rejected\r\n";
char ConnectAck3[] 	= "Connection Refused: server unavailable\r\n";
char ConnectAck4[]	= "Connection Refused: bad user name or password\r\n";
char ConnectAck5[]	= "Connection Refused: not authorized\r\n";

void printConnectAck(uint8_t Ack)
{
  switch (Ack)
  {
    case 0:
      {
    	//softLog(ConnectAck0);
        break;
      }
    case 1:
      {
    	//softLog(ConnectAck1);
        break;
      }
    case 2:
      {
    	//softLog(ConnectAck2);
        break;
      }
    case 3:
      {
    	//softLog(ConnectAck3);
        break;
      }
    case 4:
      {
    	//softLog(ConnectAck4);
        break;
      }
    case 5:
      {
    	//softLog(ConnectAck5);
        break;
      }
  }
}

unsigned int _generateMessageID(GSM_MQTT *object)
{
  if (object->_LastMessaseID < 65535)
  {
    return (object->_LastMessaseID)++;
  }
  else
  {
    object->_LastMessaseID = 0;
    return object->_LastMessaseID;
  }
}

void processing(GSM_MQTT *object)
{
  if (object->TCP_Flag == false)
  {
    object->MQTT_Flag = false;
    _tcpInit(object);
  }

  if(pingSentCounter >= pingRespMultiplier)
  {
	  _pingRespHandler(object);
  	  pingSentCounter = 0;
  }

  _ping(object);
}

bool available(GSM_MQTT *object)
{
  return object->MQTT_Flag;
}

void serialEvent()
{
  while(serialAvailable(&MQTT ,&uartBuffer))
    if (MQTT.TCP_Flag == false)
    {
      char inChar = serialRead(&uartBuffer);
      if (MQTT.index < 200)
      {
        MQTT.inputString[MQTT.index++] = inChar;
      }
      if (inChar == '\n')
      {
        MQTT.inputString[MQTT.index] = 0;
        stringComplete = true;
        //softLog(MQTT.inputString);
        if (strstr(MQTT.inputString, MQTT.reply) != NULL)
        {
          MQTT.GSM_ReplyFlag = 1;
          if (strstr(MQTT.inputString, " INITIAL") != 0)
          {
            MQTT.GSM_ReplyFlag = 2;
          }
          else if (strstr(MQTT.inputString, " START") != 0)
          {
            MQTT.GSM_ReplyFlag = 3;
          }
          else if (strstr(MQTT.inputString, "IP CONFIG") != 0)
          {
            HAL_Delay(1);
            MQTT.GSM_ReplyFlag = 4;
          }
          else if (strstr(MQTT.inputString, " GPRSACT") != 0)
          {
            MQTT.GSM_ReplyFlag = 4;
          }
          else if ((strstr(MQTT.inputString, " STATUS") != 0) || (strstr(MQTT.inputString, "TCP CLOSED") != 0))
          {
            MQTT.GSM_ReplyFlag = 5;
          }
          else if (strstr(MQTT.inputString, " TCP CONNECTING") != 0)
          {
            MQTT.GSM_ReplyFlag = 6;
          }
          else if ((strstr(MQTT.inputString, " CONNECT OK") != 0) || (strstr(MQTT.inputString, "CONNECT FAIL") != NULL) || (strstr(MQTT.inputString, "PDP DEACT") != 0))
          {
            MQTT.GSM_ReplyFlag = 7;
          }
        }
        else if (strstr(MQTT.inputString, "OK") != NULL)
        {
          GSM_Response = 1;
        }
        else if (strstr(MQTT.inputString, "ERROR") != NULL)
        {
          GSM_Response = 2;
        }
        else if (strstr(MQTT.inputString, ".") != NULL)
        {
          GSM_Response = 3;
        }
        else if (strstr(MQTT.inputString, "CONNECT FAIL") != NULL)
        {
          GSM_Response = 5;
        }
        else if (strstr(MQTT.inputString, "CONNECT") != NULL)
        {
          GSM_Response = 4;
          MQTT.TCP_Flag = true;
          //softLogLn("MQTT.TCP_Flag = True");
          AutoConnect(&MQTT);
          MQTT.pingFlag = true;
          MQTT.tcpATerrorcount = 0;
        }
        else if (strstr(MQTT.inputString, "CLOSED") != NULL)
        {
          GSM_Response = 4;
          MQTT.TCP_Flag = false;
          MQTT.MQTT_Flag = false;
        }
        MQTT.index = 0;
        MQTT.inputString[0] = 0;
      }
    }
    else
    {
      char inChar = serialRead(&uartBuffer);
      uint8_t ReceivedMessageType = (inChar / 16) & 0x0F;
      uint8_t QoS = (inChar & QoS_Mask) / QoS_Scale; //DUP and QoS settings
      uint8_t RETAIN = (inChar & RETAIN_Mask);
      if ((ReceivedMessageType >= CONNECT) && (ReceivedMessageType <= DISCONNECT))
      {
        bool NextLengthByte = true;
        MQTT.length = 0;
        MQTT.lengthLocal = 0;
        uint32_t multiplier=1;
        HAL_Delay(2);
        char Cchar = inChar;
        while ( (NextLengthByte == true) && (MQTT.TCP_Flag == true))
        {
          if (serialAvailable(&MQTT, &uartBuffer))
          {
            inChar = serialRead(&uartBuffer);
            ////softLogSin(inChar, 1);
            if ((((Cchar & 0xFF) == 'C') && ((inChar & 0xFF) == 'L') && (MQTT.length == 0)) || (((Cchar & 0xFF) == '+') && ((inChar & 0xFF) == 'P') && (MQTT.length == 0)))
            {
              MQTT.index = 0;
              MQTT.inputString[MQTT.index++] = Cchar;
              MQTT.inputString[MQTT.index++] = inChar;
              MQTT.TCP_Flag = false;
              MQTT.MQTT_Flag = false;
              MQTT.pingFlag = false;
              //softLogLn("Disconnecting");
            }
            else
            {
              if ((inChar & 128) == 128)
              {
                MQTT.length += (inChar & 127) *  multiplier;
                multiplier *= 128;
                //softLogLn("More");
              }
              else
              {
                NextLengthByte = false;
                MQTT.length += (inChar & 127) *  multiplier;
                multiplier *= 128;
              }
            }
          }
        }
        MQTT.lengthLocal = MQTT.length;
        //softLogSinLn(MQTT.length);
        if (MQTT.TCP_Flag == true)
        {
          printMessageType(ReceivedMessageType);
          MQTT.index = 0L;
          uint32_t a = 0;
          while ((MQTT.length-- > 0) && (serialAvailable(&MQTT, &uartBuffer)))
          {
            MQTT.inputString[MQTT.index++] = serialRead(&uartBuffer);
            HAL_Delay(1);
          }
          //softLogLn(" ");
          if (ReceivedMessageType == CONNACK)
          {
            MQTT.ConnectionAcknowledgement = MQTT.inputString[0] * 256 + MQTT.inputString[1];
            if (MQTT.ConnectionAcknowledgement == 0)
            {
              MQTT.MQTT_Flag = true;
              OnConnect(&MQTT);
            }
            printConnectAck(MQTT.ConnectionAcknowledgement);
            // MQTT.OnConnect();
          }
          else if (ReceivedMessageType == PUBLISH)
          {
            uint32_t TopicLength = (MQTT.inputString[0]) * 256 + (MQTT.inputString[1]);
            //softLog("Topic : '");
            MQTT.PublishIndex = 0;
            if(TopicLength > TOPIC_BUFFER_LENGTH)
            {
            	disconnect(&MQTT);
            	AutoConnect(&MQTT);
            }
            for (uint32_t iter = 2; iter < TopicLength + 2; iter++)
            {
              //softLog(MQTT.inputString[iter], 0);
              MQTT.Topic[MQTT.PublishIndex++] = MQTT.inputString[iter];
            }
            MQTT.Topic[MQTT.PublishIndex] = 0;
            //softLog("' \r\n Message :'");
            MQTT.TopicLength = MQTT.PublishIndex;

            MQTT.PublishIndex = 0;
            uint32_t MessageSTART = TopicLength + 2UL;
            int MessageID = 0;
            if (QoS != 0)
            {
              MessageSTART += 2;
              MessageID = MQTT.inputString[TopicLength + 2UL] * 256 + MQTT.inputString[TopicLength + 3UL];
            }
            if(MQTT.lengthLocal > MESSAGE_BUFFER_LENGTH)
            {
            	disconnect(&MQTT);
            	AutoConnect(&MQTT);
            }
            for (uint32_t iter = (MessageSTART); iter < (MQTT.lengthLocal); iter++)
            {
              //softLog(MQTT.inputString[iter]);
              MQTT.Message[MQTT.PublishIndex++] = MQTT.inputString[iter];
            }
            MQTT.Message[MQTT.PublishIndex] = 0;
            //softLogLn("'");
            MQTT.MessageLength = MQTT.PublishIndex;
            if (QoS == 1)
            {
              publishACK(&MQTT, MessageID);
            }
            else if (QoS == 2)
            {
              publishREC(&MQTT, MessageID);
            }
            OnMessage(MQTT.Topic, MQTT.TopicLength, MQTT.Message, MQTT.MessageLength);
            MQTT.MessageFlag = true;
          }
          else if (ReceivedMessageType == PUBREC)
          {
            //softLog("Message ID :");
            publishREL(&MQTT, 0, MQTT.inputString[0] * 256 + MQTT.inputString[1]);
            //softLogSinLn(MQTT.inputString[0] * 256 + MQTT.inputString[1]);

          }
          else if (ReceivedMessageType == PUBREL)
          {
            //softLog("Message ID :");
            publishCOMP(&MQTT, MQTT.inputString[0] * 256 + MQTT.inputString[1]);
            //softLogSinLn(MQTT.inputString[0] * 256 + MQTT.inputString[1]);

          }
          else if ((ReceivedMessageType == PUBACK) || (ReceivedMessageType == PUBCOMP) || (ReceivedMessageType == SUBACK) || (ReceivedMessageType == UNSUBACK))
          {
            //softLog("Message ID :");
            //softLogSinLn(MQTT.inputString[0] * 256 + MQTT.inputString[1]) ;
          }
          else if (ReceivedMessageType == PINGREQ)
          {
            MQTT.TCP_Flag = false;
            MQTT.pingFlag = false;
            //softLogLn("Disconnecting");
            sendATreply(&MQTT, "AT+CIPSHUT\r\n", ".", 4000) ;
            MQTT.modemStatus = 0;
          }
          else if (ReceivedMessageType == PINGRESP)
          {
        	  MQTT.respFlag = true;
          }
        }
      }
      else if ((inChar = 13) || (inChar == 10))
      {
      }
      else
      {
        //softLog("Received :Unknown Message Type :", 0);
        //softLogLn(inChar);
      }
    }
}

