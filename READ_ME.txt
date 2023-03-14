Welcome everyone: 

In this guide you should find a quick guide for settings and pin out referneces ^_^ 


~ Thank you for your support, from Steven ( Developer ) 


Open source software: 
-------------------------------------------------------------------------------------------------------------------------------
---    PIN OUT ( DEFAULT ) - limited changability
-------------------------------------------------------------------------------------------------------------------------------



light_pin                        	=   8;       // Arduino out_pin (lights)  
humidifier_pin               	= 10;       // Arduino out_pin (Accessory/humidifier)
ozone            		= 11;       // Arduino out_pin (ozone genartor/air pump- water purification)
const short int   door_pin                =   6;       // Arduino inout pin for door ajar sensor

valve_1pin              = 3;                  // Air system valves ( Should use NORMALLY CLOSED VALVES, 12v )
valve_2pin              = 4;        
valve_3pin              = 5;     

DHT22 ( default sensor type )  	= digital pin 2;


-------------------------------------------------------------------------------------------------------------------------------
---    Settings and comments:
-------------------------------------------------------------------------------------------------------------------------------


NOTE:   Incorrect settings may damage hardware, its important to read everything. 
reminder:
> No negative numbers , bool must be a true/ false, no decimal numbers ( this will surely break something or stop the code from opperating )
> Dont change the pin out modes
> Dont chage the pins if not read through the code yet. 
> Ozone on time bust be a smaller whole number than the humidifier on time ( untested outcome otherwise )



-------------------------------------------------------------------------------------------------------------------------------
Q/A -    Clone the Valve port or any port  ??  Multi sets of 3 FC chambers    ( 3 FC'S / 1 Air system ) X  Number of clones on ports 
-------------------------------------------------------------------------------------------------------------------------------

A:  

You can not just shove a lot of relay boards into that port. 

You must connect 3 relays ( 1 for each output pin ) to that port. 
You ALSO will have to clone all pretty much all ports if you have multi air systems and water tanks...
	Why ? 

The arduino output pins can only output 20 MA of 5v current... 
	If you add to many boards on the output, you will burn the board out.... 
	This could result in serious injury or damage to hardware such as a fire depending on how you build the board.
	


step 2: 

	Connect the other relay boards to the these relay boards to switch them so the current goes into the relay and not the board.
	Relay board give you 12 Amps ~ 15 Amps of expansion room on the port.

	This could be several dozen relay boards that switch air systems for banks of isolated FCs 




-------------------------------------------------------------------------------------------------------------------------------
Q/A -    How many ways can a system be created using this software?
-------------------------------------------------------------------------------------------------------------------------------


	Potentially there is no limit to what you can build. 
	There is only the prototype board layouts avaible for anyone who cannot desgine a board.
	This means you may have to build a different board / if you wanted to or needed too for specific functionality

	In gineral there is 4 base configurations of grow : 

	> Small scale steath GB enclosure:
		this allows for:
			a door and sesnor ( turns on the lights when its night time when the door is open )
			fog system 
			air system 
			water and air cleaning system 
			lighting system 
			temp sensor and read out 
			
			* usually no valves 
			* single FC configuration 

	> mid scale multi isololation FCs 
		this allows for: 
			
			Single air system / fog / ozon to supply multi FCs using valves 
				
			* door system may nt matter unless in an elcosure 
			* temp sensor is no longer useful unless you are in an enclosure or room 
			
			Note: Still can use door sensor as light system toggle 

	> large scale
			Unlimited FC configuration
				
			** MUST ! Clone ports first
			* door system may nt matter unless in an elcosure 
			* temp sensor is no longer useful unless you are in an enclosure or room 
			
			Note: Still can use door sensor as light system toggle 

	> tents

			All settings and set ups apply to all size and variation of tent systems. 
			
		
















