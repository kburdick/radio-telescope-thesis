######################################################
###
### Purpose: Radio Telescope IQ Data plotting Program
### Author: Kurt Burdick
###
### This R script conducts the base R function fft() - "fast fourier transform" 
###   on the iq data text file (csv) sampled by an SA-44B USB Spectrum Analyzer. 
### 
### The result of the fft gives the real and imaginary portion of the 
###   quadrature data signals and allows it to be plotted in R, for further 
###   scientific analysis.
###
###
### date: 2/8/2020
### modified: 4/15/2020 
#####################################################


# Load the required packages to read in a csv/txt file
library(readr)
library(ggplot2)
library(dplyr)


# PATH to the file produced by the C++ program that runs the spectrum analyzer
iq_file <- "C:/Users/burdi/Documents/4-19-FALL-Ithaca/PHYS-Research/Radio_telescope/realTimeTest/realTimeTest/iq-data-test.txt"

# read in the text file (csv) to a data variable
radio_Data <- read.csv(iq_file)

rowCount <- nrow(radio_Data)
rowCount

# check the data import
radio_Data


# conduct the Fast Fourier Transform for IQ data conversion
FFT_result <- fft(radio_Data$iq.data)

# check fft data in console for desired result
FFT_result

rowCount <- c(1:nrow(radio_Data))
rowCount
    
# take the magnitude square of the fft
spectrum <- abs( (FFT_result) * ( FFT_result))  
spectrum


# create a data frame for the data plot in ggplot
newData <- data.frame(newData)
newData

# need this?
radio_Data

# spectrum after fft conversion to magnitude
spectrum_plot <- ggplot(newData, aes(x = rowCount, y = spectrum)) + geom_line() + 
  ggtitle("Spectrum Radio Data Plot") + xlab("Span Data") + ylab("Spectrum Data")

spectrum_plot


# original IQ data plot
qplot( Re(FFT_result), Im(FFT_result), geom = "line")
