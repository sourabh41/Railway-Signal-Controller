----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    12:32:33 02/06/2018 
-- Design Name: 
-- Module Name:    Count - Behavioral 
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity Count is
    Port ( K : in  STD_LOGIC_VECTOR (31 downto 0);
           ans : out  STD_LOGIC_VECTOR (5 downto 0));
end Count;


architecture Behavioral of Count is

	component HalfAdder
		Port ( x : in  STD_LOGIC;
				 y : in  STD_LOGIC;
				 sum : out  STD_LOGIC;
				 carry : out  STD_LOGIC);
	end component;
	component FullAdder
		Port ( x : in  STD_LOGIC;
				 y : in  STD_LOGIC;
				 c : in  STD_LOGIC;
				 sum : out  STD_LOGIC;
				 carry : out  STD_LOGIC);
	end component;
	
	signal level0 : STD_LOGIC_VECTOR (15 downto 0);
	signal level1 : STD_LOGIC_VECTOR (24 downto 0);	
	signal level2 : STD_LOGIC_VECTOR (11 downto 0);
	signal level3 : STD_LOGIC_VECTOR (4 downto 0);
	signal level4 : STD_LOGIC_VECTOR (1 downto 0);
	
begin

	FA0 : FullAdder port map( x => K(0), y => K(1), c => K(2), sum => level0(0), carry => level1(0));
	FA1 : FullAdder port map( x => K(3), y => K(4), c => K(5), sum => level0(1), carry => level1(1));
	FA2 : FullAdder port map( x => K(6), y => K(7), c => K(8), sum => level0(2), carry => level1(2));
	FA3 : FullAdder port map( x => K(9), y => K(10), c => K(11), sum => level0(3), carry => level1(3));
	FA4 : FullAdder port map( x => K(12), y => K(13), c => K(14), sum => level0(4), carry => level1(4));
	FA5 : FullAdder port map( x => K(15), y => K(16), c => K(17), sum => level0(5), carry => level1(5));
	FA6 : FullAdder port map( x => K(18), y => K(19), c => K(20), sum => level0(6), carry => level1(6));
	FA7 : FullAdder port map( x => K(21), y => K(22), c => K(23), sum => level0(7), carry => level1(7));
	FA8 : FullAdder port map( x => K(24), y => K(25), c => K(26), sum => level0(8), carry => level1(8));
	FA9 : FullAdder port map( x => K(27), y => K(28), c => K(29), sum => level0(9), carry => level1(9));
	HA1 : HalfAdder port map( x => K(30), y => K(31), sum => level0(10), carry => level1(10));
	
	FA10: FullAdder port map( x => level0(0), y => level0(1), c =>level0(2), sum=> level0(11),carry => level1(11));
	FA11: FullAdder port map( x => level0(3), y => level0(4), c =>level0(5), sum=> level0(12),carry => level1(12));
	FA12: FullAdder port map( x => level0(6), y => level0(7), c =>level0(8), sum=> level0(13),carry => level1(13));
	HA2 : HalfAdder port map( x => level0(9), y => level0(10), sum => level0(14), carry => level1(14));

	FA13: FullAdder port map( x => level1(0), y => level1(1), c =>level1(2), sum=> level1(15),carry => level2(0));
	FA14: FullAdder port map( x => level1(3), y => level1(4), c =>level1(5), sum=> level1(16),carry => level2(1));
	FA15: FullAdder port map( x => level1(6), y => level1(7), c =>level1(8), sum=> level1(17),carry => level2(2));
	HA3 : HalfAdder port map( x => level1(9), y => level1(10), sum => level1(18), carry => level2(3));
	
	FA16: FullAdder port map( x => level0(11), y => level0(12), c =>level0(13), sum=> level0(15),carry => level1(19));
	HA4 : HalfAdder port map( x => level0(14), y => level0(15), sum=> ans(0),carry => level1(20));
	
	FA17 : FullAdder port map( x => level1(11), y => level1(12), c => level1(13), sum => level1(21), carry => level2(4));
	FA18 : FullAdder port map( x => level1(14), y => level1(15), c => level1(16), sum => level1(22), carry => level2(5));
	FA19 : FullAdder port map( x => level1(17), y => level1(18), c => level1(19), sum => level1(23), carry => level2(6));
	FA20 : FullAdder port map( x => level1(20), y => level1(21), c => level1(22), sum => level1(24), carry => level2(7));
	HA5  : HalfAdder port map( x => level1(23), y => level1(24), sum => ans(1), carry => level2(8));
	
	FA21 : FullAdder port map( x => level2(0) , y => level2(1), c => level2(2), sum => level2(9), carry => level3(0));
	FA22 : FullAdder port map( x => level2(3) , y => level2(4), c => level2(5), sum => level2(10), carry => level3(1));
	FA23 : FullAdder port map( x => level2(6) , y => level2(7), c => level2(8), sum => level2(11), carry => level3(2));
	FA24 : FullAdder port map( x => level2(9) , y => level2(10), c => level2(11), sum => ans(2), carry => level3(3));
	
	FA25 : FullAdder port map( x => level3(0), y => level3(1), c => level3(2), sum => level3(4), carry => level4(0)); 
	HA6  : HalfAdder port map( x => level3(3), y => level3(4), sum => ans(3), carry => level4(1));
	
	HA7  : HalfAdder port map( x => level4(0), y => level4(1), sum => ans(4), carry => ans(5));
	
end Behavioral;