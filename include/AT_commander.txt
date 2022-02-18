String readReply(uint32_t timeout);
boolean checkForExptReplyinRcvdString(String recvdString, const char* exptReply);
boolean waitResponse(const char* exptReply, uint32_t timeout);
String readReply(uint32_t timeout, const char* exptReply);
boolean waitResponse(const char* exptReply, uint32_t timeout, int quick) ;
boolean SendAT(String ATCommand, const char* exptReply, uint32_t timeout, int retry);