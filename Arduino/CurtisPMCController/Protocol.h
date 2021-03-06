#define PACKET_START_CHAR '!'
#define MSGID_CMD_UPDATE 'c'
#define CMD_PWM_INDEX 2
//[!,1][c,1][pwm value,4][checksum, 2][Termination, 2]

#define CMD_CHECKSUM_INDEX 6
#define CMD_MESSAGE_LENGTH 10

#define CHECKSUM_LENGTH 2
#define TERMINATOR_LENGTH 2

/*
 * Encoder/Decode Command packets
 */

static unsigned char decodeUint8( char *checksum ) {
  unsigned char first_digit = checksum[0] <= '9' ? checksum[0] - '0' : ((checksum[0] - 'A') + 10);
  unsigned char second_digit = checksum[1] <= '9' ? checksum[1] - '0' : ((checksum[1] - 'A') + 10);
  unsigned char decoded_checksum = (first_digit * 16) + second_digit;
  return decoded_checksum;  
}  

static uint16_t decodeProtocolUint16( char *uint16_string ) {
  uint16_t decoded_uint16 = 0;
  unsigned int shift_left = 12;
  for(int i = 0; i < 4; i++ ) {
    unsigned char digit = uint16_string[i] <= '9' ? uint16_string[i] - '0' : ((uint16_string[i] - 'A') + 10);
    decoded_uint16 += (((uint16_t)digit) << shift_left);
    shift_left -= 4;
  }
  return decoded_uint16;  
}

static void encodeProtocolUint16( uint16_t value, char* buff ) {
  sprintf(&buff[0],"%04X", value );
}

static bool verifyChecksum( char *buffer, int content_length ) {
  // Calculate Checksum
  unsigned char checksum = 0;
  for(int i = 0; i < content_length; i++) {
    checksum += buffer[i];
  }

  // Decode Checksum
  unsigned char decoded_checksum = decodeUint8(&buffer[content_length]);

  return ( checksum == decoded_checksum );
}


static void encodeTermination( char *buffer, int total_length, int content_length ) {
  if((total_length >= (CHECKSUM_LENGTH + TERMINATOR_LENGTH)) && (total_length >= content_length + (CHECKSUM_LENGTH + TERMINATOR_LENGTH))) {
    // Checksum 
    unsigned char checksum = 0;
    for(int i = 0; i < content_length; i++) {
      checksum += buffer[i];
    }
    // convert checksum to two ascii bytes
    sprintf(&buffer[content_length], "%02X", checksum);
    // Message Terminator
    sprintf(&buffer[content_length + CHECKSUM_LENGTH], "%s","\r\n");
  }
}

static int encodeCommand(char* protocol_buffer, uint16_t pwm) {
  // Header
  protocol_buffer[0] = PACKET_START_CHAR;
  protocol_buffer[1] = MSGID_CMD_UPDATE;

  // Data
  encodeProtocolUint16(pwm, &protocol_buffer[CMD_PWM_INDEX]);

  // Footer
  encodeTermination( protocol_buffer, CMD_MESSAGE_LENGTH, CMD_MESSAGE_LENGTH - 4 );

  return CMD_MESSAGE_LENGTH;
}

static int decodeCommand(char *buffer, int length, uint16_t& pwm) {
  if(length < CMD_MESSAGE_LENGTH) return 0;
  if(( buffer[0] == '!') && (buffer[1] == MSGID_CMD_UPDATE)) {
    if(!verifyChecksum(buffer, CMD_CHECKSUM_INDEX)) return 0;

    pwm = decodeProtocolUint16(&buffer[CMD_PWM_INDEX]);

    return CMD_MESSAGE_LENGTH;
  }
  return 0;
}
