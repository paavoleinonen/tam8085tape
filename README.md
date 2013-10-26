tam8085tape
===========

Utility for generating tape files for TAM8085 single board computer.

Usage: tam8085tape <input> <output>

Input is a raw byte stream. If size of input is not a multiple of 256 bytes zero-bytes are added to complete full page.

Output is signed 8 bit samples with 32000 sps. Use sox or other utility to convert them to container of your liking. See makefile for example.
