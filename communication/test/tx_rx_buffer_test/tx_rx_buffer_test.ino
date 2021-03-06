
//struct for transmitting and receiving data:
struct controlData_t {
  float val1;
  float val2;
  float val3;
};

// struct for buffering and accessing data:
struct controlData_full {
  uint8_t cmd;
  float val1;
  float val2;
  float val3;
};

uint8_t cmd;
char rxcmd;
void printstruct(controlData_full data) {
  Serial.println("Data:  ");
  Serial.print("CMD:  "); Serial.print(data.cmd);
  Serial.print(", val1:  "); Serial.print(data.val1);
  Serial.print(", val2:  "); Serial.print(data.val2);
  Serial.print(", val3:  "); Serial.println(data.val3);
}

union ZIGBEE_Packet_t {
  controlData_t packet;
  uint8_t ZBPacket[sizeof(controlData_t)];
};
ZIGBEE_Packet_t txdata;
ZIGBEE_Packet_t rxdata;

#define PACKET_SIZE sizeof(txdata.ZBPacket)

// Buffer setup
#include <CircularBuffer.h>
#define BUFFER_SIZE 20
controlData_full tempdata;
controlData_full rxdatafull;
CircularBuffer<controlData_full, BUFFER_SIZE> rxbuffer;


// Transmit function, takes command argument which is used for
void transmit(uint8_t cmd) {
  Serial1.write(cmd);
  for (int k = 0; k < PACKET_SIZE; k++) {
    Serial1.write(txdata.ZBPacket[k]);
  }
}


void receive() {
  if (Serial1.available() > PACKET_SIZE ) {
    uint8_t cmdtemp = Serial1.read();
    if (cmdtemp == '<') {
      // rxcmd= Serial.read();
      for (int k = 0; k < PACKET_SIZE; k++) {
        rxdata.ZBPacket[k] = Serial1.read();
      }
      rxdatafull.cmd = cmdtemp;
      rxdatafull.val1 = rxdata.packet.val1;
      rxdatafull.val2 = rxdata.packet.val2;
      rxdatafull.val3 = rxdata.packet.val3;
      rxbuffer.push(rxdatafull);
      cmdtemp = 0;
    }
  }
}
// get retrieved from buffer value using following command:
void get_rx_data() {
  tempdata = rxbuffer.shift(); // get packet from buffer.
}


//XBEE setup:
void xbee_setup() {
  Serial1.begin(115200);
  int k = sizeof(txdata.ZBPacket) + 1;
  delay(15000);
  Serial.print("Size of packet"); Serial.println(k);
  Serial.println("XBEE online");
}


// Sample period time setting.
unsigned long t0 = 0;
const unsigned long ts = 10; // time for each transmisssion:
int count = 0;
unsigned long tstart = 0; //test vars time keeping
unsigned long tstop = 0; //test vars time keeping


int N = 4; // used for checking the number of elements in the buffer.
void setup() {

  Serial.begin(115200);
  Serial1.begin(115200);
  int k = sizeof(txdata.ZBPacket) + 1;
  delay(3000);
  Serial.print("Size of packet"); Serial.println(k);
  //pinMode(1, INPUT_PULLUP);
 // pinMode(LED_BUILTIN, OUTPUT);
 // pinMode(3, OUTPUT); // transmission begin pin goes high when a transmission starts.
 // pinMode(4, OUTPUT); // when a packet is received pin goes high.
  //digitalWrite(LED_BUILTIN, LOW);
  //t0 = micros();
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
}


int timer_var = 0, time_now = 0, time_last = 0;

void loop() {
  // set tx packet:

  //  Serial.println("Setting values for rx packet:  ");
  txdata.packet.val1 = 313.3445;
  txdata.packet.val2 = 44.33;
  txdata.packet.val3 = 14.312;
  cmd = '<';
  //   txdata.packet.cmd = 'C';
transmit('<');
delay(1000);  //printstruct(txdata.packet);
  
  
  
  /*time_now = micros();
  // transmission:
  transmit('C');
  time_last = micros();
  timer_var = time_last - time_now;
  Serial.println(timer_var);
  delay(1000);*/
  
  /*if (digitalRead(1) == LOW) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("transmit 1 packet:");
    delay(1000);
    for (int i = 0; i < N; i++) {
      transmit('<');
    }
    //while ((t0 + ts ) > micros()) {}
    // t0 = micros();
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
  }*/
  // receive data;
  receive();
  //check buffer:
  if (rxbuffer.size() > N ) {
    Serial.println("Printing from buffer:");
    while (rxbuffer.isEmpty() != true) { // Print all elements in the buffer.
      //Serial.print("Items in buffer: "); Serial.println(rxbuffer.size());
      get_rx_data();
      printstruct(tempdata);
      delay(1000);
      Serial.print("Items in buffer: "); Serial.println(rxbuffer.size());
    }
  }
}
