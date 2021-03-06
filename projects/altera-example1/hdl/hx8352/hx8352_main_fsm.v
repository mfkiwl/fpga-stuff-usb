
module hx8352_main_fsm(
	input clk,
	input rst,
	input bus_done,
	input delay_done,
	
	input step,
	input  [3:0] cmd_in,
	input  [15:0] data_in,
	
	output init_done,
	output reg [15:0]data_to_write,
	output reg command_or_data,
	output reg bus_step,
	output reg [15:0]delay_value,
	output reg delay_step,
	output reg lcd_cs,
	output reg lcd_rst
);
localparam 
HIGH    = 1'b1,
LOW     = 1'b0,
LCD_CMD    = 1'b0,
LCD_DATA   = 1'b1,
CMD_Product_ID								=	8'h00,
CMD_Data_read_write						=	8'h22,

STATE_INITIALIZE							= 4'h1,
STATE_RESET_WAIT							= 4'h9,
STATE_RESET_WAIT_DONE					= 4'ha,
STATE_INIT_SEQ								= 4'hb,
STATE_IDLE									= 4'h2,
STATE_PROCESS_CMD							= 4'he,
STATE_TRANSFER_PIXEL						= 4'h4,
STATE_TRANSFER_PIXEL_LOAD_CMD 		= 4'h5,
STATE_TRANSFER_PIXEL_LOAD_CMD_END	= 4'h6,
STATE_TRANSFER_PIXEL_LOAD_DATA		= 4'h7,
STATE_TRANSFER_PIXEL_LOAD_DATA_END	= 4'h8,

CMD_NOP		=	4'h0,
CMD_HOME		= 	4'h1,
CMD_SET_X	=	4'h2,
CMD_SET_Y	=	4'h3,
CMD_SET_XY	=	4'h4,
CMD_SET_P	=	4'h5
;
/*
 Input register
 *step
 Commands cmd_in
 Home
 Set x-pos ->
 Set y-pos ->
 Set xy-pos->
 Set pixel ->
*/

reg  lcd_init_step;
wire init_bus_step, init_command_or_data, init_delay_step;
wire [15:0]init_data_to_write, init_delay_value;
hx8352_lcd_init 
	init_ut0( 
	.clk(clk),
	.rst(rst),
	.step(lcd_init_step),
	.bus_done(bus_done),
	.delay_step(init_delay_step),
	.delay_value(init_delay_value),
	.delay_done(delay_done),
	.data_to_write(init_data_to_write),
	.command_or_data(init_command_or_data),
	.done(init_done),
	.bus_step(init_bus_step)
	);
	
reg [3:0]  fsm_state;
reg [15:0] delay_value_reg, data_to_write_reg;
reg bus_step_reg, command_or_data_reg, delay_step_reg;
always @(posedge clk or posedge rst) begin
  if (rst) begin
		fsm_state <= STATE_INITIALIZE;
		lcd_init_step <= LOW;
		data_to_write_reg <= 16'h0000;
		bus_step_reg <= LOW;
		command_or_data_reg <= LCD_CMD;
		lcd_cs <= HIGH;
		lcd_rst <= HIGH;
		delay_step_reg <= LOW;
  end else begin 	
		case (fsm_state)
			STATE_IDLE: begin
				lcd_cs <= HIGH;
				if(step) 
					fsm_state <= STATE_PROCESS_CMD;
				else
					fsm_state <= STATE_IDLE;					
			end
			STATE_PROCESS_CMD: begin
				case (cmd_in) 
				CMD_NOP: fsm_state <= STATE_IDLE;
				CMD_HOME: begin
					fsm_state <= STATE_IDLE;
				end
				CMD_SET_X: begin
					fsm_state <= STATE_IDLE;
				end
				CMD_SET_Y: begin
					fsm_state <= STATE_IDLE;
				end
				CMD_SET_XY: begin
					fsm_state <= STATE_IDLE;
				end	
				CMD_SET_P: begin
					fsm_state <= STATE_TRANSFER_PIXEL;
				end			
				default: fsm_state <= STATE_IDLE;
				endcase
			end
			STATE_TRANSFER_PIXEL: begin
				lcd_cs <= LOW;
				bus_step_reg <= LOW;
				fsm_state <= STATE_TRANSFER_PIXEL_LOAD_CMD; 
			end
			STATE_TRANSFER_PIXEL_LOAD_CMD : begin
				command_or_data_reg <= LCD_CMD;
				data_to_write_reg   <= {8'h00, CMD_Data_read_write};
				bus_step_reg <= HIGH;
				fsm_state <= STATE_TRANSFER_PIXEL_LOAD_CMD_END; 
			end 
			STATE_TRANSFER_PIXEL_LOAD_CMD_END : begin
				bus_step_reg <= LOW;
				if(bus_done)
					fsm_state <= STATE_TRANSFER_PIXEL_LOAD_DATA; 
			end
			STATE_TRANSFER_PIXEL_LOAD_DATA: begin 
				command_or_data_reg <= LCD_DATA;
				data_to_write_reg <= data_in;
				bus_step_reg <= HIGH;
				fsm_state <= STATE_TRANSFER_PIXEL_LOAD_DATA_END; 
			end
			STATE_TRANSFER_PIXEL_LOAD_DATA_END : begin
				bus_step_reg <= LOW;
				if(bus_done) begin
					fsm_state <= STATE_IDLE; 
				end
			end
			STATE_INITIALIZE: begin
				lcd_rst <= LOW;
				delay_step_reg <= HIGH;
				delay_value_reg <= 16'd50;
				fsm_state <= STATE_RESET_WAIT; 
			end
			STATE_RESET_WAIT: begin
				delay_step_reg <= LOW;
				if(delay_done) begin
					lcd_rst <= HIGH;
					fsm_state <= STATE_RESET_WAIT_DONE;
					delay_step_reg <= HIGH;
					delay_value_reg <= 16'd60;
					fsm_state <= STATE_RESET_WAIT_DONE; 
				end else
					fsm_state <= STATE_RESET_WAIT; 
			end
			STATE_RESET_WAIT_DONE: begin
				delay_step_reg <= LOW;
				if(delay_done) begin
					lcd_init_step <= HIGH;
					lcd_cs <= LOW;
					fsm_state <= STATE_INIT_SEQ; 
				end else
					fsm_state <= STATE_RESET_WAIT_DONE; 
			end
			STATE_INIT_SEQ: begin
				lcd_init_step <= LOW;

				if(init_done) begin
					fsm_state <= STATE_IDLE;
				end 
			end
		default: fsm_state <= STATE_INITIALIZE;
		endcase
	end
end
//select signals based on the state, this should be just a mux
always @(*) begin
	case(fsm_state)
		STATE_INIT_SEQ:begin
			bus_step = init_bus_step;
			command_or_data = init_command_or_data;
			delay_step = init_delay_step;
			delay_value = init_delay_value;
			data_to_write = init_data_to_write;
		end
		default:begin
			bus_step = bus_step_reg;
			command_or_data = command_or_data_reg;
			delay_step = delay_step_reg;
			delay_value = delay_value_reg;
			data_to_write = data_to_write_reg;
		end
	endcase
end

endmodule 

