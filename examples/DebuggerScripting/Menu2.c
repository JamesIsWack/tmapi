//****************************************************************************
//
// Copyright SN Systems Ltd 2011
//
// Description: Menu2.c is an example of using menus without interfering with the
//  main program flow (in this case, repeatedly printing a number to the screen).
//  The main difference here is that a PeekNotification call is made instead of 
//  a GetNotification call.
//
//****************************************************************************


int main (int argc, char ** argv)
{
	sn_uint32 uNumberToPrint = 0;
	sn_uint32 uPrint1;
	sn_uint32 uPrint2;
	sn_uint32 uPrint3;
	sn_notify Notify;

	AddMenuItem("Print 1", &uPrint1, "Self");
	AddMenuItem("Print 2", &uPrint2, "Self");
	AddMenuItem("Print 3", &uPrint3, "Self");

	RequestNotification(NT_CUSTMENU);

	while (1)
	{
		PeekNotification(NT_CUSTMENU, &Notify);		//  Use peek notification so blocking does not occur
		if ((sn_uint32)Notify.pParam1 == uPrint1)
			uNumberToPrint=1;
		else if ((sn_uint32)Notify.pParam1 == uPrint2)
			uNumberToPrint=2;
		else if ((sn_uint32)Notify.pParam1 == uPrint3)
			uNumberToPrint=3;

		printf("%i\n", uNumberToPrint);
	}
	return 0;
}

