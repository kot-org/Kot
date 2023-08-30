TARGET ?= liamd

init:
	@ xbstrap init .
	@ xbstrap install-tool --all

build:
	@ cd target && make $(TARGET)/build $(word 2,$(MAKECMDGOALS))

run:
	@ cd target && make $(TARGET)/run