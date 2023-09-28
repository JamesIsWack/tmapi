//****************************************************************************
//
// Copyright SN Systems Ltd 2011
//
// Description: Menu.c demonstrates a simple menu example.  As well as adding 
//  menus, a menu is also removed from the list originally.  When each menu item
//  is selected it is also removed from the list.
//
//****************************************************************************


int main(int argc, char ** argv)
{
	sn_uint32 i;
	sn_uint32 uItemId1;
	sn_uint32 uItemId2;
	sn_uint32 uItemId3;
	sn_notify Notify;

	AddMenuItem("Self", &uItemId1, "Self");
	AddMenuItem("Item Delete", &uItemId3, "ScriptView");
	AddMenuItem("Scripts", &uItemId2, "ScriptView");
	RemoveMenuItem(uItemId3);
	AddMenuItem("Autos", &uItemId3, "AutosView");

	RequestNotification(NT_CUSTMENU);
	while (1)
	{
		GetNotification(NT_CUSTMENU, &Notify);
		if ((sn_uint32)Notify.pParam1 == uItemId1)
			printf("Item 1");
		else if ((sn_uint32)Notify.pParam1 == uItemId2)
            printf("Item 2");
		else if ((sn_uint32)Notify.pParam1 == uItemId3)
			printf("Item 3");

		printf(" %i\n", (sn_uint32)Notify.pParam1);

		RemoveMenuItem((sn_uint32)Notify.pParam1);
	}
	return 0;
}
