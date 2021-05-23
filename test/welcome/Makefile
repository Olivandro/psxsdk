PROJNAME = welcome
PROJ_LIBS = -lmeidogte

include ../project.mk

$(PROJNAME)_extra:
	$(MAKE_COMMAND) -C data
	mkdir -p cd_root
	cp data/*.tim cd_root

$(PROJNAME)_clean_extra:
	$(MAKE_COMMAND) -C data clean
