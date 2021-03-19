cd
cd $1
if test -e Blocks; then
	rm -r Blocks
fi
if test -e Files; then
	rm -r Files
fi
cd Metadata
if test -e Bitmap.bin; then
	rm Bitmap.bin
fi