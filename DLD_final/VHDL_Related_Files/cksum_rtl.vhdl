--
-- Copyright (C) 2009-2012 Chris McClelland
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU Lesser General Public License for more details.
--
-- You should have received a copy of the GNU Lesser General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;



architecture rtl of swled is

	-- Constants
	constant coordinates : std_logic_vector(7 downto 0) := "00010011";
	constant key : std_logic_vector(31 downto 0) := "11001100110011001100110011000001";
	constant ack1 : std_logic_vector(31 downto 0) := "10011011011001010101110110001101";
	constant ack2 : std_logic_vector(31 downto 0) := "01001101101000101011010100001101";
	constant reading_channel : std_logic_vector(6 downto 0) := "0000010";
	constant writing_channel : std_logic_vector(6 downto 0) := "0000011";

	-- Reading / writing signals
	signal output, output_next : std_logic_vector(7 downto 0) := "00000000";
	signal times_read, times_read_next : integer := 0;
	signal input, input_next : std_logic_vector(31 downto 0) := (others => '0');
	signal times_written, times_written_next : integer := 0;

	-- Decrypter signals
	signal enable_decrypter : std_logic;
	signal reset_decrypter : std_logic;
	signal decrypter_output : std_logic_vector(31 downto 0);
	signal decryption_done : std_logic;

	-- Encrypter signals
	signal encrypted_coordinates : std_logic_vector(31 downto 0);
	signal encrypted_ack1 : std_logic_vector(31 downto 0);
	signal coordinates_encryption_done : std_logic := '0';
	signal ack1_encryption_done : std_logic := '0';

	-- State signal
	signal state : integer := 0;

	-- Time signals
	signal ticks : integer := 0;
	signal time : integer := 0;
	signal time_mod3 : integer := 0;

	-- State 16 signals (board displays lights)
	signal direction_data : std_logic_vector(63 downto 0) := (others => '0');
	signal data_to_process : std_logic_vector(7 downto 0) := (others => '0');
	signal opposite_train_waiting : std_logic;
	signal train_waiting : std_logic;
	signal privilege : std_logic;
	signal display : std_logic_vector(7 downto 0) := (others => '0');

	-- S3 signals
	signal enable_encrypter : std_logic;
	signal reset_encrypter : std_logic;
	signal data_for_encryption : std_logic_vector(31 downto 0);
	signal encrypted_data : std_logic_vector(31 downto 0);
	signal data_encryption_done : std_logic;

	-- UART Signals
	constant c_CLKS_PER_BIT : integer := 434;
	signal uart_rx_dv : std_logic;
	signal uart_rx_data : std_logic_vector(7 downto 0);
	signal uart_data_received : std_logic;
	signal uart_tx_dv : std_logic;
	signal uart_tx_active : std_logic;
	signal uart_tx_done : std_logic;
	signal uart_data : std_logic_vector(7 downto 0);
	signal uart_data_to_be_used : std_logic := '0';

begin


	process(clk_in, reset_in)
	begin

		-- Reset button pressed, move to state 1
		if (reset_in = '1') then
			state <= 1;
			time <= 0;
			ticks <= 0;
			uart_data_received <= '0';


		-- Clock edge
		elsif ( rising_edge(clk_in) ) then

			output <= output_next;
			times_read <= times_read_next;
			input <= input_next;
			times_written <= times_written_next;
			if (uart_rx_dv = '1') then
				uart_data_received <= '1';
			end if ;
			if (uart_reset_in = '1') then
				uart_data_received <= '0';
				uart_data_to_be_used <= '0';
			end if ;


			-- When state is 0 (coordinates and ack1 encrypted)
			if (state = 0) then
				if (coordinates_encryption_done = '1' and ack1_encryption_done = '1') then
					state <= 1;
					time <= 0;
					ticks <= 0;
					uart_data_received <= '0';
				end if;


			-- When state is 1 (reset state)
			elsif (state = 1) then
				if ( ticks = 48000000 ) then
					time <= time + 1;
					ticks <= 0;
				else 
					ticks <= ticks + 1;
				end if;
				if (time = 3) then
					state <= 2;
					times_read <= 0;
					output <= encrypted_coordinates(31 downto 24);
				end if;


			-- When state is 2 (host reads encrypted coordinates)
			elsif (state = 2) then
				if (times_read = 4) then
					state <= 3;
					times_written <= 0;
					time <= 0;
					ticks <= 0;
				end if;
			

			-- When state is 3 (host writes encrypted coordinates)
			elsif (state = 3) then
				if ( ticks = 48000000 ) then
					time <= time + 1;
					ticks <= 0;
				else 
					ticks <= ticks + 1;
				end if;
				if (time = 256) then
					state <= 2;
					times_read <= 0;
					output <= encrypted_coordinates(31 downto 24);
				end if;

				if (times_written = 4) then
					state <= 4;
				end if;


			-- When state is 4 (coordinates being decrypted)
			elsif (state = 4) then
				if (decryption_done = '1') then
					if (decrypter_output = "000000000000000000000000" & coordinates) then
						state <= 5;
						times_read <= 0;
						output <= encrypted_ack1(31 downto 24);
					else
						state <= 2;
						times_read <= 0;
						output <= encrypted_coordinates(31 downto 24);
					end if;
				end if;


			-- When state is 5 (host reads encrypted ack1)
			elsif (state = 5) then
				if (times_read = 4) then
					state <= 6;
					times_written <= 0;
					time <= 0;
					ticks <= 0;
				end if;


			-- When state is 6 (host writes encrypted ack2)
			elsif (state = 6) then
				if ( ticks = 48000000 ) then
					time <= time + 1;
					ticks <= 0;
				else 
					ticks <= ticks + 1;
				end if;
				if (time = 256) then
					state <= 2;
					times_read <= 0;
					output <= encrypted_coordinates(31 downto 24);
				end if;

				if (times_written = 4) then
					state <= 7;
				end if;
				

			-- When state is 7 (ack2 being decrypted)
			elsif (state = 7) then
				if (decryption_done = '1') then
					if (decrypter_output = ack2) then
						state <= 8;
						times_written <= 0;
					else
						state <= 2;
						times_read <= 0;
						output <= encrypted_coordinates(31 downto 24);
					end if;
				end if;


			-- When state is 8 (host writes encrypted direction data)
			elsif (state = 8) then
				if (times_written = 4) then
					state <= 9;
				end if;
				

			-- When state is 9 (data being decrypted)
			elsif (state = 9) then
				if (decryption_done = '1') then
					direction_data(63 downto 32) <= decrypter_output;
					state <= 10;
					times_read <= 0;
					output <= encrypted_ack1(31 downto 24);
				end if;


			-- When state is 10 (host reads encrypted ack1)
			elsif (state = 10) then
				if (times_read = 4) then
					state <= 11;
					times_written <= 0;
				end if;
				

			-- When state is 11 (host writes encrypted direction data)
			elsif (state = 11) then
				if (times_written = 4) then
					state <= 12;
				end if;


			-- When state is 12 (data being decrypted)
			elsif (state = 12) then
				if (decryption_done = '1') then
					direction_data(31 downto 0) <= decrypter_output;
					state <= 13;
					times_read <= 0;
					output <= encrypted_ack1(31 downto 24);
				end if;
				

			-- When state is 13 (host reads encrypted ack1 and board uses UART data received previously)
			elsif (state = 13) then
				if (uart_data_to_be_used = '1') then
					if (uart_data(5 downto 3) = "000" and direction_data(63) = '1' and uart_data(7) = '1' ) then
						if (direction_data(62)='1') then
							direction_data(62) <= uart_data(6);
							direction_data(58 downto 56) <= uart_data(2 downto 0);
						end if;
					elsif( uart_data(5 downto 3) = "001" and direction_data(55) = '1' and uart_data(7) = '1' ) then
						if (direction_data(54)='1') then
							direction_data(54) <= uart_data(6);
							direction_data(50 downto 48) <= uart_data(2 downto 0);
						end if;
					elsif( uart_data(5 downto 3) = "010" and direction_data(47) = '1' and uart_data(7) = '1' ) then
						if (direction_data(46)='1') then
							direction_data(46) <= uart_data(6);
							direction_data(42 downto 40) <= uart_data(2 downto 0);
						end if;
					elsif( uart_data(5 downto 3) = "011" and direction_data(39) = '1' and uart_data(7) = '1' ) then
						if (direction_data(38)='1') then
							direction_data(38) <= uart_data(6);
							direction_data(34 downto 32) <= uart_data(2 downto 0);
						end if;	
					elsif( uart_data(5 downto 3) = "100" and direction_data(31) = '1' and uart_data(7) = '1' ) then
						if (direction_data(30)='1') then
							direction_data(30) <= uart_data(6);
							direction_data(26 downto 24) <= uart_data(2 downto 0);
						end if;
					elsif( uart_data(5 downto 3) = "101" and direction_data(23) = '1' and uart_data(7) = '1' ) then
						if (direction_data(22)='1') then
							direction_data(22) <= uart_data(6);
							direction_data(18 downto 16) <= uart_data(2 downto 0);
						end if;
					elsif( uart_data(5 downto 3) = "110" and direction_data(15) = '1' and uart_data(7) = '1' ) then
						if (direction_data(14)='1') then
							direction_data(14) <= uart_data(6);
							direction_data(10 downto 8) <= uart_data(2 downto 0);
						end if;
					elsif( uart_data(5 downto 3) = "111" and direction_data(7) = '1' and uart_data(7) = '1' ) then
						if (direction_data(6)='1') then
							direction_data(6) <= uart_data(6);
							direction_data(2 downto 0) <= uart_data(2 downto 0);
						end if;						
					end if;
					uart_data_to_be_used <= '0';
				end if ;

				if (times_read = 4) then
					state <= 14;
					times_written <= 0;
					time <= 0;
					ticks <= 0;
				end if;
				

			-- When state is 14 (host writes encrypted ack2)
			elsif (state = 14) then
				if ( ticks = 48000000 ) then
					time <= time + 1;
					ticks <= 0;
				else 
					ticks <= ticks + 1;
				end if;
				if (time = 256) then
					state <= 2;
					times_read <= 0;
					output <= encrypted_coordinates(31 downto 24);
				end if;
				
				if (times_written = 4) then
					state <= 15;
				end if;
				

			-- When state is 15 (ack2 being decrypted)
			elsif (state = 15) then
				if (decryption_done = '1') then
					if (decrypter_output = ack2) then
						state <= 16;
						time <= 0;
						time_mod3 <= 0;
						ticks <= 0;
					else
						state <= 2;
						times_read <= 0;
						output <= encrypted_coordinates(31 downto 24);
					end if;
				end if;
				

			-- When state is 16 (display state)
			elsif (state = 16) then

				-- Update time
				if ( ticks = 48000000 ) then
					time <= time + 1;
					time_mod3 <= time_mod3 + 1;
					ticks <= 0;
				else 
					ticks <= ticks + 1;
				end if;

				-- Drive display
				if ( time >= 0 and time < 24 ) then
					if ( ticks = 1 ) then
						if (time = 0) then data_to_process <= direction_data(63 downto 56); train_waiting <= sw_in(0); opposite_train_waiting <= sw_in(4); privilege <= '0';
						elsif (time = 3) then data_to_process <= direction_data(55 downto 48); train_waiting <= sw_in(1); opposite_train_waiting <= sw_in(5); privilege <= '0';
						elsif (time = 6) then data_to_process <= direction_data(47 downto 40); train_waiting <= sw_in(2); opposite_train_waiting <= sw_in(6); privilege <= '0';
						elsif (time = 9) then data_to_process <= direction_data(39 downto 32); train_waiting <= sw_in(3); opposite_train_waiting <= sw_in(7); privilege <= '0';
						elsif (time = 12) then data_to_process <= direction_data(31 downto 24); train_waiting <= sw_in(4); opposite_train_waiting <= sw_in(0); privilege <= '1';
						elsif (time = 15) then data_to_process <= direction_data(23 downto 16); train_waiting <= sw_in(5); opposite_train_waiting <= sw_in(1); privilege <= '1';
						elsif (time = 18) then data_to_process <= direction_data(15 downto 8); train_waiting <= sw_in(6); opposite_train_waiting <= sw_in(2); privilege <= '1';
						elsif (time = 21) then data_to_process <= direction_data(7 downto 0); train_waiting <= sw_in(7); opposite_train_waiting <= sw_in(3); privilege <= '1';
						end if;
						if (time_mod3 = 3) then
							time_mod3 <= 0;
						end if;
					elsif ( ticks = 2 ) then
						if ( data_to_process(7) = '1' and data_to_process(6) = '1' and train_waiting = '1' and opposite_train_waiting = '0') then
							if (data_to_process(2 downto 0) < "010") then
								display <= data_to_process(5 downto 3) & "00010";
							else
								display <= data_to_process(5 downto 3) & "00100";
							end if ;
						elsif ( data_to_process(7) = '1' and data_to_process(6) = '1' and train_waiting = '1' and opposite_train_waiting = '1' and privilege = '1') then
							if (time_mod3 = 0) then
								display <= data_to_process(5 downto 3) & "00100";
							elsif (time_mod3 = 1) then
								display <= data_to_process(5 downto 3) & "00010";
							else
								display <= data_to_process(5 downto 3) & "00001";
							end if;																			
						else
							display <= data_to_process(5 downto 3) & "00001";
						end if;
					end if;
				elsif ( time > 24 and time < 32 ) then
					display <= (others => '1');
				else
					display <= (others => '0');
				end if;

				-- Move to state 17 after 24 seconds
				if (time = 24) then
					state <= 17;
				end if;


			-- When state is 17 (just checking if up is pressed)
			elsif (state = 17) then
				if (up_in = '1') then
					state <= 18;
				else
					data_for_encryption <= "11111111111111111111111111111111";
					state <= 19;
				end if;


			-- When state is 18 (waiting till down pressed)
			elsif (state = 18) then
				if (down_in = '1') then
					data_for_encryption <= "000000000000000000000000" & sw_in;
					state <= 19;
				end if;


			-- When state is 19 (start encryption of data)
			elsif (state = 19) then
				if (data_encryption_done = '1') then
					state <= 20;
					times_read <= 0;
					output <= encrypted_data(31 downto 24);
				end if ;


			-- When state is 20 (host reads encrypted data)
			elsif (state = 20) then
				if (times_read = 4) then
					state <= 21;
				end if;


			-- When state is 21 (just checking if left is pressed)
			elsif (state = 21) then
				if (left_in = '1') then
					state <= 22;
				else
					state <= 24;
				end if ;


			-- When state is 22 (waiting till right pressed)
			elsif (state = 22) then
				if (right_in = '1') then
					state <= 23;
					uart_tx_dv <= '1';
				end if ;


			-- When state is 23 (data sending on UART)
			elsif (state = 23) then
				if (uart_tx_done = '1') then
					state <= 24;
					uart_tx_dv <= '0';
				end if ;


			-- When state is 24 (check if data received on UART)
			elsif (state = 24) then
				if (uart_data_received = '1') then
					state <= 25;
					uart_data_received <= '0';
				else
					state <= 26;	
					time <= 0;
					ticks <= 0;
				end if ;


			-- When state is 25 (update direction data using UART data)
			elsif (state = 25) then
				uart_data <= uart_rx_data;
				uart_data_to_be_used <= '1';
				state <= 26;
				time <= 0;
				ticks <= 0;


			-- When state is 26 (wait for T0 and move to state 2)
			elsif (state = 26) then
				if ( ticks = 48000000 ) then
					time <= time + 1;
					ticks <= 0;
				else 
					ticks <= ticks + 1;
				end if;
				if (time = 3) then
					state <= 2;
					times_read <= 0;
					output <= encrypted_coordinates(31 downto 24);
				end if;

			end if;

		end if;

	end process;

	----------------------------------------------------------------------------------------------------------------------------------

	-- Encrypter for coordinates
	coordinates_encrypter : entity work.Encrypter
		port map(
			clock => clk_in,
			K => key,
			P => "000000000000000000000000" & coordinates,
			reset => '0',
			enable => '1',
			C => encrypted_coordinates,
			done => coordinates_encryption_done
		);

	-- Encrypter for ack1
	ack1_encrypter : entity work.Encrypter
		port map(
			clock => clk_in,
			K => key,
			P => ack1,
			reset => '0',
			enable => '1',
			C => encrypted_ack1,
			done => ack1_encryption_done
		);

	-- Encrypter for state S3
	s3_encrypter : entity work.Encrypter
		port map(
			clock => clk_in,
			K => key,
			P => data_for_encryption,
			reset => reset_encrypter,
			enable => enable_encrypter,
			C => encrypted_data,
			done => data_encryption_done
		);

	-- Select when S3 encrypter is enabled and resetted
	with state select enable_encrypter <=
		'1' when 19,
		'0' when others;

	with state select reset_encrypter <=
		'0' when 19,
		'0' when 20,
		'1' when others;

	----------------------------------------------------------------------------------------------------------------------------------

	-- Common decrypter
	decrypter : entity work.Decrypter
		port map(
			clock => clk_in,
			K => key,
			C => input,
			reset => reset_decrypter,
			enable => enable_decrypter,
			P => decrypter_output,
			done => decryption_done
		);

	-- Select when decrypter is enabled
	with state select enable_decrypter <=
		'1' when 4,
		'1' when 7,
		'1' when 15,
		'1' when 9,
		'1' when 12,
		'0' when others;

	-- Select when decrypter is resetted
	with state select reset_decrypter <=
		'0' when 4,
		'0' when 7,
		'0' when 15,
		'0' when 9,
		'0' when 10,
		'0' when 12,
		'0' when 13,
		'1' when others;

	----------------------------------------------------------------------------------------------------------------------------------

	-- Select when can host read
	with state select f2hValid_out <=
		'1' when 2,
		'1' when 5,
		'1' when 10,
		'1' when 13,
		'1' when 20,
		'0' when others;

	-- Select values to return for each channel when the host is reading
	with chanAddr_in select f2hData_out <=
		output when reading_channel,
		x"00" when others;

	-- Output when reading
	output_next <= 
		encrypted_coordinates(23 downto 16) when chanAddr_in = reading_channel and f2hReady_in = '1' and times_read = 0 and state = 2
		else encrypted_coordinates(15 downto 8) when chanAddr_in = reading_channel and f2hReady_in = '1' and times_read = 1 and state = 2
		else encrypted_coordinates(7 downto 0) when chanAddr_in = reading_channel and f2hReady_in = '1' and times_read = 2 and state = 2
		else encrypted_ack1(23 downto 16) when chanAddr_in = reading_channel and f2hReady_in = '1' and times_read = 0 and (state = 5 or state = 10 or state = 13)
		else encrypted_ack1(15 downto 8) when chanAddr_in = reading_channel and f2hReady_in = '1' and times_read = 1 and (state = 5 or state = 10 or state = 13)
		else encrypted_ack1(7 downto 0) when chanAddr_in = reading_channel and f2hReady_in = '1' and times_read = 2 and (state = 5 or state = 10 or state = 13)
		else encrypted_data(23 downto 16) when chanAddr_in = reading_channel and f2hReady_in = '1' and times_read = 0 and state = 20
		else encrypted_data(15 downto 8) when chanAddr_in = reading_channel and f2hReady_in = '1' and times_read = 1 and state = 20
		else encrypted_data(7 downto 0) when chanAddr_in = reading_channel and f2hReady_in = '1' and times_read = 2 and state = 20
		else output;

	times_read_next <= 
		times_read + 1 when chanAddr_in = reading_channel and f2hReady_in = '1'
		else times_read;

	----------------------------------------------------------------------------------------------------------------------------------

	-- Select when can host write
	with state select h2fReady_out <=
		'1' when 2,
		'1' when 3,
		'1' when 6,
		'1' when 8,
		'1' when 11,
		'1' when 14,
		'0' when others;

	-- Input when writing
	input_next <=
		input(23 downto 0) & h2fData_in when chanAddr_in = writing_channel and h2fValid_in = '1'
		else input;

	times_written_next <=
		times_written + 1 when chanAddr_in = writing_channel and h2fValid_in = '1'
		else times_written;

	----------------------------------------------------------------------------------------------------------------------------------
	
	-- LEDs
	-- with state select led_out <=
	--	"11111111" when 1,
	--	display when 16,
	--	"00000000" when others;
	with state select led_out <=
		"111" & uart_data_received & uart_data_to_be_used & "111" when 1,
		display(7 downto 5) & uart_data_received & uart_data_to_be_used & display(2 downto 0) when 16,
		"000" & uart_data_received & uart_data_to_be_used & "000" when others;
	-- led_out <= std_logic_vector(to_unsigned(state, led_out'length));
	-- led_out <= uart_rx_data;

	-- seven segment display
	seven_seg : entity work.seven_seg
		port map(
			clk_in => clk_in,
			data_in => "0000000000000000",
			dots_in => "00" & f2hReady_in & reset_in,
			segs_out => sseg_out,
			anodes_out => anode_out
		);

	----------------------------------------------------------------------------------------------------------------------------------

	-- UART

	uart_rx : entity work.uart_rx
		generic map(
    		g_CLKS_PER_BIT => c_CLKS_PER_BIT
    	)
    	port map(
    		i_clk       => clk_in,
    		i_rx_serial => rx_in,
    		o_rx_dv     => uart_rx_dv,
    		o_rx_byte   => uart_rx_data
    	);

    uart_tx : entity work.uart_tx
    	generic map(
			g_CLKS_PER_BIT => c_CLKS_PER_BIT
		)
		port map(
			i_clk       => clk_in,
			i_tx_dv     => uart_tx_dv,
			i_tx_byte   => sw_in,
			o_tx_active => uart_tx_active,
			o_tx_serial => tx_out,
			o_tx_done   => uart_tx_done
		);

	----------------------------------------------------------------------------------------------------------------------------------


end architecture;
