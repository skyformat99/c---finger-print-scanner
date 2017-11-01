#ifndef _FPS_H_
#define _FPS_H_


#include <stdio.h>
#include "types.h"
#include "_Uart.h"
#include "Utils.h"
#include "Uart.h"



/*-----set static variable of commands and response packet never changes-----*/
#define COMMAND_START_CODE_1 0x55
#define COMMAND_START_CODE_2 0xAA
#define COMMAND_DEVICE_ID_1  0x01
#define COMMAND_DEVICE_ID_2  0x00
/* *************************************************************************** */
#define STRING_SEND_SIZE 12
#define checksumSize 10
#define EXIT_FAILURE -1
#define EXIT_SUCCESS  0

enum cmd
{

    open =              0x01,//01 Open Initialization
    close =             0x02,// 02 Close Termination
    usbckeck =          0x03,// 03 UsbInternalCheck Check if the connected USB device is valid
    changeBaundrate =   0x04,// 04 ChangeBaudrate Change UART baud rate
    Cmosled =           0x12,// 12 CmosLed Control CMOS LED
    GetEnrolleount =    0x20,// 20 GetEnrollCount Get enrolled fingerprint count
    ChechEnrolled =     0x21,// 21 CheckEnrolled Check whether the specified ID is already enrolled
    EnrollStart  =      0x22,// 22 EnrollStart Start an enrollment
    enroll1 =           0x23,// 23 Enroll1 Make 1st template for an enrollment
    enroll2 =           0x24,// 24 Enroll2 Make 2nd template for an enrollment
    enroll3 =           0x25,// 25 Enroll3  Make 3rd template for an enrollment, merge three templates into one template, save merged template to the database
    IsPressfinger =     0x26,// 26 IsPressFinger Check if a finger is placed on the sensor
    Deleteid =          0x40,// 40 DeleteID Delete the fingerprint with the specified ID
    DeleteAll =         0x41,// 41 DeleteAll Delete all fingerprints from the database
    Verify  =           0x50,// 50 Verify 1:1 Verification of the capture fingerprint image with the specified ID
    Identify  =         0x51,// 51 Identify 1:N Identification of the capture fingerprint image with the database
    VerifyTemplate=     0x52,// 52 VerifyTemplate 1:1 Verification of a fingerprint template with the specified ID
    IdentifyTemplate =  0x53,// 53 IdentifyTemplate 1:N Identification of a fingerprint template with the database
    CaptureFinger =     0x60,// 60 CaptureFinger Capture a fingerprint image(256x256) from the sensor
    MakeTemplate =      0x61,// 61 MakeTemplate Make template for transmission
    GetImage =          0x62,// 62 GetImage Download the captured fingerprint image(256x256)
    GetRawImage =       0x63,// 63 GetRawImage Capture & Download raw fingerprint image(320x240)
    GetTemplate =       0x70,// 70 GetTemplate Download the template of the specified ID
    SetTemplate =       0x71,// 71 SetTemplate Upload the template of the specified ID
    GetDatabaseStart =  0x72,// 72 GetDatabaseStart Start database download, obsolete
    GetDatabaseEnd =    0x73,// 73 GetDatabaseEnd End database download, obsolete
    SetSecurityLevel =  0xf0,
    GetSecurityLevel =  0xf1,
    Identifty_Template2=0xf4,
    Ack =               0x30,// 30 Ack Acknowledge.
    Nack =              0x31,// 31 Nack Non-acknowledge.



};

enum error
{
    NO_ERROR =              0x1000,//ACTION SUCCESS
    NACK_TIMEOUT =          0x1001,// NACK_TIMEOUT 0x1001 Obsolete, capture timeout
    NACK_INVALID_BAUDRATE = 0x1002,// NACK_INVALID_BAUDRATE 0x1002 Obsolete, Invalid serial baud rate
    NACK_INVALID_POS =      0x1003,// NACK_INVALID_POS 0x1003 The specified ID is not between 0~1000
    NACK_IS_NOT_USED =      0x1004,// NACK_IS_NOT_USED 0x1004 The specified ID is not used
    NACK_IS_ALREADY_USED =  0x1005,// NACK_IS_ALREADY_USED 0x1005 The specified ID is already used
    NACK_COMM_ERR =         0x1006,// NACK_COMM_ERR 0x1006 Communication Error
    NACK_VERIFY_FAILED =    0x1007,// NACK_VERIFY_FAILED 0x1007 1:1 Verification Failure
    NACK_IDENTIFY_FAILED =  0x1008,// NACK_IDENTIFY_FAILED 0x1008 1:N Identification Failure
    NACK_DB_IS_FULL =       0x1009,// NACint ChangeBaudRate(int baud)K_DB_IS_FULL 0x1009 The database is full
    NACK_DB_IS_EMPTY =      0x100A,// NACK_DB_IS_EMPTY 0x100A The database is empty
    NACK_TURN_ERR =         0x100B, // NACK_TURN_ERR 0x100B Obsolete, Invalid order of the enrollment (The order was not as: EnrollStart -> Enroll1 -> Enroll2 -> Enroll3)
    NACK_BAD_FINGER =       0x100C, // NACK_BAD_FINGER 0x100C Too bad fingerprint
    NACK_ENROLL_FAILED =    0x100D,// NACK_ENROLL_FAILED 0x100D Enrollment Failure
    NACK_IS_NOT_SUPPORTED = 0x100E, // NACK_IS_NOT_SUPPORTED 0x100E The specified command is not supported
    NACK_DEV_ERR =          0x100F, // NACK_DEV_ERR 0x100F Device Error, especially if Crypto-Chip is trouble
    NACK_CAPTURE_CANCELED = 0x1010, // NACK_CAPTURE_CANCELED 0x1010 Obsolete, The capturing is canceled
    NACK_INVALID_PARAM =    0x1011,// NACK_INVALID_PARAM 0x1011 Invalid parameter
    NACK_FINGER_IS_NOT_PRESSED = 0x1012,// NACK_FINGER_IS_NOT_PRESSED 0x1012 Finger is not pressed,
    NACK_UNEXPECTED_ERROR = 0x1013 // NACK_UNEXPECTED ERROR general error happen

};

/*------------------------------------------------------------------ */
U8 commandPacket[12] = {COMMAND_START_CODE_1,COMMAND_START_CODE_2 ,COMMAND_DEVICE_ID_1,COMMAND_DEVICE_ID_2}; 
U8 responsePacket[12] = {0};
U8 ParameterBytes[4]={0};

/*-------------------------------------------------------------------*/
U8 GetHighByte(U16 w);
U8 GetLowByte(U16 w);
U16 _CalculateChecksum(U8* ptr , U32 length);
U16 CalculateChecksum(U8* buffer, U32 length);
void int_to_parameter(U32 );
U32 IntFromParameter(U8* );
U32  handle_error(U8 hige, U8 low);
U32 handleResponce(U8 [] ,U32 );
U32 ChangeBaudRate(U32 baud);
void clearBuffersData(U8* , U8* , U8*);

/*fps commands implementataion*/

U32 command_open();
U32 ChangeBaudRate(U32 baud);
U32 SetLED(U32 val);
U32 GetEnrollCount();
U32 Checkenrolled( U32 id);
U32 Enrollstart(U32 id);
U32 Enroll1();
U32 Enroll2();
U32 Enroll3();
U32 DeleteId(U32 id);
U32 IsPressFinger();
#endif
