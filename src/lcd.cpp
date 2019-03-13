/*
	String strT = "T: ";
	String He;
	if (true)    //heat
		He = "ON";
	else
		He = "OFF";
	strT = strT + TempSetpoint + "C "+ int(steinhart) + "C " + He;
	char charT[strT.length()+1];
	strT.toCharArray(charT, strT.length()+);
	u8g.drawStr(0, 40, charT ); //charT
 */
/*void strFromInt(char* str, int v) 
{
	char* p = str;

	*p++ = (v / 100) + '0'; // Convert 100's to ASCII digit (0-9) and put in string
	v = v % 100;    // Return remainder after dividing by 100
	*p++ = (v / 10) + '0';  // Convert 10's to ASCII digit (0-9) and put in string
	v = v % 10;
	*p++ = v + '0';   // Convert 1's to ASCII digit (0-9) and put in string
	*p++ = '\0';    // Terminating null

	// Remove leading zeros
	for (p = str; (p < str + 2) && (*p == '0'); p++)
		*p = ' '; // Write a space character
}

*/
