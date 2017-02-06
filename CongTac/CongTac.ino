#include <IPv6Stack.h>
#include <XBeeMACLayer.h>
#include <cstring>

#define IS_INTERMEDIATE_ROUTER  (UIP_CONF_ROUTER && 0)
#define IS_BORDER_ROUTER (UIP_CONF_ROUTER && !IS_INTERMEDIATE_ROUTER)
#define UDP_PORT 8765
//#define SEND_INTERVAL  (10 * 1000)// 10 seconds
#define UDP_MAX_DATA_LEN 100

const int BUTTONPIN40 = 40;
const int BUTTONPIN41 = 41;
int ledStatePin40 = 0;
int ledStatePin41 = 0;
int ledSendPin40 = 0;
int ledSendPin41 = 0;
int buttonStatePin40;
int buttonStatePin41;
int isAction = 0;

char *ledStatus[2] = {"on", "off"};

int mem() {
    uint8_t * heapptr, * stackptr;
    stackptr = (uint8_t *) malloc(4);
    heapptr = stackptr;
    free(stackptr);
    stackptr = (uint8_t *) (SP);
    return stackptr - heapptr;
}

XBeeMACLayer macLayer;

//We use a buffer in order to put the data that we will send
char udp_send[UDP_MAX_DATA_LEN];

//This will be the destination IP address
IPv6Address addr_dest;

char udp_data[UDP_MAX_DATA_LEN];
int udp_data_length = 0;
IPv6Address sender_address;
uint16_t dest_port = UDP_PORT;

void udp_callback(char *data, int datalen, int sender_port, IPv6Address &sender_addr) {
    Serial.println(mem());
    delay(100);
    data[datalen] = 0;
    Serial.println();
    Serial.println();
    Serial.print("Data received from ip: ");
    sender_addr.print();
    Serial.print(" port: ");
    Serial.print(sender_port);
    Serial.print(", data: ");
    Serial.print(data);
    Serial.println();
}

void sendData(char *data) {
    IPv6Stack::udpSend(addr_dest, dest_port, data, strlen(data));
}

void doAction() {
    if (buttonStatePin40 == HIGH) {
        if (ledStatePin40 == 1) {
            ledSendPin40 = 0;
        } else {
            ledSendPin40 = 1;
        }
    }
    if (buttonStatePin41 == HIGH) {
        if (ledStatePin41 == 1) {
            ledSendPin41 = 0;
        } else {
            ledSendPin41 = 1;
        }
    }
    sendLedData();
}

void sendLedData() {
        char s[100] = "led40=";
        strcat(s, ledStatus[ledSendPin40]);
        strcat(s, "&");
        strcat(s, "led41=");
        strcat(s, ledStatus[ledSendPin41]);
        sendData(s);
        ledStatePin40 = ledSendPin40;
        ledStatePin41 = ledSendPin41;
}

void setup() {
    pinMode(BUTTONPIN40, INPUT);
    pinMode(BUTTONPIN41, INPUT);

    Serial.begin(9600);
    delay(1000);
    Serial.println();
    Serial.print("MEMORY LEFT:");
    Serial.println(mem());
    delay(100);

    // init network-device
    if (!IPv6Stack::initMacLayer(&macLayer)) {
        Serial.println("CANNOT INITIALIZE XBEE MODULE.. CANNOT CONTINUE");
        while (true) {
        };
    }

    //init IP Stack
    IPv6Stack::initIpStack();
    Serial.println("IPV6 INITIALIZED");
    delay(100);

    //init UDP
    IPv6Stack::initUdp(UDP_PORT);
    Serial.println("UDP INITIALIZED");
    delay(100);

    Serial.println("SETUP FINISHED!");
    delay(100);
}

void loop() {
    IPv6Stack::pollTimers();

    buttonStatePin40 = digitalRead(BUTTONPIN40);
    buttonStatePin41 = digitalRead(BUTTONPIN41);

    if (buttonStatePin40 == HIGH || buttonStatePin41 == HIGH) {
        isAction = 1;
        delay(300);

        while (digitalRead(BUTTONPIN40) == HIGH || digitalRead(BUTTONPIN41) == HIGH) {
            if (digitalRead(BUTTONPIN40) != buttonStatePin40 && buttonStatePin40 == LOW) {
                buttonStatePin40 = HIGH;
            }
            if (digitalRead(BUTTONPIN41) != buttonStatePin41 && buttonStatePin41 == LOW) {
                buttonStatePin41 = HIGH;
            }
        }
        doAction();
    }
    delay(100);
}
