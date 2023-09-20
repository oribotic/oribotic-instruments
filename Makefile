
.PHONY: oi-zip

##### oi puredata abstraction library

# override on commandline via: make oi-zip OI_ZIP_VER=0.3.0
OI_ZIP_VER ?= git

# create a zip of just the oi abstraction library
oi-zip:
	rm -f *.zip puredata/*.zip
	cd puredata && zip -r oi.zip oi -x '**/.*' -x '**/__MACOSX'
	mv puredata/oi.zip oi-$(OI_ZIP_VER).zip
