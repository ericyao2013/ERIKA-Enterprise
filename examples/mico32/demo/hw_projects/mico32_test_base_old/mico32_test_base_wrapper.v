`timescale 1 ns / 1 ns

`define BOARD_FREQ 25
`define PLATFORM_FREQ 25
// Define Wrapping Module
module mico32_test_base_wrapper( 
                            clk_i,reset_n
                            , sramsram_wen
                            , sramsram_data
                            , sramsram_addr
                            , sramsram_csn
                            , sramsram_be
                            , sramsram_oen
                            , SPIFlashCEJ
                            , SPIFlashSCK
                            , SPIFlashSI
                            , SPIFlashSO
                            , SPIFlashWPJ
                            , sdramsdr_DQ
                            , sdramsdr_A
                            , sdramsdr_BA
                            , sdramsdr_CKE
                            , sdramsdr_CSn
                            , sdramsdr_RASn
                            , sdramsdr_CASn
                            , sdramsdr_WEn
                            , sdramsdr_DQM
                            , sdramsdr_CLK
                            , uartSIN
                            , uartSOUT
                            , net_spiMISO_MASTER
                            , net_spiMOSI_MASTER
                            , net_spiSS_N_MASTER
                            , net_spiSCLK_MASTER
                            , camera_i2cSDA
                            , camera_i2cSCL
                            , rtc_i2cSDA
                            , rtc_i2cSCL
                            , lcd_gpioPIO_IO
                            , misc_gpioPIO_BOTH_IN
                            , misc_gpioPIO_BOTH_OUT
                            , serparioSER_IN
                            , serparioSER_OUT
                            , serparioSH_CLK
                            , serparioSTORE
                            , serparioOUT_EN
                            , ext_spiMISO_MASTER
                            , ext_spiMOSI_MASTER
                            , ext_spiSS_N_MASTER
                            , ext_spiSCLK_MASTER
                            , spi_flash_hold_out
                            , SPIFlash_bootCEJ
                            , SPIFlash_bootSCK
                            , SPIFlash_bootSI
                            , SPIFlash_bootSO
			    , cameracam_y
			    , cameracam_pclk
			    , cameracam_hsync
			    , cameracam_vsync
			    , cameracam_mclk
			    , cameracam_rst
			    , cameracam_enb
                            );
    
    // Inputs
    input clk_i;
    input reset_n;
    input SPIFlashSO;
    input uartSIN;
    input net_spiMISO_MASTER;
    input [3:0] misc_gpioPIO_BOTH_IN;
    input serparioSER_IN;
    input        ext_spiMISO_MASTER;
    input SPIFlash_bootSO;

    // Outputs
    output       sramsram_wen;
    output [17:0] sramsram_addr;
    output        sramsram_csn;
    output [1:0]  sramsram_be;
    output        sramsram_oen;
    output        SPIFlashCEJ;
    output        SPIFlashSCK;
    output        SPIFlashSI;
    output        SPIFlashWPJ;
    output [12:0] sdramsdr_A;
    output [1:0]  sdramsdr_BA;
    output        sdramsdr_CKE;
    output        sdramsdr_CSn;
    output        sdramsdr_RASn;
    output        sdramsdr_CASn;
    output        sdramsdr_WEn;
    output [1:0]  sdramsdr_DQM;
    output        sdramsdr_CLK;
    output        uartSOUT;
    output        net_spiMOSI_MASTER;
    output [0:0]  net_spiSS_N_MASTER;
    output        net_spiSCLK_MASTER;
    output [2:0]  misc_gpioPIO_BOTH_OUT;
    output        serparioSER_OUT;
    output        serparioSH_CLK;
    output        serparioSTORE;
    output        serparioOUT_EN;
    output        ext_spiMOSI_MASTER;
    output [2:0]  ext_spiSS_N_MASTER;
    output        ext_spiSCLK_MASTER;
    output        SPIFlash_bootCEJ;
    output        SPIFlash_bootSCK;
    output        SPIFlash_bootSI;
    output        spi_flash_hold_out;
    output        cameracam_mclk;
    output        cameracam_rst;
    output        cameracam_enb;
    
    // Input
    input [7:0]   cameracam_y;
    input         cameracam_pclk;
    input         cameracam_hsync;
    input         cameracam_vsync;
    
    // Bidirs
    inout [15:0]  sramsram_data;
    inout [15:0]  sdramsdr_DQ;
    inout         camera_i2cSDA;
    inout         camera_i2cSCL;
    inout         rtc_i2cSDA;
    inout         rtc_i2cSCL;
    inout [8:0]  lcd_gpioPIO_IO;
    
    wire          sram_addr_0;

    wire          main_clock;
    wire          main_reset_n;

    pmi_pll_fp #(.pmi_freq_clki(`BOARD_FREQ),
                 .pmi_freq_clkop(`PLATFORM_FREQ),
                 .pmi_freq_clkos(`PLATFORM_FREQ),
                 .pmi_freq_clkok(`PLATFORM_FREQ),
                 .pmi_family(`LATTICE_FAMILY),
                 .pmi_phase_adj(0),
                 .pmi_duty_cycle(50),
                 .pmi_clkfb_source("CLKOP"),
                 .module_type("pmi_pll"))
    platform_pll (.CLKI(clk_i),
                  .CLKFB(main_clock),
                  .RESET(!reset_n),
                  .CLKOP(main_clock),
                  .CLKOS(),
                  .CLKOK(),
                  .CLKOK2(),
                  .LOCK(main_reset_n)
                  );

    test_base base_platform (
                               .clk_i(main_clock), 
                               .reset_n(main_reset_n), 
                               .sramsram_wen(sramsram_wen), 
                               .sramsram_data(sramsram_data), 
                               .sramsram_addr({sramsram_addr[17:0],sram_addr_0}),
                               .sramsram_csn(sramsram_csn), 
                               .sramsram_be(sramsram_be), 
                               .sramsram_oen(sramsram_oen), 
                               .SPIFlashCEJ(SPIFlashCEJ), 
                               .SPIFlashSCK(SPIFlashSCK), 
                               .SPIFlashSI(SPIFlashSI), 
                               .SPIFlashSO(SPIFlashSO), 
                               .sdramsdr_DQ(sdramsdr_DQ), 
                               .sdramsdr_A(sdramsdr_A), 
                               .sdramsdr_BA(sdramsdr_BA), 
                               .sdramsdr_CKE(sdramsdr_CKE), 
                               .sdramsdr_CSn(sdramsdr_CSn), 
                               .sdramsdr_RASn(sdramsdr_RASn), 
                               .sdramsdr_CASn(sdramsdr_CASn), 
                               .sdramsdr_WEn(sdramsdr_WEn), 
                               .sdramsdr_DQM(sdramsdr_DQM), 
                               .sdramsdr_CLK(sdramsdr_CLK), 
                               .uartSIN(uartSIN), 
                               .uartSOUT(uartSOUT), 
                               .net_spiMISO_MASTER(net_spiMISO_MASTER), 
                               .net_spiMOSI_MASTER(net_spiMOSI_MASTER), 
                               .net_spiSS_N_MASTER(net_spiSS_N_MASTER), 
                               .net_spiSCLK_MASTER(net_spiSCLK_MASTER), 
                               .camera_i2cSDA(camera_i2cSDA), 
                               .camera_i2cSCL(camera_i2cSCL), 
                               .rtc_i2cSDA(rtc_i2cSDA), 
                               .rtc_i2cSCL(rtc_i2cSCL), 
                               .lcd_gpioPIO_IO(lcd_gpioPIO_IO), 
                               .misc_gpioPIO_BOTH_IN({~misc_gpioPIO_BOTH_IN[1:0],misc_gpioPIO_BOTH_IN}), 
                               .misc_gpioPIO_BOTH_OUT(misc_gpioPIO_BOTH_OUT), 
                               .serparioSER_IN(serparioSER_IN),
                               .serparioSER_OUT(serparioSER_OUT),
                               .serparioSH_CLK(serparioSH_CLK),
                               .serparioSTORE(serparioSTORE),
                               .serparioOUT_EN(serparioOUT_EN),
                               .ext_spiMISO_MASTER(ext_spiMISO_MASTER), 
                               .ext_spiMOSI_MASTER(ext_spiMOSI_MASTER), 
                               .ext_spiSS_N_MASTER(ext_spiSS_N_MASTER), 
                               .ext_spiSCLK_MASTER(ext_spiSCLK_MASTER),
                               .SPIFlash_bootCEJ(SPIFlash_bootCEJ), 
                               .SPIFlash_bootSCK(SPIFlash_bootSCK), 
                               .SPIFlash_bootSI(SPIFlash_bootSI), 
                               .SPIFlash_bootSO(SPIFlash_bootSO),
			       .cameracam_y(cameracam_y),
			       .cameracam_pclk(cameracam_pclk),
			       .cameracam_hsync(cameracam_hsync),
			       .cameracam_vsync(cameracam_vsync),
			       .cameracam_mclk(cameracam_mclk),
			       .cameracam_rst(cameracam_rst),
			       .cameracam_enb(cameracam_enb)
                               );

    // Unblock SPI Flash
    assign spi_flash_hold_out = 1'b1;
    assign SPIFlashWPJ = 1'b1;
endmodule
