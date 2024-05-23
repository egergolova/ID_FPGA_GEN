----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 22.04.2024 23:53:50
-- Design Name: 
-- Module Name: leds - Architectural
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
use IEEE.NUMERIC_STD.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

--Port A- Allow generation of bistable element. From static state ( either 0 or 1) to oscillation
--Port Gen_rand - Allow generation of random number.
--Port clock - just a clock from quartz element on FPGA)
-- Leds- output, which goes to leds
entity leds is
  Port (--A: in std_logic;
  Gen_rand: in std_logic;
  clock: in std_logic;
  Leds: out std_logic_vector(15 downto 0);
  Rand_Num: out std_logic_vector(15 downto 0));
end leds;

architecture behavioral of leds is

component led_contr is
  Port (Contr:in std_logic;
Gen_rand: in std_logic;
Led: out std_logic;
 Outp: out std_logic_vector(15 downto 0) );
end component;

component Counter is
  Port (
  Cl:in std_logic;
  Allow: in std_logic;
  F:out std_logic);
end component;

component count_clock is
  Port ( 
  init_st: in std_logic;
  quartz: in std_logic;
  allow: out std_logic);
end component;

component Num_rem_reg is
  Port (Allow: in std_logic;
  Num: in std_logic_vector(15 downto 0);
  Res:out std_logic_vector(15 downto 0) );
end component;

attribute dont_touch:string;
attribute dont_touch of behavioral: architecture is "true";
signal S: std_logic;
signal Ind: integer;
type   numbers is array(0 to 15) of std_logic_vector(15 downto 0);
signal Nums: numbers;
begin
    U0: count_clock port map(init_st=>Gen_rand,quartz=>clock, allow=>S);
    gen:
    for I in 0 to 15 generate
        U: led_contr port map(Contr=>S,Gen_rand=>S, Led=>Leds(I), Outp=>Nums(I));
    end generate gen;
    
    -- getting the lower part of the 0's random number to choose, which of the 16
    -- shall be chosen
    Ind<=to_integer(unsigned(Nums(0) and "0000000000001111"));
    Rand_Num<=Nums(Ind);
end behavioral;
