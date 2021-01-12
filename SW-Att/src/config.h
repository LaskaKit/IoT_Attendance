char ssid[] = "yourWiFiSSIDname";   // your network SSID (name)
char pass[] = "yourWiFipassword";   // your network password

String GAS_ID = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";  //getactivespreadsheetID

byte target_tag[4][4]={
    {0xC9, 0x4F, 0x02, 0xB9},   // TAG_ID [0] C9 4F 02 B9 "Helena"
    {0x86, 0x2D, 0x14, 0xCA},   // TAG_ID [1] 86 2D 14 CA "Martin"
    {0x76, 0x16, 0xB0, 0xCA},   // TAG_ID [2] 76 16 B0 CA "Marek"
    {0x05, 0xB6, 0xBE, 0x02},   // TAG_ID [3] 05 B6 BE 02 "Kosta"
    };

char names[4][8]={
    { "Helena" },
    { "Martin" },
    { "Marek" },
    { "Kosta" },
 };