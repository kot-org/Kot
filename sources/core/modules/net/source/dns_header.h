#define DNS_PORT 53

#define QR_QUERY 0
#define QR_RESPONSE 1

#define OPCODE_QUERY 0
#define OPCODE_IQUERY 1
#define OPCODE_STATUS 2

#define AA_NONAUTHORITY 0
#define AA_AUTHORITY 1

struct dns_header{
	uint16_t id;

    uint8_t rd:1; 
    uint8_t tc:1;
    uint8_t aa:1;
    uint8_t opcode:4;
    uint8_t qr:1;
 
    uint8_t rcode:4;
    uint8_t cd:1;
    uint8_t ad:1;
    uint8_t z:1;
    uint8_t ra:1;

	uint16_t qdcount;
	uint16_t ancount;
	uint16_t nscount;
	uint16_t arcount;
};


struct dns_packet{
	struct dns_header header;
	uint8_t data[];
};