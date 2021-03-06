//---------------------------------------------------------------------------
// SharkBoad SystemModule
//
// Top Level Design for the Xilinx Spartan 3-100E Device
//---------------------------------------------------------------------------

/*#
# SharkBoad
# Copyright (C) 2012 Bogotá, Colombia
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#*/
/*
#  _________.__                  __   __________                       .___
# /   _____/|  |__ _____ _______|  | _\______   \ _________ _______  __| _/
# \_____  \ |  |  \\__  \\_  __ \  |/ /|    |  _//  _ \__  \\_  __ \/ __ | 
# /        \|   Y  \/ __ \|  | \/    < |    |   (  <_> ) __ \|  | \/ /_/ | 
#/_______  /|___|  (____  /__|  |__|_ \|______  /\____(____  /__|  \____ | 
#        \/      \/     \/           \/       \/           \/           \/ 
#
*/
module system
#(
	parameter	clk_freq	= 50000000,
	parameter	uart_baud_rate	= 57600
) (
	input		clk50MHz,
	input		rst,
	input       uart_rx, 
	input       uart_tx,
	output		[2:0] leds,
	
	//! ============ L-SIDE ============  
	//! === Bank ===
	input 		io9,
	input 		io5,
	input 		io4,
	input 		io3,
	input 		io2,
	input 		io38,
	input 		io95,
	input 		io94,
	//! === Bank ===
	input 		io92,
	input 		io91,
	input 		io90,
	input 		i89,
	input 		i88,
	input 		io86,
	input 		io85,
	input 		io84,
	//! === Bank ===
	input 		io83,
	input 		io79,
	input 		io78,
	input 		io71,
	output 		io70,
	output 		io68,//tn1638_strobe
	inout 		io67,//tm1638_io
	output 		io66,//tm1638_clk
	
	
	//! ============ R-SIDE ============ 
	//! === Bank ===
	output 		io10,
	output 		io11,
	output 		io12,
	output 		io15,
	output 		io16,
	output 		io17,
	output 		io18,
	output 		io22,
	//! === Bank ===
	output 		io23,
	input 		i25,
	output 		io26,
	input 		i30,
	output 		io32,
	output 		io33,
	output 		io34,
	output 		io35,
	//! === Bank ===
	output 		io36,
	output 		io40,
	output 		io41,
	output 		io44,
	output 		io53,
	output 		io57,
	output 		io58,
	input 		io60
);
  localparam [6:0]
	S_1     = 7'b0000110,
	S_2     = 7'b1011011,
	S_3     = 7'b1001111,
	S_4     = 7'b1100110,
	S_5     = 7'b1101101,
	S_6     = 7'b1111101,
	S_7     = 7'b0000111,
	S_8     = 7'b1111111,
	S_BLK   = 7'b0000000;

//---------------------------------------------------------------------------
// General Purpose IO
//---------------------------------------------------------------------------

wire n_rst=~rst;

wire counter_tm1638_ovf;
counter	#(    .N(32),
              .M(10) 
   		)
	counter_unit0 
   (
    .clk(clk50MHz), .reset(n_rst),
    .max_tick(counter_tm1638_ovf),
    .q()
   );

wire counter_1hz_unit_ovf;
counter	#(    .N(32), // number of bits in counter
              .M(50000) // Remember for simulation 50000 = frec(counter_tm1638_ovf)=>1KHz, for implementation use 50000000 => 1 Hz 
   		)
	counter_1hz_unit 
   (
    .clk(clk50MHz), .reset(n_rst),
    .max_tick(counter_1hz_unit_ovf),
    .q()
   );
   
   
wire[7:0] counter_leds;
wire[31:0] counter_low_frec;
assign counter_leds = counter_low_frec[7:0];
counter	#(    .N(32), 
              .M(32'hFFFF_FFFF)
   		)
	ccounter_leds_unit 
   (
    .clk(counter_1hz_unit_ovf), .reset(n_rst),
    .max_tick(),
    .q(counter_low_frec)
   );
    
    
    wire tm1638_strobe;
    wire tm1638_clk;
    wire tm1638_data_io;
    assign io67 = tm1638_data_io;
    assign io68 = tm1638_strobe;
    assign io66 = tm1638_clk;
    
	wire [15:0]hx8352_data;
	wire hx8352_rs;
	wire hx8352_wr;
	wire hx8352_rd;
	wire hx8352_cs;
	wire hx8352_rst;
	wire [7:0] tm1638_keys;
	wire [7:0] debug_instruction_step;
	tm1638_keys_display_encoded
		tm1638_keys_display_encoded_unit0
		(
		.clk_5MHz(counter_tm1638_ovf),
		.n_rst(n_rst),
		.display_off(1'b0),
		.display_level(3'b100),
		.display_value({debug_instruction_step, hx8352_data}),
		.dots(8'h00),
		.leds_green({ hx8352_cs, hx8352_rst, hx8352_rs, hx8352_wr, hx8352_rd, counter_1hz_unit_ovf, counter_leds[1], counter_leds[1]}),
		.leds_red(tm1638_keys),
		.keys(tm1638_keys),
		.tm1638_strobe(tm1638_strobe),
		.tm1638_clk(tm1638_clk),
		.tm1638_data_io(tm1638_data_io)
		);

	wire clk_hs;
	
	
	`ifdef SIMULATION
		assign clk_hs = clk50MHz;
	`else
	//clk_hs 11.538ns ~ 86.67 MHz for lcd operation
	
   DCM_SP #(
      .CLKDV_DIVIDE(2.0),                   // CLKDV divide value
                                            // (1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5,8,9,10,11,12,13,14,15,16).
      .CLKFX_DIVIDE(15),                     // Divide value on CLKFX outputs - D - (1-32)
      .CLKFX_MULTIPLY(26),                   // Multiply value on CLKFX outputs - M - (2-32)
      .CLKIN_DIVIDE_BY_2("FALSE"),          // CLKIN divide by two (TRUE/FALSE)
      .CLKIN_PERIOD(20.000),                // Input clock period specified in nS
      .CLKOUT_PHASE_SHIFT("NONE"),          // Output phase shift (NONE, FIXED, VARIABLE)
      .CLK_FEEDBACK("1X"),                  // Feedback source (NONE, 1X, 2X)
      .DESKEW_ADJUST("SYSTEM_SYNCHRONOUS"), // SYSTEM_SYNCHRNOUS or SOURCE_SYNCHRONOUS
      .DFS_FREQUENCY_MODE("LOW"),           // Unsupported - Do not change value
      .DLL_FREQUENCY_MODE("LOW"),           // Unsupported - Do not change value
      .DSS_MODE("NONE"),                    // Unsupported - Do not change value
      .DUTY_CYCLE_CORRECTION("TRUE"),       // Unsupported - Do not change value
      .FACTORY_JF(16'hc080),                // Unsupported - Do not change value
      .PHASE_SHIFT(0),                      // Amount of fixed phase shift (-255 to 255)
      .STARTUP_WAIT("FALSE")                // Delay config DONE until DCM_SP LOCKED (TRUE/FALSE)
   )
	   DCM_SP_inst (
		  .CLK0(CLKFB),         // 1-bit output: 0 degree clock output
		  //.CLK180(CLK180),     // 1-bit output: 180 degree clock output
		  //.CLK270(CLK270),     // 1-bit output: 270 degree clock output
		  //.CLK2X(CLK2X),       // 1-bit output: 2X clock frequency clock output
		  //.CLK2X180(CLK2X180), // 1-bit output: 2X clock frequency, 180 degree clock output
		  //.CLK90(CLK90),       // 1-bit output: 90 degree clock output
		  //.CLKDV(CLKDV),       // 1-bit output: Divided clock output
		  .CLKFX(clk_hs),         // 1-bit output: Digital Frequency Synthesizer output (DFS)
		  //.CLKFX180(CLKFX180), // 1-bit output: 180 degree CLKFX output
		  .LOCKED(reset_n),     // 1-bit output: DCM_SP Lock Output
		  //.PSDONE(PSDONE),     // 1-bit output: Phase shift done output
		  //.STATUS(STATUS),     // 8-bit output: DCM_SP status output
		  .CLKFB(CLKFB),       // 1-bit input: Clock feedback input
		  .CLKIN(clk50MHz),        // 1-bit input: Clock input
		  .DSSEN(GND_PIN),       // 1-bit input: Unsupported, specify to GND.
		  .PSCLK(GND_PIN),       // 1-bit input: Phase shift clock input
		  .PSEN(GND_PIN),        // 1-bit input: Phase shift enable
		  .PSINCDEC(GND_PIN),    // 1-bit input: Phase shift increment/decrement input
		  .RST(GND_PIN)          // 1-bit input: Active high reset input
		);
	`endif
	hx8352_controller
		hx8352_controller_unit0
		(
		.clk(clk_hs),
		.rst(n_rst),
		.data_in(counter_low_frec[15:0] ),
		.lcd_rs(hx8352_rs),
		.lcd_wr(hx8352_wr),
		.lcd_rd(hx8352_rd),
		.lcd_rst(hx8352_rst),
		.lcd_cs(hx8352_cs),
		.data_output(hx8352_data),
		.debug_instruction_step(debug_instruction_step)
		);
//----------------------------------------------------------------------------
// Wires Assigments
//----------------------------------------------------------------------------
wire unused = io9  | io5  | io4  | io3  | io2  | io38 |
			  io95 | io94 | io92 | io91 | io90 | i89  |
			  i88  | io86 | io85 | io84 | io83 | io79 |
			  
			  io78 | io71 | i30  | i25  | io60 | uart_tx | uart_tx;
assign io70 = unused;

assign io10 = hx8352_data[0];
assign io11 = hx8352_data[1];
assign io12 = hx8352_data[2];
assign io15 = hx8352_data[3];
assign io16 = hx8352_data[4];
assign io17 = hx8352_data[5];
assign io18 = hx8352_data[6];
assign io22 = hx8352_data[7];
assign io23 = hx8352_data[8];
assign io26 = hx8352_data[9];
assign io32 = hx8352_data[10];
assign io33 = hx8352_data[11];
assign io34 = hx8352_data[12];
assign io35 = hx8352_data[13];
assign io36 = hx8352_data[14];
assign io40 = hx8352_data[15];

assign io41 = hx8352_rs;
assign io44 = hx8352_wr;
assign io53 = hx8352_rd;
assign io57 = hx8352_cs;
assign io58 = hx8352_rst;
assign leds = counter_leds[2:0];

endmodule
