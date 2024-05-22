----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 17.04.2024 10:45:40
-- Design Name: 
-- Module Name: Oscillator - Structural
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

entity Oscillator is
  Port ( EN_O: in STD_LOGIC;
         Q_O: out STD_LOGIC);
end Oscillator;

architecture Structural of Oscillator is
component XOR_EN is
    Port ( EN : in STD_LOGIC;
           A : in STD_LOGIC;
           Q : out STD_LOGIC);
end component;

attribute dont_touch:string;
attribute dont_touch of structural: architecture is "true";

signal Q_INV: std_Logic;
begin

    XOR_1: XOR_EN port map (EN=>EN_O, A=>Q_INV, Q=>Q_INV);
    Q_O <= Q_INV;
end Structural;
