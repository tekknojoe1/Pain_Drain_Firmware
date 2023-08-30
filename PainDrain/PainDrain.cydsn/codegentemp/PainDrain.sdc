# THIS FILE IS AUTOMATICALLY GENERATED
# Project: C:\Users\zunis\Desktop\WorkPainDrain\Pain_Drain_Firmware\PainDrain\PainDrain.cydsn\PainDrain.cyprj
# Date: Wed, 30 Aug 2023 23:30:01 GMT
#set_units -time ns
create_clock -name {CyClk_LF} -period 30517.578125 -waveform {0 15258.7890625} [list [get_pins {ClockBlock/lfclk}]]
create_clock -name {CyILO} -period 31250 -waveform {0 15625} [list [get_pins {ClockBlock/ilo}]]
create_clock -name {CyFLL} -period 20 -waveform {0 10} [list [get_pins {ClockBlock/fll}]]
create_clock -name {CyClk_HF0} -period 20 -waveform {0 10} [list [get_pins {ClockBlock/hfclk_0}]]
create_clock -name {CyClk_Fast} -period 20 -waveform {0 10} [list [get_pins {ClockBlock/fastclk}]]
create_clock -name {CyClk_Peri} -period 40 -waveform {0 20} [list [get_pins {ClockBlock/periclk}]]
create_generated_clock -name {CyClk_Slow} -source [get_pins {ClockBlock/periclk}] -edges {1 2 3} [list [get_pins {ClockBlock/slowclk}]]
create_generated_clock -name {Clock_2} -source [get_pins {ClockBlock/periclk}] -edges {1 25 51} [list [get_pins {ClockBlock/ff_div_11}]]
create_generated_clock -name {Clock_5} -source [get_pins {ClockBlock/periclk}] -edges {1 25 51} [list [get_pins {ClockBlock/ff_div_12}]]
create_generated_clock -name {ADC_intSarClock} -source [get_pins {ClockBlock/periclk}] -edges {1 25 51} [list [get_pins {ClockBlock/ff_div_49}]]
create_generated_clock -name {Clock_6} -source [get_pins {ClockBlock/periclk}] -edges {1 25 51} [list [get_pins {ClockBlock/ff_div_13}]]
create_generated_clock -name {Clock_4} -source [get_pins {ClockBlock/periclk}] -edges {1 5 9} [list [get_pins {ClockBlock/ff_div_15}]]
create_generated_clock -name {SPI_SCBCLK} -source [get_pins {ClockBlock/periclk}] -edges {1 5 9} [list [get_pins {ClockBlock/ff_div_2}]]
create_generated_clock -name {Clock_7} -source [get_pins {ClockBlock/periclk}] -edges {1 25 51} [list [get_pins {ClockBlock/ff_div_18}]]
create_generated_clock -name {Clock_3} -source [get_pins {ClockBlock/periclk}] -edges {1 25 51} [list [get_pins {ClockBlock/ff_div_20}]]
create_generated_clock -name {Clock_1} -source [get_pins {ClockBlock/periclk}] -edges {1 5 9} [list [get_pins {ClockBlock/ff_div_26}]]
create_clock -name {CyPeriClk_App} -period 40 -waveform {0 20} [list [get_pins {ClockBlock/periclk_App}]]
create_clock -name {CyClk_HF1} -period 20 -waveform {0 10} [list [get_pins {ClockBlock/hfclk_1}]]
create_clock -name {CyIMO} -period 125 -waveform {0 62.5} [list [get_pins {ClockBlock/imo}]]


# Component constraints for C:\Users\zunis\Desktop\WorkPainDrain\Pain_Drain_Firmware\PainDrain\PainDrain.cydsn\TopDesign\TopDesign.cysch
# Project: C:\Users\zunis\Desktop\WorkPainDrain\Pain_Drain_Firmware\PainDrain\PainDrain.cydsn\PainDrain.cyprj
# Date: Wed, 30 Aug 2023 23:29:43 GMT
