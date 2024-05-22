----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 17.04.2024 11:29:10
-- Design Name: 
-- Module Name: CommonF - Structural
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

entity CommonF is
   Port (EN_C: in STD_LOGIC;
         Q_Com: out STD_LOGIC);
end CommonF;

architecture Structural of CommonF is
component Oscillator is
  Port ( EN_O: in STD_LOGIC;
         Q_O: out STD_LOGIC);
end component;

component Counter is
    Port ( O : in STD_LOGIC;
           Q_C : out STD_LOGIC);
end component;

signal Oscil_Count: STD_LOGIC;

attribute dont_touch:string;
attribute dont_touch of structural: architecture is "true";
begin
    Oscillator_1: Oscillator port map (EN_O=>EN_C, Q_O=>Q_Com);
--    Counter_1: Counter port map (O=>Oscil_Count, Q_C=>Q_Com);
end Structural;
