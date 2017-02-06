#include <IPv6Stack.h>
#include <XBeeMACLayer.h>
#include <cstring>

#define IS_INTERMEDIATE_ROUTER  (UIP_CONF_ROUTER && 0)
#define IS_BORDER_ROUTER (UIP_CONF_ROUTER && !IS_INTERMEDIATE_ROUTER)
#define UDP_PORT 8765
//#define SEND_INTERVAL  (10 * 1000)// 10 seconds
#define UDP_MAX_DATA_LEN 100

const int LEDPIN40 = 40;
const int LEDPIN41 = 41;

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

void doAction(char *name, char *action) {
    if (strcmp(name, "led40") == 0) {
        if (strcmp(action, "on") == 0) {
            digitalWrite(LEDPIN40, HIGH);
            Serial.println("Led 40 on");
            delay(50);
        } else {
            digitalWrite(LEDPIN40, LOW);
            Serial.println("Led 40 off");
            delay(50);
        }
    }

    if (strcmp(name, "led41") == 0) {
        if (strcmp(action, "on") == 0) {
            digitalWrite(LEDPIN41, HIGH);
            Serial.println("Led 41 on");
            delay(50);
        } else {
            digitalWrite(LEDPIN41, LOW);
            Serial.println("Led 41 off");
            delay(50);
        }
    }
}

void udp_callback(char *data, int datalen, int sender_port, IPv6Address &sender_addr) {
    char *p;
    char **b;
    int bi = 0;

    Serial.println(mem());
    delay(100);
    //Show received dada
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

    int index = 0;
    char *key;
    char *value;
    char *l;
    while (index < datalen) {
        p = strtok(&data[index], "&");
        index += strlen(p) + 1;
        if (p != NULL) {
            char *i = strchr(p, '=');
            key = strtok(p, "=");
            value = i + 1;
            doAction(key, value);
        }
    }
    Serial.println("Sending response..");
    Serial.println();
//    addr_dest = sender_addr;
//    dest_port = sender_port;
//    IPv6Stack::udpSend(sender_addr, sender_port, "OK", 2);
    
    sender_addr.print();
    delay(50);
    Serial.println("Send OK!");
}

void setup() {
    pinMode(LEDPIN40, OUTPUT);
    pinMode(LEDPIN41, OUTPUT);

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
    if (IPv6Stack::receivePacket()) {
        if (IPv6Stack::udpDataAvailable()) {
            udp_data_length = IPv6Stack::getUdpDataLength();
            IPv6Stack::getUdpData(udp_data);
            IPv6Stack::getUdpSenderIpAddress(sender_address);
            udp_callback(udp_data, udp_data_length, IPv6Stack::getUdpSenderPort(), sender_address);
        }
    }
    delay(100);
}
