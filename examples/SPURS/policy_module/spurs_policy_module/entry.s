					.text		
					.global	cellSpursModuleEntry
					.type	cellSpursModuleEntry, @function
cellSpursModuleEntry:
								il      $5,0
								hbra    GOTO_MAIN, pm_main
								ila     $2, 256*1024 - 16 * 1
								lnop
								ila     $1, 256*1024 - 16 * 3
								stqa    $5,256*1024 - 16 * 1
								stqa    $5,256*1024 - 16 * 2
								stqa    $2,256*1024 - 16 * 3
GOTO_MAIN:			brsl    $0, pm_main
								lnop
