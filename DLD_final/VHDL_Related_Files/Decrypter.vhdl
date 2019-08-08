----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    12:35:05 02/06/2018 
-- Design Name: 
-- Module Name:    Decrypter - Behavioral 
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

entity Decrypter is
    Port ( clock : in  STD_LOGIC;
           K : in  STD_LOGIC_VECTOR (31 downto 0);
           C : in  STD_LOGIC_VECTOR (31 downto 0);
           P : out  STD_LOGIC_VECTOR (31 downto 0);
		   done : out STD_LOGIC;
           reset : in  STD_LOGIC;
           enable : in  STD_LOGIC);
end Decrypter;


architecture Behavioral of Decrypter is

	component Count
		Port ( K : in  STD_LOGIC_VECTOR (31 downto 0);
				 ans : out  STD_LOGIC_VECTOR (5 downto 0));
	end component;
	
	signal T : STD_LOGIC_VECTOR (3 downto 0);
	signal T1 : STD_LOGIC_VECTOR (3 downto 0);
	signal i: STD_LOGIC_VECTOR (5 downto 0);  
	signal temp: STD_LOGIC_VECTOR(31 downto 0); 
	signal N1 : std_logic_vector (5 downto 0);
	signal N0 : std_logic_vector (5 downto 0);

begin

	C1 : Count port map (K => K, ans => N1);
	N0 <= std_logic_vector(32 - unsigned(N1(5 downto 0)));
   
	P1 : process(clock, reset, enable)
	begin
		if (reset = '1') then 
			temp <= "00000000000000000000000000000000";
			P <= "00000000000000000000000000000000";
			i <= "000000";
			done <= '0';
			
		elsif (clock'event and clock = '1' and enable = '1') then
			if (i="000000") then
				temp <= C;			
				T(3) <= K(31) xor K(27) xor K(23) xor K(19) xor K(15) xor K(11) xor K(7) xor K(3);
				T(2) <= K(30) xor K(26) xor K(22) xor K(18) xor K(14) xor K(10) xor K(6) xor K(2);
				T(1) <= K(29) xor K(25) xor K(21) xor K(17) xor K(13) xor K(9) xor K(5) xor K(1);
				T(0) <= K(28) xor K(24) xor K(20) xor K(16) xor K(12) xor K(8) xor K(4) xor K(0);
				done <= '0';
				i <= i + "000001";
			elsif(i="000001") then	
				done <= '0';		
				T <= T+"1111";
				i <= i+"000001";
			elsif (i < N0+2) then
				done <= '0';
				temp <= temp xor (T & T & T & T & T & T & T & T);
				i <= i + "000001";
				T <= T + "1111";
			elsif (i = N0+2) then
				P <= temp;
				done <= '1';
				i <= i + "000001";
			end if;
		end if;
	end process;

end Behavioral;