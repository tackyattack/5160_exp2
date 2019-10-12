#include "outputs.h"

void write_port_bit(uint8_t port_num, uint8_t bit_num, uint8_t bit_val)
{
	if(bit_val)
	{
		switch(port_num)
			{
				case PORT0:
					P0|=(1<<bit_num);
					break;
				case PORT1:
					P1|=(1<<bit_num);
					break;
				case PORT2:
					P2|=(1<<bit_num);
			   		break;
				case PORT3:
					P3|=(1<<bit_num);
			}
	}
else
	{
		switch(port_num)
			{
				case PORT0:
					P0&=~(1<<bit_num);
					break;
				case PORT1:
					P1&=~(1<<bit_num);
					break;
				case PORT2:
					P2&=~(1<<bit_num);
			   		break;
				case PORT3:
					P3&=~(1<<bit_num);
			}
	}

}

// void write_port_value(uint8_t port_num, uint8_t port_val)
// {
// 	switch(port_num)
// 		{
// 			case PORT0:
// 				P0 = port_val;
// 				break;
// 			case PORT1:
// 				P1 = port_val;
// 				break;
// 			case PORT2:
// 				P2 = port_val;
// 			   	break;
// 			case PORT3:
// 				P3 = port_val;
// 		}
// }
