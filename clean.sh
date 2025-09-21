#!/bin/sh
for i in raknet .; do
	rm -rf $i/*.make $i/Makefile $i/obj $i/bin $i/lib
done
