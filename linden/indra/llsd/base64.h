
int base64_encode_len(int inlength);
int base64_encode_binary(char *pEncoded, const unsigned char *pIn, int inLength);
int base64_decode_len(const char * pIn);
int base64_decode_binary(unsigned char *pDecoded, const char *pIn);
