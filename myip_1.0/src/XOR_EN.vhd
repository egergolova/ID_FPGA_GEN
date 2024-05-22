----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 17.04.2024 10:52:05
-- Design Name: 
-- Module Name: XOR_EN - Behavioral
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

entity XOR_EN is
    Port ( EN : in STD_LOGIC;
           A : in STD_LOGIC;
           Q : out STD_LOGIC);
end XOR_EN;

architecture Behavioral of XOR_EN is
attribute dont_touch:string;
attribute dont_touch of Behavioral: architecture is "true";
begin
 Q <= EN xor A;   
end Behavioral;
