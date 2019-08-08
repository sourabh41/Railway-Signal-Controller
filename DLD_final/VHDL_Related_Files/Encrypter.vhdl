----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    12:33:35 02/06/2018 
-- Design Name: 
-- Module Name:    Encrypter - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity Encrypter is
    Port ( clock : in  STD_LOGIC;
           K : in  STD_LOGIC_VECTOR (31 downto 0);
           P : in  STD_LOGIC_VECTOR (31 downto 0);
           C : out  STD_LOGIC_VECTOR (31 downto 0);
           reset : in  STD_LOGIC;
			  done : out STD_LOGIC;
           enable : in  STD_LOGIC);
end Encrypter;


architecture Behavioral of Encrypter is

	component Count
		Port ( K : in  STD_LOGIC_VECTOR (31 downto 0);
				 ans : out  STD_LOGIC_VECTOR (5 downto 0));
	end component;
	
	signal T : STD_LOGIC_VECTOR (3 downto 0);
	signal i: STD_LOGIC_VECTOR (5 downto 0);
	signal temp: STD_LOGIC_VECTOR(31 downto 0); 
	signal N : std_logic_vector (5 downto 0);

begin

	C1 : Count port map (K => K, ans => N);
	
   P1 : process(clock, reset, enable)
	begin
		if (reset = '1') then 
			temp <= "00000000000000000000000000000000";
			C <= "00000000000000000000000000000000";
			i <= "000000"; 
			done <= '0';
					
		elsif (clock'event and clock = '1' and enable = '1') then
			if (i = "000000") then
				temp <= P;				
				T(3) <= K(31) xor K(27) xor K(23) xor K(19) xor K(15) xor K(11) xor K(7) xor K(3);
				T(2) <= K(30) xor K(26) xor K(22) xor K(18) xor K(14) xor K(10) xor K(6) xor K(2);
				T(1) <= K(29) xor K(25) xor K(21) xor K(17) xor K(13) xor K(9) xor K(5) xor K(1);
				T(0) <= K(28) xor K(24) xor K(20) xor K(16) xor K(12) xor K(8) xor K(4) xor K(0);
				i <= i + "000001";
				done <= '0';
				
			elsif (i < N+1) then
				temp <= temp xor (T & T & T & T & T & T & T & T);
				T <= T + "0001";
				i <= i + "000001";
				done <= '0';
			elsif (i = N+1) then
				C <= temp;
				done <= '1';
				i <= i + "000001";
			end if; 
		end if;
	end process;

end Behavioral;