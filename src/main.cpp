#include <ESP8266WiFi.h>
 
const char* ssid = "*****";				// <----------- input your SSID
const char* password = "*****";			// <----------- input your Password
int ModbusTCP_port = 502;
 
//////// Required for Modbus TCP / IP /// 
#define maxInputRegister 20
#define maxHoldingRegister 20
 
#define MB_FC_NONE 0
#define MB_FC_READ_REGISTERS 3 				//implemented
#define MB_FC_WRITE_REGISTER 6 				//implemented
#define MB_FC_WRITE_MULTIPLE_REGISTERS 16 	//implemented


// MODBUS Error Codes
#define MB_EC_NONE 0
#define MB_EC_ILLEGAL_FUNCTION 1
#define MB_EC_ILLEGAL_DATA_ADDRESS 2
#define MB_EC_ILLEGAL_DATA_VALUE 3
#define MB_EC_SLAVE_DEVICE_FAILURE 4

// MODBUS MBAP offsets
#define MB_TCP_TID 0
#define MB_TCP_PID 2
#define MB_TCP_LEN 4
#define MB_TCP_UID 6
#define MB_TCP_FUNC 7
#define MB_TCP_REGISTER_START 8
#define MB_TCP_REGISTER_NUMBER 10

#define LED_BUILTIN1 2
#define LED_BUILTIN2 16

byte ByteArray[260];

unsigned int MBHoldingRegister[maxHoldingRegister];
 
//////////////////////////////////////////////////////////////////////////
 
WiFiServer MBServer(ModbusTCP_port);
 
void setup() {
    pinMode(LED_BUILTIN1, OUTPUT);
    pinMode(LED_BUILTIN2, OUTPUT);    	
	
    Serial.begin(9600);
	
	delay(100) ;
	
	WiFi.begin(ssid, password);
	
	delay(100) ;
	
	Serial.println(".");
	
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	
	MBServer.begin();
	
	Serial.println("Connected ");
	Serial.print("ESP8266 Slave Modbus TCP/IP ");
	Serial.print(WiFi.localIP());
	Serial.print(":");
	Serial.println(String(ModbusTCP_port));
	Serial.println("Modbus TCP/IP Online");
}
 
 
void loop() {
	// Check if a client has connected // Modbus TCP/IP
	WiFiClient client = MBServer.available();
	if (!client) {
		return;
	}
	
	boolean flagClientConnected = 0;
	byte byteFN = MB_FC_NONE;
 
	int Start;
	int WordDataLength;
	int ByteDataLength;
	int MessageLength;
 
	// Modbus TCP/IP
	while (client.connected()){
		if(client.available()){
			flagClientConnected = 1;
			int i = 0;
			while(client.available()){
				ByteArray[i] = client.read();
				i++;
			}
		client.flush();

		///// code here
	
	/////// Holding Register [0] A [9] = 10 Holding Registers Writing
 	MBHoldingRegister[0] = random(0,12);
	MBHoldingRegister[1] = random(0,12);
	MBHoldingRegister[2] = random(0,12);
	MBHoldingRegister[3] = random(0,12);
	MBHoldingRegister[4] = random(0,12);
	MBHoldingRegister[5] = random(0,12);
	MBHoldingRegister[6] = random(0,12);
	MBHoldingRegister[7] = random(0,12);
	MBHoldingRegister[8] = random(0,12);
	MBHoldingRegister[9] = random(0,12);
 
 
	///// Holding Register [10] A [19] = 10 Holding Registers Reading
//	int TempVal[10];
 
//	TempVal[0] = MBHoldingRegister[10];
//	TempVal[1] = MBHoldingRegister[11];
//	TempVal[2] = MBHoldingRegister[12];
//	TempVal[3] = MBHoldingRegister[13];
//	TempVal[4] = MBHoldingRegister[14];
//	TempVal[5] = MBHoldingRegister[15];
//	TempVal[6] = MBHoldingRegister[16];
//	TempVal[7] = MBHoldingRegister[17];
//	TempVal[8] = MBHoldingRegister[18];
//	TempVal[9] = MBHoldingRegister[19];
 
	/// Enable Output 14
//	digitalWrite(14, MBHoldingRegister[14] );
 
 
	//// debug
 
//	for (int i = 0; i < 10; i++) {
//		Serial.print("[");
//		Serial.print(i);
//		Serial.print("] ");
//		Serial.print(TempVal[i]);
//	}
//	Serial.println("");
    switch (MBHoldingRegister[10]) {
        case 101:
            // statements
            digitalWrite(LED_BUILTIN1, HIGH);
            digitalWrite(LED_BUILTIN2, LOW);
            Serial.println("robot picked part up");
            break;
        case 202:
            // statements
            digitalWrite(LED_BUILTIN1, LOW);
            digitalWrite(LED_BUILTIN2, HIGH);            
            Serial.println("robot put part down");
        	break;
        default:
            digitalWrite(LED_BUILTIN1, LOW);
            digitalWrite(LED_BUILTIN2, LOW);
            // statements
            break;
    }
	//// end code
 
 
	//// rutine Modbus TCP
	byteFN = ByteArray[MB_TCP_FUNC];
	Start = word(ByteArray[MB_TCP_REGISTER_START],ByteArray[MB_TCP_REGISTER_START+1]);
	WordDataLength = word(ByteArray[MB_TCP_REGISTER_NUMBER],ByteArray[MB_TCP_REGISTER_NUMBER+1]);
	}
 
 
	// Handle request

	switch(byteFN) {
		case MB_FC_NONE:
		break;
 
	case MB_FC_READ_REGISTERS: // 03 Read Holding Registers
		ByteDataLength = WordDataLength * 2;
		ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
		ByteArray[8] = ByteDataLength; //Number of bytes after this one (or number of bytes of data).
		for(int i = 0; i < WordDataLength; i++){
			ByteArray[ 9 + i * 2] = highByte(MBHoldingRegister[Start + i]);
			ByteArray[10 + i * 2] = lowByte(MBHoldingRegister[Start + i]);
		}
		MessageLength = ByteDataLength + 9;
		client.write((const uint8_t *)ByteArray,MessageLength);
 
		byteFN = MB_FC_NONE;
 
		break;

	case MB_FC_WRITE_REGISTER: // 06 Write Holding Register
		MBHoldingRegister[Start] = word(ByteArray[MB_TCP_REGISTER_NUMBER],ByteArray[MB_TCP_REGISTER_NUMBER+1]);
		ByteArray[5] = 6; //Number of bytes after this one.
		MessageLength = 12;
		client.write((const uint8_t *)ByteArray,MessageLength);
		byteFN = MB_FC_NONE;
		break;
 
	case MB_FC_WRITE_MULTIPLE_REGISTERS: //16 Write Holding Registers
		ByteDataLength = WordDataLength * 2;
		ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
		for(int i = 0; i < WordDataLength; i++){
			MBHoldingRegister[Start + i] = word(ByteArray[ 13 + i * 2],ByteArray[14 + i * 2]);
		}
		MessageLength = 12;
		client.write((const uint8_t *)ByteArray,MessageLength); 
		byteFN = MB_FC_NONE;
 
		break;
		}
	}
}