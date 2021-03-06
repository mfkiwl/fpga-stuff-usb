module hx8352_controller
(
	input  clk,
	input  rst,
	input  step,
	input  write_cmd,
	input  [7:0] cmd_in,
	input  [15:0] data_in,	
	
	output busy,
	output [15:0] data_output,
	output lcd_rs,
	output lcd_wr,
	output lcd_rd,
	output lcd_rst,
	output lcd_cs,
	output [15:0]debug_instruction_step
);	
	reg  [15:0] data_to_write;
	reg  command_or_data;
	wire  bus_controller_step;
	wire  lcd_rst_done;
	wire  bus_busy;
	
	localparam 
        LCD_CMD    = 1'b0,
        LCD_DATA   = 1'b1;
    /* 6.1 Command set = Taken from datasheet */
	localparam	
			CMD_Product_ID						=	8'h00,
			CMD_Display_mode					=	8'h01,
			CMD_Column_Address_Start_1			=	8'h02,
			CMD_Column_Address_Start_2			=	8'h03,
			CMD_Column_Address_End_1			=	8'h04,
			CMD_Column_Address_End_2			=	8'h05,
			CMD_Row_Address_Start_1				=	8'h06,
			CMD_Row_Address_Start_2				=	8'h07,
			CMD_Row_Address_End_1				=	8'h08,
			CMD_Row_Address_End_2				=	8'h09,
			CMD_Partial_Area_Start_Row_1		=	8'h0a,
			CMD_Partial_Area_Start_Row_2		=	8'h0b,
			CMD_Partial_Area_End_Row_1			=	8'h0c,
			CMD_Partial_Area_End_Row_2			=	8'h0d,
			CMD_Vertical_Scroll_Top_Fixed_Area_1=	8'h0e,
			CMD_Vertical_Scroll_Top_Fixed_Area_2=	8'h0f,
			CMD_Vertical_Scroll_Height_Area_1	=	8'h10,
			CMD_Vertical_Scroll_Height_Area_2	=	8'h11,
			CMD_Vertical_Scroll_Button_Fixed_1	=	8'h12,
			CMD_Vertical_Scroll_Button_Fixed_2	=	8'h13,
			CMD_Vertical_Scroll_Start_Address_1	=	8'h14,
			CMD_Vertical_Scroll_Start_Address_2	=	8'h15,
			CMD_Memory_Access_Control			=	8'h16,
			CMD_OSC_Control_1					=	8'h17,
			CMD_OSC_Control_2					=	8'h18,
			CMD_Power_Control_1					=	8'h19,
			CMD_Power_Control_2					=	8'h1a,
			CMD_Power_Control_3					=	8'h1b,
			CMD_Power_Control_4					=	8'h1c,
			CMD_Power_Control_5					=	8'h1d,
			CMD_Power_Control_6					=	8'h1e,
			CMD_VCOM_Control					=	8'h1f,
			CMD_Data_read_write					=	8'h22,
			CMD_Display_Control_1				=	8'h23,
			CMD_Display_Control_2				=	8'h24,
			CMD_Display_Control_3				=	8'h25,
			CMD_Display_Control_4				=	8'h26,
			CMD_Display_Control_5				=	8'h27,
			CMD_Display_Control_6				=	8'h28,
			CMD_Display_Control_7				=	8'h29,
			CMD_Display_Control_8				=	8'h2a,
			CMD_Cycle_Control_1					=	8'h2b,
			CMD_Cycle_Control_2					=	8'h2c,
			CMD_Cycle_Control_3					=	8'h2d,
			CMD_Cycle_Control_4					=	8'h2e,
			CMD_Cycle_Control_5					=	8'h2f,
			CMD_Cycle_Control_6					=	8'h30,
			CMD_Cycle_Control_7					=	8'h31,
			CMD_Cycle_Control_8					=	8'h32,
			CMD_Cycle_Control_10				=	8'h34,
			CMD_Cycle_Control_11				=	8'h35,
			CMD_Cycle_Control_12				=	8'h36,
			CMD_Cycle_Control_13				=	8'h37,
			CMD_Cycle_Control_14				=	8'h38,
			CMD_Cycle_Control_15				=	8'h39,
			CMD_Interface_Control_1				=	8'h3a,
			CMD_Source_Control_1				=	8'h3c,
			CMD_Source_Control_2				=	8'h3d,
			CMD_Gamma_Control_1					=	8'h3e,
			CMD_Gamma_Control_2					=	8'h3f,
			CMD_Gamma_Control_3					=	8'h40,
			CMD_Gamma_Control_4					=	8'h41,
			CMD_Gamma_Control_5					=	8'h42,
			CMD_Gamma_Control_6					=	8'h43,
			CMD_Gamma_Control_7					=	8'h44,
			CMD_Gamma_Control_8					=	8'h45,
			CMD_Gamma_Control_9					=	8'h46,
			CMD_Gamma_Control_10				=	8'h47,
			CMD_Gamma_Control_11				=	8'h48,
			CMD_Gamma_Control_12				=	8'h49,
			CMD_PANEL_Control					=	8'h55,
			CMD_OTP_1							=	8'h56,
			CMD_OTP_2							=	8'h57,
			CMD_OTP_3							=	8'h58,
			CMD_OTP_4							=	8'h59,
			CMD_IP_Control						=	8'h5a,
			CMD_DGC_LUT_WRITE					=	8'h5c,
			CMD_DATA_Control					=	8'h5d,
			CMD_Test_Mode						=	8'h83,
			CMD_VDDD_control					=	8'h85,
			CMD_Powr_driving_Control			=	8'h8A,
			CMD_VGS_RES_control_1				=	8'h8B,
			CMD_VGS_RES_control_2				=	8'h8C,
			CMD_PWM_Control_0					=	8'h91,
			CMD_PWM_Control_1					=	8'h95,
			CMD_PWM_Control_2					=	8'h96,
			CMD_PWM_Control_3					=	8'h97,
			CMD_CABC_Period_Control_1			=	8'h6B,
			CMD_CABC_Period_Control_2			=	8'h6C,
			CMD_CABC_Gain1						=	8'h6F,
			CMD_CABC_Gain2						=	8'h70,
			CMD_CABC_Gain3						=	8'h71,
			CMD_CABC_Gain4						=	8'h72,
			CMD_CABC_Gain5						=	8'h73,
			CMD_CABC_Gain6						=	8'h74,
			CMD_CABC_Gain7						=	8'h75,
			CMD_CABC_Gain8						=	8'h76,
			CMD_CABC_Gain9						=	8'h77;    
        
        
	hx8352_reset_generator
		hx8352_reset_generator_unit
		(
			.clk(clk),
			.rst(rst),
			.lcd_rst(lcd_rst),
			.lcd_rst_done(lcd_rst_done)
		);

	hx8352_controller_bus_controller 
		hx8352_controller_bus_controller_unit
		(
			.clk(clk),
			.rst(~lcd_rst_done),
			.data_input(data_to_write),
			.data_command(command_or_data),
			.transfer_step(bus_controller_step),
			.busy(bus_busy),
			.data_output(data_output),
			.lcd_rs(lcd_rs),
			.lcd_wr(lcd_wr),
			.lcd_rd(lcd_rd)
		);
	
	wire fsm_clk;
	reg  fsm_clk_reg;	
	
	always @(posedge clk or posedge rst)
	begin
		if(rst) begin
			fsm_clk_reg <= 0;
			data_in_reg <= 0;
		end
		else begin
			fsm_clk_reg <= fsm_clk;
		end
	end
	
	assign fsm_clk = ~fsm_clk_reg;
	
	
	
	wire  step_sync;
	reg   step_reg;	
	reg   write_cmd_reg;
	reg  [7:0] cmd_in_reg;
	reg  [15:0] data_in_reg;
	
	always @(posedge fsm_clk or posedge fsm_rst)
	begin
		if(fsm_rst) begin
			step_reg <= 0;
			cmd_in_reg <= 0;
			data_in_reg <= 0;
			write_cmd_reg <= 0;
		end
		else begin
			step_reg <= step;
			cmd_in_reg <= cmd_in;
			data_in_reg <= data_in;
			write_cmd_reg <= write_cmd;
		end
	end
	
	assign step_sync = step & ~step_reg;	
	
	reg [7:0] delay_value;
	reg delay_step;
	wire delay_done;
    hx8352_delay_ms
	   hx8352_delay_ms_unit
	   (
		.clk(clk),
		.rst(rst),
		.step(delay_step),
		.delay_ms(delay_value),
		.done(delay_done)
	   );
	localparam 
		HIGH    = 1'b1,
		LOW     = 1'b0,
		STATE_INITIALIZE	= 3'b000,
		STATE_IDLE			= 3'b001,
		STATE_TRANSFER_CMD 	= 3'b010,
		STATE_TRANSFER_PIXEL= 3'b011;
	
	wire        fsm_rst;
	reg         cs_reg;
	reg         init_done;
    reg 		bus_controller_step_reg;
    reg  [2:0]  fsm_state;
	reg  [7:0]  instruction_step_reg;
    wire [7:0]  instruction_step_next;
    
    assign fsm_rst = rst | ~lcd_rst_done;
	assign instruction_step_next = instruction_step_reg + 1;
    
    always @(posedge fsm_clk or posedge fsm_rst) begin
        if (fsm_rst) begin
            instruction_step_reg <= 8'h00;
            data_to_write <= 16'h0;
            bus_controller_step_reg <= 0;
            delay_step <= 0;
            init_done <= LOW;
            delay_value <= 8'h0;
            command_or_data <= HIGH;
            cs_reg <= 1;
            fsm_state <= STATE_INITIALIZE;
        end 
        else begin
			if (~bus_busy & delay_done) begin				
				case (fsm_state)
					STATE_IDLE: begin
						if(step_sync) begin
							if(write_cmd_reg)
								fsm_state <= STATE_TRANSFER_CMD;
							else
								fsm_state <= STATE_TRANSFER_PIXEL;
						end
						instruction_step_reg <= 8'h00;
						bus_controller_step_reg <= LOW;
					end
					STATE_TRANSFER_CMD: begin
						bus_controller_step_reg <= HIGH;
						case (instruction_step_reg)
							8'h00:  cs_reg <= LOW;
							8'h01:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, cmd_in_reg}}; 
							8'h02:  {command_or_data, data_to_write}     <= {LCD_DATA, data_in_reg}; 
							8'h03:  begin //! NOP
									cs_reg <= HIGH;
									command_or_data <= LCD_CMD;
									data_to_write <= {8'h00, CMD_Product_ID};
									fsm_state <= STATE_IDLE;						
									end
							default: fsm_state <= STATE_IDLE; 
						endcase
					end			
					STATE_TRANSFER_PIXEL: begin
						bus_controller_step_reg <= HIGH;
						case (instruction_step_reg)
							8'h00:  cs_reg <= LOW;
							8'h01:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Data_read_write}}; 
							8'h02:  {command_or_data, data_to_write}     <= {LCD_DATA, data_in_reg}; 
							8'h03:  begin //! NOP
									cs_reg <= HIGH;
									command_or_data <= LCD_CMD;
									data_to_write <= {8'h00, CMD_Product_ID};
									fsm_state <= STATE_IDLE;						
									end
							default: fsm_state <= STATE_IDLE; 
						endcase
					end
					STATE_INITIALIZE: begin
						bus_controller_step_reg <= HIGH;
						case (instruction_step_reg)
							8'h00:  {cs_reg, instruction_step_reg}     <= {LOW, 8'h10};
							8'h10:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Test_Mode}};
							8'h11:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h02}};//TESTM=1 
							8'h12:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_VDDD_control}};
							8'h13:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h03}};//VDC_SEL=011
							8'h14:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_VGS_RES_control_1}};
							8'h15:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h01}};
							8'h16:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_VGS_RES_control_2}};
							8'h17:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h93}};//STBA[7]=1,STBA[5:4]=01,STBA[1:0]=11
							8'h18:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_PWM_Control_0}};
							8'h19:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h01}};//DCDC_SYNC=1
							8'h1a:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Test_Mode}};
							8'h1b:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h00}}; //TESTM=0
							8'h1c:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Gamma_Control_1}};//Gamma Setting
							8'h1d:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'hB0}};
							8'h1e:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Gamma_Control_2}};
							8'h1f:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h03}};
							8'h20:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Gamma_Control_3}};
							8'h21:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h10}};
							8'h22:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Gamma_Control_5}};
							8'h23:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h13}};
							8'h24:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Gamma_Control_6}};
							8'h25:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h46}};
							8'h26:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Gamma_Control_7}};
							8'h27:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h23}};
							8'h28:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Gamma_Control_8}};
							8'h29:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h76}};
							8'h2a:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Gamma_Control_9}};
							8'h2b:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h00}};
							8'h2c:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Gamma_Control_10}};
							8'h2d:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h5E}};
							8'h2e:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Gamma_Control_11}};
							8'h2f:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h4F}};
							8'h30:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Gamma_Control_12}};
							8'h31:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h40}};
							8'h32:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00,CMD_OSC_Control_1}};//**********Power On sequence************
							8'h33:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h91}};
							8'h34:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Cycle_Control_1}};
							8'h35:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'hF9}};
							8'h36:  begin //! Delay 10 ms
									delay_step   <= HIGH;
									delay_value  <= 8'd10;
									instruction_step_reg <= 8'h40;
									end
							8'h40:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Power_Control_3}};
							8'h41:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h14}};
							8'h42:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Power_Control_2}};
							8'h43:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h11}};
							8'h44:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Power_Control_4}};
							8'h45:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h06}}; // 0d
							8'h46:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_VCOM_Control}};
							8'h47:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h42}};
							8'h48:  begin //! Delay 20 ms
									delay_step   <= HIGH;
									delay_value  <= 8'd20;
									instruction_step_reg <= 8'h50;
									end
							8'h50:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Power_Control_1}};
							8'h51:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h0A}};
							8'h52:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Power_Control_1}};
							8'h53:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h1A}};
							8'h54:  begin //! Delay 40 ms
									delay_step   <= HIGH;
									delay_value  <= 8'd60;
									instruction_step_reg <= 8'h60;
									end
							8'h60:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Power_Control_1}};
							8'h61:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h12}};
							8'h62:  begin //! Delay 40 ms
									delay_step   <= HIGH;
									delay_value  <= 8'd40;
									instruction_step_reg <= 8'h70;
									end
							8'h70:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Power_Control_6}};
							8'h71:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h27}};
							8'h72:  begin //! Delay 40 ms
									delay_step   <= HIGH;
									delay_value  <= 8'd100;
									instruction_step_reg <= 8'h80;
									end
							8'h80:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Display_Control_2}};//**********DISPLAY ON SETTING***********
							8'h81:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h60}};					
							8'h82:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Source_Control_2}};
							8'h83:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h40}};					
							8'h84:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Cycle_Control_10}};
							8'h85:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h38}};					
							8'h86:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Cycle_Control_11}};
							8'h87:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h38}};					
							8'h88:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Display_Control_2}};
							8'h89:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h38}};
							8'h8A:  begin //! Delay 40 ms
									delay_step   <= HIGH;
									delay_value  <= 8'd40;
									instruction_step_reg <= 8'h90;
									end
							8'h90:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Display_Control_2}};
							8'h91:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h3C}};					
							8'h92:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Memory_Access_Control}};
							8'h93:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h1C}};					
							8'h94:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Display_mode}};
							8'h95:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h06}};					
							8'h96:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_PANEL_Control}};
							8'h97:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h00}};					
							8'h98:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Column_Address_Start_1}};
							8'h99:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h00}};
							8'h9A:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Column_Address_Start_2}};
							8'h9B:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h00}};
							8'h9C:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Column_Address_End_1}};
							8'h9D:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h00}};
							8'h9E:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Column_Address_End_2}};
							8'h9F:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'hEF}};					
							8'hA0:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Row_Address_Start_1}};
							8'hA1:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h00}};
							8'hA2:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Row_Address_Start_2}};
							8'hA3:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h00}};
							8'hA4:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Row_Address_End_1}};
							8'hA5:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h01}};
							8'hA6:  {command_or_data, data_to_write}     <= {LCD_CMD,  {8'h00, CMD_Row_Address_End_2}};
							8'hA7:  {command_or_data, data_to_write}     <= {LCD_DATA, {8'h00, 8'h8F}};					
							8'hA8:  begin //! NOP
									cs_reg <= HIGH;
									command_or_data <= LCD_CMD;
									data_to_write <= {8'h00, CMD_Product_ID};
									fsm_state <= STATE_IDLE;
									init_done <= HIGH; 							
									end
							default: fsm_state <= STATE_IDLE; 
						endcase
					end
				default: fsm_state <= STATE_INITIALIZE;
				endcase
			end
			else if(~delay_done) begin
				delay_step <= LOW;
				bus_controller_step_reg <= LOW;
			end 
			else begin 
				instruction_step_reg <= instruction_step_next;
				bus_controller_step_reg <= LOW;
			end       
        end
    end
    
    assign bus_controller_step = bus_controller_step_reg & delay_done & ~delay_step;
	assign busy = bus_busy | ~lcd_rst_done | (fsm_state != STATE_IDLE) | step_sync | bus_controller_step; 
	assign debug_instruction_step = instruction_step_reg;
	assign lcd_cs = cs_reg;
endmodule
