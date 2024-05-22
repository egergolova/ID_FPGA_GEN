----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 18.04.2024 14:27:24
-- Design Name: 
-- Module Name: multiple_structs - Behavioral
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

entity multiple_structs is
    Port ( EN : in STD_LOGIC;
           leds: out STD_LOGIC_VECTOR(15 downto 0));
end multiple_structs;

architecture Structural of multiple_structs is
component CommonF is
   Port (EN_C: in STD_LOGIC;
         Q_Com: out STD_LOGIC);
end component;

attribute dont_touch:string;
attribute dont_touch of structural: architecture is "true";
begin
    Gener:
        for I in 0 to 15 generate
            contrl_led: CommonF port map (EN_C=>EN, Q_Com=>leds(I));
    end generate Gener;
end Structural;
