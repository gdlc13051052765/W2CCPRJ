#include "includes.h"
#include "st_flash.h"
 
int main(void)
{
	systerm_init();

	while (1)
	{
		app_dispatch();
	}
}

