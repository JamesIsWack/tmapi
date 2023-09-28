//****************************************************************************
//
// Copyright SN Systems Ltd 2011
//
// Description: BrokenMenu.c is the example used in the documentation to show that
//  it is important to break associations between menu item IDs and the code when
//  adding and removing them.  In this example, the ID that was first assigned to
//  "To be deleted" has been re-assigned to "Fails to work".  This is because the
//  IDs are recycled (a maximum of 20 are available).  If a script will be repeatedly
//  adding and removing items from the menu, it may need to keep track of which
//  menu calls are expected.
//
//****************************************************************************

int main (int argc, char ** argv)
{
	sn_uint32 item1;
	sn_uint32 item2;
	sn_uint32 item3;
	sn_notify Notify;

	AddMenuItem("To be deleted", &item1, "Self");
	AddMenuItem("Works fine", &item2, "Self");
	RemoveMenuItem(item1);				//	Delete first menu item
	AddMenuItem("Fails to work", &item3, "Self");
	RequestNotification(NT_CUSTMENU);

	while(1)
	{
		GetNotification(NT_CUSTMENU, &Notify);
		if ((sn_uint32)Notify.pParam1 == item1)
			printf("Wasn't I deleted?\n");
		else if ((sn_uint32)Notify.pParam1 == item2)
			printf("I work fine\n");
		else if ((sn_uint32)Notify.pParam1 == item3)
			printf("I'm never called!");
	}
	return 0;
}
