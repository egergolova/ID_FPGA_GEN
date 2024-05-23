----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 17.04.2024 11:15:55
-- Design Name: 
-- Module Name: Counter - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
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
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity Counter is
    Port ( O : in STD_LOGIC;
           Q_C : out STD_LOGIC);
end Counter;

architecture Behavioral of Counter is
signal Counter_Value: integer := 0;
signal is1G: STD_LOGIC;

attribute dont_touch:string;
attribute dont_touch of Behavioral: architecture is "true";
begin
process (O, Counter_Value, is1G)
begin
--INC
    if rising_edge(O) then
        Counter_Value <= Counter_Value + 1;
    end if;
--CMP
    if Counter_Value = 100000000 then
        Counter_Value <= 0;
    end if;
   
    if Counter_Value = 0 then
        is1G <= not is1G;
    end if;
end process;
Q_C <= is1G;
end Behavioral;
