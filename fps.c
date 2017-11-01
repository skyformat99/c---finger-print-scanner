
#include "fps.h"
#include <stdio.h>
/*
#include "_Uart.h"
#include "Utils.h"
#include "Uart.h"
*/
// Returns the high byte from a word
U8 GetHighByte(U16 w)
{
    return (U8)(w>>8)&0x00FF;
}

// Returns the low byte from a word
U8 GetLowByte(U16 w)
{
    return (U8)w&0x00FF;
}

//command ckecksum  = offset cmd_ptr[0]+.....cmd_ptr[9]
U16 _CalculateChecksum(U8* ptr_temp ,U32 length)
{
   

	U16 w = 0;
	w += COMMAND_START_CODE_1 ;
	w += COMMAND_START_CODE_2;
	w += COMMAND_DEVICE_ID_1;
	w += COMMAND_DEVICE_ID_2;
	w += ptr_temp[4];
	w += ptr_temp[5];
	w += ptr_temp[6];
	w += ptr_temp[7];
	w += ptr_temp[8];
	w += ptr_temp[9];

	return w;
}

//set int to 4 bytes array for command packet
void int_to_parameter(U32 i )

{
    
    ParameterBytes[0] = (i & 0x000000ff);
    ParameterBytes[1] = (i & 0x0000ff00) >> 8;
    ParameterBytes[2] = (i & 0x00ff0000) >> 16;
    ParameterBytes[3] = (i & 0xff000000) >> 24;
    

}

//max value is int 1000  3E8 HEX - max id.
// Gets an int from the parameter bytes
U32 IntFromParameter(U8* ParameterBytes )
{
    U32 retval = 0;
    retval = (retval << 16) + ParameterBytes[3];
    retval = (retval << 16) + ParameterBytes[2];
    retval = (retval << 16) + ParameterBytes[1];
    retval = (retval << 16) + ParameterBytes[0];
    return retval;
}


/*calculate response buffer checksum only 10 first bytes without recieving checksum*/
//ckecksum  = offset response_ptr[0]+.....response_ptr[9]
U16 CalculateChecksum(U8 buffer[], U32 length)
{
    U16 checksum = 0;
    U32 i = 0;
    for (i=0; i<length; i++)
    {
        checksum += buffer[i];
    }
    return checksum;
}


/*get response string and get ack|not-ack and error printing*/
U32 handleResponce(U8* buffer,U32 size)
{


U16 checksum = 0;
if (buffer[0]== COMMAND_START_CODE_1)
{
if (buffer[0] != COMMAND_START_CODE_1 && buffer[1]!=COMMAND_START_CODE_2 && buffer[2]!=COMMAND_DEVICE_ID_1 && buffer[3] != COMMAND_DEVICE_ID_2)
{
Util_Printf("error in the first 4 set values!!!\n " );
return EXIT_FAILURE;
}

if(buffer[8] != 0x30 && buffer[8]!= 0x31)
    {Util_Printf("error in ack low -> no such value:    " );
    Util_Printf("0x%x\n" , (buffer[8]));
    return EXIT_FAILURE;
    }

if (buffer[9] != 0x00)
{
 Util_Printf("error in ack high -> no such value \t   " );
 Util_Printf("%x  \n\n" ,(U8) buffer[9]);
return EXIT_FAILURE;
}

checksum = _CalculateChecksum(buffer ,checksumSize);
buffer[10] = GetLowByte(checksum);
buffer[11] = GetHighByte(checksum);

if (buffer[8] == 0x30)
					return  EXIT_SUCCESS;
                 

          // high byte   low byte
//handle_error(buffer[5],buffer[4]);

}
Util_Printf("action  -->  failed  \n");

return EXIT_FAILURE;
}

// parses bytes into one of the possible errors from the finger print scanner
enum error  handle_error(U8 high, U8 low)
{
 enum error e;
//high byte 0x10  low byte  is error number
if (high == 0x10)//only for checking
{

    switch (low) {
    case 0x00: e = NO_ERROR; Util_Printf("ERROR : 0x00 :ACTION SUCCESS \n"); break;//never be print. only for clearness
    case 0x01: e = NACK_TIMEOUT;Util_Printf("ERROR : 0x01 :NACK_TIMEOUT \n"); break;
    case 0x02: e = NACK_INVALID_BAUDRATE;Util_Printf("ERROR : 0x02 :NACK_INVALID_BAUDRATE \n"); break;
    case 0x03: e = NACK_INVALID_POS;Util_Printf("ERROR : 0x03 :NACK_INVALID_POS\n"); break;
    case 0x04: e = NACK_IS_NOT_USED; Util_Printf("ERROR : 0x04 :NACK_IS_NOT_USED\n"); break;
    case 0x05: e = NACK_IS_ALREADY_USED; Util_Printf("ERROR : 0x5 :NACK_IS_ALREADY_USED\n"); break;
    case 0x06: e = NACK_COMM_ERR;  Util_Printf("ERROR : 0x06 :NACK_COMM_ERR\n");break;
    case 0x07: e = NACK_VERIFY_FAILED; Util_Printf("ERROR : 0x07 :NACK_VERIFY_FAILED\n"); break;
    case 0x08: e = NACK_IDENTIFY_FAILED; Util_Printf("ERROR : 0x08 :NACK_IDENTIFY_FAILED\n"); break;
    case 0x09: e = NACK_DB_IS_FULL; Util_Printf("ERROR : 0x09 :NACK_DB_IS_FULL\n"); break;
    case 0x0A: e = NACK_DB_IS_EMPTY; Util_Printf("ERROR : 0x0A :NACK_DB_IS_EMPTY\n"); break;
    case 0x0B: e = NACK_TURN_ERR; Util_Printf("ERROR : 0x0B :NACK_TURN_ERR\n"); break;
    case 0x0C: e = NACK_BAD_FINGER; Util_Printf("ERROR : 0x0C :NACK_BAD_FINGER\n");break;
    case 0x0D: e = NACK_ENROLL_FAILED;  Util_Printf("ERROR : 0x0D :NACK_ENROLL_FAILED\n");break;
    case 0x0E: e = NACK_IS_NOT_SUPPORTED; Util_Printf("ERROR : 0x0E :NACK_IS_NOT_SUPPORTED\n"); break;
    case 0x0F: e = NACK_DEV_ERR; Util_Printf("ERROR : 0x0F : NACK_DEV_ERR\n"); break;
    case 0x10: e = NACK_CAPTURE_CANCELED;  Util_Printf("ERROR : 0x10 :NACK_CAPTURE_CANCELED\n");break;
    case 0x11: e = NACK_INVALID_PARAM; Util_Printf("ERROR : 0x11 :NACK_INVALID_PARAM\n"); break;
    case 0x12: e = NACK_FINGER_IS_NOT_PRESSED; Util_Printf("ERROR : 0x12 : NACK_FINGER_IS_NOT_PRESSED\n"); break;
    default :  e = NACK_UNEXPECTED_ERROR  ; Util_Printf("ERROR : UNEXPECTED ERROR \n");
    
   
  }
}
 Util_Printf("\n\n");
    return e;
}



//Initialises the device and gets ready for commands
U32 command_open(){

 U32 result = 0;
 U16 check = 0;
 U16 cmd = 0x01;
 U32 i = 0;
 U8* response ;
 U8 commandPacket[12] = {COMMAND_START_CODE_1,COMMAND_START_CODE_2 ,COMMAND_DEVICE_ID_1,COMMAND_DEVICE_ID_2};
 U8 responsePacket[12] = {0};
 U8 ParameterBytes[4] = {0};
 Util_Printf("FPS - open \n");
 commandPacket[4] = 0x00;
 commandPacket[5] = 0x00;
 commandPacket[6] = 0x00;
 commandPacket[7]  =0x00;
 commandPacket[8]  =GetLowByte(cmd);
 commandPacket[9]  =GetHighByte(cmd);
 for (i=0;i<checksumSize;i++)
     check+=commandPacket[i];

 commandPacket[10]  =GetLowByte(check);
 commandPacket[11]  =GetHighByte(check);
/*finish to create command packet send it by uart and get response*/
 response = _Uart_Send_get(commandPacket);
  for(i=0;i<12;)
			responsePacket[i++]=response[i++];
 result = handleResponce(responsePacket,STRING_SEND_SIZE);
 
 
 return result;

}




// Changes the baud rate of the connection
// Parameter: 9600 || 115200
// Returns: True if success, false if invalid baud
//i prefer work at default value as 9600 bps.
U32 ChangeBaudRate(U32 baud)
{
    U32 result = 0 , i = 0;
    U16 check = 0;
    U16 cmd = 0x04;
    U8* response ;
    U8 commandPacket[12] = {COMMAND_START_CODE_1,COMMAND_START_CODE_2 ,COMMAND_DEVICE_ID_1,COMMAND_DEVICE_ID_2}; 
    U8 responsePacket[12] = {0};
    U8 ParameterBytes[4] = {0};
     
    if (baud != 9600 || baud != 115200)
        {
	Util_Printf ("no valid baundrate  115200 or 9600 !!  \n ... please try again..\n");
	return EXIT_FAILURE;
	    }	
    if ((baud == 9600) ||  (baud == 115200))
    {

        Util_Printf("FPS - ChangeBaudRate");
        int_to_parameter(baud);
        commandPacket[4]=ParameterBytes[0];
        commandPacket[5]=ParameterBytes[1];
        commandPacket[6]=ParameterBytes[2];
        commandPacket[7]=ParameterBytes[3];
        commandPacket[8] = GetLowByte(cmd);
        commandPacket[9]= GetHighByte(cmd);
        check = _CalculateChecksum(commandPacket,checksumSize);
        commandPacket[10] = GetLowByte(check);
        commandPacket[11] = GetHighByte(check);
        response= _Uart_Send_get(commandPacket);
        for(i=0;i<12;)
			responsePacket[i++]=response[i++];
        result = handleResponce(responsePacket,STRING_SEND_SIZE);
              
       
        if (result == 0)
			return EXIT_SUCCESS;
		else
		
        return EXIT_FAILURE;

    }	
    	
  
   
    Util_Printf("error --> invalid baundrate \n");
    return EXIT_FAILURE; 
}
// Turns on or off the LED backlight
// Parameter: 1 turns on the backlight, 0 turns it off
// Returns: 1 if successful, 0 if not
U32 SetLED(U32 val)
{
    U32 result = 0,i=0;
    U16 check = 0;
    U16 cmd = 0x12;
    U8* response ;
    U8 commandPacket[12] = {COMMAND_START_CODE_1,COMMAND_START_CODE_2 ,COMMAND_DEVICE_ID_1,COMMAND_DEVICE_ID_2}; 
    U8 responsePacket[12] = {0};
    U8 ParameterBytes[4] = {0};
    if (val == 1)
         {
         commandPacket[4]=0x01;
         Util_Printf("FPS - LED ON");
         }
      if(val == 0)
        {
		 commandPacket[4]=0x00;
         Util_Printf("FPS - LED OFF");
		}  
        
        commandPacket[5]=0x00;
        commandPacket[6]=0x00;
        commandPacket[7]=0x00;
        commandPacket[8] = GetLowByte(cmd);
        commandPacket[9]= GetHighByte(cmd);
        check = _CalculateChecksum(commandPacket,checksumSize);
        commandPacket[10] = GetLowByte(check);
        commandPacket[11] = GetHighByte(check);
        response= _Uart_Send_get(commandPacket);
        result = handleResponce(responsePacket,STRING_SEND_SIZE);
        for(i=0;i<12;)
			responsePacket[i++]=response[i++];
        
        if(result == EXIT_SUCCESS)
			return EXIT_SUCCESS;
   
        return EXIT_FAILURE; 
}

// Gets the number of enrolled fingerprints
// Return: The total number of enrolled fingerprints
U32 GetEnrollCount(){

 U32 result = 0;
 U16 check = 0;
 U16 cmd = 0x20;
 U32 i = 0;
 U32 j = 4;
 U8* response ;
 U8 commandPacket[12] = {COMMAND_START_CODE_1,COMMAND_START_CODE_2 ,COMMAND_DEVICE_ID_1,COMMAND_DEVICE_ID_2}; 
 U8 responsePacket[12] = {0};
 U8 ParameterBytes[4] = {0};
 Util_Printf("FPS - get enroll count");
 commandPacket[4] = 0x00;
 commandPacket[5] = 0x00;
 commandPacket[6] = 0x00;
 commandPacket[7]  =0x00;
 commandPacket[8]  =GetLowByte(cmd);
 commandPacket[9]  =GetHighByte(cmd);
 for (i=0;i<checksumSize;i++)
     check+=commandPacket[i];

 commandPacket[10]  =GetLowByte(check);
 commandPacket[11]  =GetHighByte(check);
/*finish to create command packet send it by uart and get response*/
 response = _Uart_Send_get(commandPacket);
  for(i=0;i<12;)
		responsePacket[i++]=response[i++];
 result = handleResponce(responsePacket,STRING_SEND_SIZE);
 
 if (result == 1)
 {
	for(i=0;i<4;i++) 
	   ParameterBytes[i] = responsePacket[j];
	
	result = IntFromParameter(ParameterBytes);
	return result;   //return enroll clount	 
 }
	 
  Util_Printf("there %d registered people in FPS", i );
 
return EXIT_FAILURE;	 
 
}
// checks to see if the ID number is in use or not
// Parameter: 0-1000
// Return: 0 if the ID number is enrolled, -1 if not
U32 Chechenrolled(U32 id)
{
enum error e;	
U32 result = 0 ,i=0;
U16 check = 0;
U16 cmd = 0x21;
U8* response ;
U8 commandPacket[12] = {COMMAND_START_CODE_1,COMMAND_START_CODE_2 ,COMMAND_DEVICE_ID_1,COMMAND_DEVICE_ID_2}; 
U8 responsePacket[12] = {0};
U8 ParameterBytes[4] = {0};	
		
Util_Printf("FPS - check enrolled id");
int_to_parameter(id);
commandPacket[4]=ParameterBytes[0];
commandPacket[5]=ParameterBytes[1];
commandPacket[6]=ParameterBytes[2];
commandPacket[7]=ParameterBytes[3];
commandPacket[8] = GetLowByte(cmd);
commandPacket[9]= GetHighByte(cmd);
check = _CalculateChecksum(commandPacket,checksumSize);
commandPacket[10] = GetLowByte(check);
commandPacket[11] = GetHighByte(check);
response= _Uart_Send_get(commandPacket);
 for(i=0;i<12;)
		responsePacket[i++]=response[i++];
result = handleResponce(responsePacket,STRING_SEND_SIZE);	
if (result == EXIT_SUCCESS)
		return EXIT_SUCCESS; // id is used

e = handle_error(responsePacket[4] , responsePacket[5]);
if (e == NACK_INVALID_POS) 	return 1; //wrong input id
if (e == NACK_IS_NOT_USED)	return 2; //id not used  
	
Util_Printf("undefined error!!!   0x21");
return EXIT_FAILURE; 	
	
}


// Starts the Enrollment Process
// Parameter: 0-1000
// options:
//	0 - ACK
//	1 - Database is full
//	2 - Invalid Position
//	3 - Position(ID) is already used

U32 Enrollstart(U32 id)
{
 
 U32 result = 0 , i=0;
 U16 check = 0;
 U16 cmd = 0x22;
 U8* response ;
 U8 commandPacket[12] = {COMMAND_START_CODE_1,COMMAND_START_CODE_2 ,COMMAND_DEVICE_ID_1,COMMAND_DEVICE_ID_2}; 
 U8 responsePacket[12] = {0};
 U8 ParameterBytes[4] = {0};
 Util_Printf("FPS - start enrollment");
 int_to_parameter(id);
 commandPacket[4] = ParameterBytes[0];
 commandPacket[5] = ParameterBytes[1];
 commandPacket[6] = ParameterBytes[2];
 commandPacket[7]  =ParameterBytes[3];
 commandPacket[8]  =GetLowByte(cmd);
 commandPacket[9]  =GetHighByte(cmd);
 for (i=0;i<checksumSize;i++)
     check+=commandPacket[i];

 commandPacket[10]  =GetLowByte(check);
 commandPacket[11]  =GetHighByte(check);
//finish to create command packet send it by uart and get response
 response = _Uart_Send_get(commandPacket);
  for(i=0;i<12;)
		responsePacket[i++]=response[i++];
 result = handleResponce(responsePacket,STRING_SEND_SIZE);
 if (result == 1) 
	               return 0; //can start first enrollment
 else{//check error
	             // high byte   low byte
  result = handle_error(responsePacket[5],responsePacket[4]); //find error code and display on screen
  Util_Printf("action fail" );
  
  if (result == 0x09){return 1;} 
  else if (result == 0x03){return 2;}
  else if (result == 0x05){return 3;}
	 }	 
 Util_Printf("undefined error!!! 0x-22");
  return EXIT_FAILURE;
}

// Gets the first scan of an enrollment
// Return:
//	0 - ACK
//	1 - Enroll Failed
//	2 - Bad finger
//  3 - Used id
U32 Enroll1()
{
 enum error err;	
 U32 result = 0 , i=0;
 U16 check = 0;
 U16 cmd = 0x23;
 U8* response ;
 U8 commandPacket[12] = {COMMAND_START_CODE_1,COMMAND_START_CODE_2 ,COMMAND_DEVICE_ID_1,COMMAND_DEVICE_ID_2}; 
 U8 responsePacket[12] = {0};
 U8 ParameterBytes[4] = {0};
 Util_Printf("FPS - Enroll1");
 commandPacket[4] = 0;
 commandPacket[5] = 0;
 commandPacket[6] = 0;
 commandPacket[7]  =0;
 commandPacket[8]  =GetLowByte(cmd);
 commandPacket[9]  =GetHighByte(cmd);
 for (i=0;i<checksumSize;i++)
     check+=commandPacket[i];

 commandPacket[10]  =GetLowByte(check);
 commandPacket[11]  =GetHighByte(check);
//finish to create command packet send it by uart and get response
 response = (U8*)_Uart_Send_get(commandPacket);
  for(i=0;i<12;)
		responsePacket[i++]=response[i++];
 result = handleResponce(responsePacket,STRING_SEND_SIZE);
 if (result == EXIT_SUCCESS) {
	 result = IntFromParameter(ParameterBytes);
	 if (result<1000 ) 
	 clearBuffersData(commandPacket,responsePacket,ParameterBytes);
	               return 3;} //can make first  enrollment
 else{//check error
	             // high byte   low byte
  err = handle_error(responsePacket[5],responsePacket[4]); //find error code and display on screen
  Util_Printf(" enroll1 action fail" );
  clearBuffersData(commandPacket,responsePacket,ParameterBytes);
  if (err ==   NACK_ENROLL_FAILED )
					return 1; 
  else
  {if (err == NACK_BAD_FINGER)
		return 2;
 }	 	

}
Util_Printf("undefined error!!!   0x23");
return EXIT_FAILURE;
}

// Gets the second scan of an enrollment
// Return:
//	0 - ACK
//	1 - Enroll Failed
//	2 - Bad finger
//  3 - Used id
U32 Enroll2()
{
 enum error err;	
 U32 result = 0 , i=0;
 U16 check = 0;
 U16 cmd = 0x24;
 U8* response ;
 U8 commandPacket[12] = {COMMAND_START_CODE_1,COMMAND_START_CODE_2 ,COMMAND_DEVICE_ID_1,COMMAND_DEVICE_ID_2}; 
 U8 responsePacket[12] = {0};
 U8 ParameterBytes[4] = {0}; 
 Util_Printf("FPS - Enroll2");
 commandPacket[4] = 0;
 commandPacket[5] = 0;
 commandPacket[6] = 0;
 commandPacket[7]  =0;
 commandPacket[8]  =GetLowByte(cmd);
 commandPacket[9]  =GetHighByte(cmd);
 for (i=0;i<checksumSize;i++)
     check+=commandPacket[i];

 commandPacket[10]  =GetLowByte(check);
 commandPacket[11]  =GetHighByte(check);
//finish to create command packet send it by uart and get response
 response = _Uart_Send_get(commandPacket);
  for(i=0;i<12;)
		responsePacket[i++]=response[i++];
 result = handleResponce(responsePacket,STRING_SEND_SIZE);
 if (result == EXIT_SUCCESS) {
	 result = IntFromParameter(ParameterBytes);
	 if (result<1000 ) 
	 clearBuffersData(commandPacket,responsePacket,ParameterBytes);
	               return 3;} //can make second  enrollment
 else{//check error
	             // high byte   low byte
  err = handle_error(responsePacket[5],responsePacket[4]); //find error code and display on screen
  Util_Printf(" enroll2 action fail" );
  clearBuffersData(commandPacket,responsePacket,ParameterBytes);
  if (err == NACK_ENROLL_FAILED)
					return 1; 
  else
  {if (err == NACK_BAD_FINGER)
		return 2;
 }	 	

}
Util_Printf("undefined error!!!  0x24");
return EXIT_FAILURE;
}


// Gets the third scan of an enrollment
// Return:
//	0 - ACK
//	1 - Enroll Failed
//	2 - Bad finger
//  3 - Used id
U32 Enroll3()
{
 enum error err;	
 U32 result = 0 , i=0;
 U16 check = 0;
 U16 cmd = 0x25;
 U8* response ;
U8 commandPacket[12] = {COMMAND_START_CODE_1,COMMAND_START_CODE_2 ,COMMAND_DEVICE_ID_1,COMMAND_DEVICE_ID_2}; 
 U8 responsePacket[12] = {0};
 U8 ParameterBytes[4] = {0};
 Util_Printf("FPS - Enroll3");
 commandPacket[4] = 0;
 commandPacket[5] = 0;
 commandPacket[6] = 0;
 commandPacket[7]  =0;
 commandPacket[8]  =GetLowByte(cmd);
 commandPacket[9]  =GetHighByte(cmd);
 for (i=0;i<checksumSize;i++)
     check+=commandPacket[i];

 commandPacket[10]  =GetLowByte(check);
 commandPacket[11]  =GetHighByte(check);
//finish to create command packet send it by uart and get response
 response = _Uart_Send_get(commandPacket);
  for(i=0;i<12;)
		responsePacket[i++]=response[i++];
 result = handleResponce(responsePacket,STRING_SEND_SIZE);
 if (result == EXIT_SUCCESS) {//parameterbytes = id in parameter
	 result = IntFromParameter(ParameterBytes);
	 if (result<1000 ) 
	 clearBuffersData(commandPacket,responsePacket,ParameterBytes);
	               return 3;} //can make third  enrollment
 else{//check error
	             // high byte   low byte
  err = handle_error(responsePacket[5],responsePacket[4]); //find error code and display on screen
  Util_Printf(" enroll3 action fail" );
  clearBuffersData(commandPacket,responsePacket,ParameterBytes);
  if (err == NACK_ENROLL_FAILED )
					return 1; 
  else
  {if (err == NACK_BAD_FINGER)
		return 2;
 }	 	

}
Util_Printf("undefined error!!!  0x25");
return EXIT_FAILURE;
}

//This command is used while enrollment, the host waits to take off the finger per
//enrollment stage.  return : parameter-> 0  for pressed
//                            parameter != 0 for not pressed 
U32 IsPressFinger()
{
U32 result = 0 ,i=0;
U16 check = 0;
U16 cmd = 0x26;
U8 commandPacket[12] = {COMMAND_START_CODE_1,COMMAND_START_CODE_2 ,COMMAND_DEVICE_ID_1,COMMAND_DEVICE_ID_2}; 
U8 responsePacket[12] = {0};
U8 ParameterBytes[4] = {0};
U8* response =  responsePacket; 	
		
Util_Printf("FPS - Is Pressed Finger ?");
commandPacket[4]=0;
commandPacket[5]=0;
commandPacket[6]=0;
commandPacket[7]=0;
commandPacket[8]=GetLowByte(cmd);
commandPacket[9]=GetHighByte(cmd);
check = _CalculateChecksum(commandPacket,checksumSize);
commandPacket[10] = GetLowByte(check);
commandPacket[11] = GetHighByte(check);
response= _Uart_Send_get(commandPacket);
 for(i=0;i<12;)
		responsePacket[i++]=response[i++];
result = handleResponce(responsePacket,STRING_SEND_SIZE);
result = IntFromParameter(ParameterBytes);        
if (result == 0)
	return EXIT_SUCCESS;
	
Util_Printf("undefined error!!! 0x26");
  return EXIT_FAILURE;
		
}	

// Deletes the specified ID (enrollment) from the database
// Parameter: 0-1000 (id number to be deleted)
// Returns: true if successful, false if position invalid	
U32 DeleteId(U32 id)
{
 enum error err;	
 U32 result = 0 , i=0;
 U16 check = 0;
 U16 cmd = 0x40;
 U8* response ;
 U8 commandPacket[12] = {COMMAND_START_CODE_1,COMMAND_START_CODE_2 ,COMMAND_DEVICE_ID_1,COMMAND_DEVICE_ID_2}; 
 U8 responsePacket[12] = {0};
 U8 ParameterBytes[4] = {0};
 Util_Printf("FPS - Delete Id");
 int_to_parameter(id);
 commandPacket[4] = ParameterBytes[0];
 commandPacket[5] = ParameterBytes[1];
 commandPacket[6] = ParameterBytes[2];
 commandPacket[7]  =ParameterBytes[3];
 commandPacket[8]  =GetLowByte(cmd);
 commandPacket[9]  =GetHighByte(cmd);
 for (i=0;i<checksumSize;i++)
		check+=commandPacket[i];

 commandPacket[10]  =GetLowByte(check);
 commandPacket[11]  =GetHighByte(check);
//finish to create command packet send it by uart and get response
 response = _Uart_Send_get(commandPacket);
  for(i=0;i<12;)
		responsePacket[i++]=response[i++];
 result = handleResponce(responsePacket,STRING_SEND_SIZE);
 if (result == EXIT_SUCCESS) {clearBuffersData(commandPacket,responsePacket,ParameterBytes);
	               return EXIT_SUCCESS;} //can start first enrollment
 else{//check error
	             
  result = IntFromParameter(ParameterBytes); //find error code and display on screen
                         // high byte   low byte
  err = handle_error(responsePacket[4] ,responsePacket[5]);
  Util_Printf("action fail !!!" );
  clearBuffersData(commandPacket,responsePacket,ParameterBytes);
  if (err == NACK_INVALID_POS )  return 1; 
}
  Util_Printf("undefined error!!! 0x40");
  return EXIT_FAILURE;
}	


	

