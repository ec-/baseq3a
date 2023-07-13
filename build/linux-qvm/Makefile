include config.mk

srcs = $(foreach file,$1,$(if $(patsubst %.asm,,$(file)),$2/$(file).asm,$(file)))
qa_asm = $(call srcs,$(QA_SRC),vm/game)
cg_asm = $(call srcs,$(CG_SRC),vm/cgame)
ui_asm = $(call srcs,$(UI_SRC),vm/ui)

all: dirs $(PK3)

dirs:
	mkdir -p vm/game vm/cgame vm/ui

.PHONY: all dirs

$(PK3): vm/qagame.qvm vm/cgame.qvm vm/ui.qvm
	$(7Z) $@ vm/*.qvm vm/*.jts ../../assets/*

cc = cd vm/$1 && $(Q3LCC) $2 -o $(notdir $@).tmp ../../$< && mv $(notdir $@).tmp $(notdir $@)
qa_cc = $(call cc,game,$(QA_CFLAGS))
cg_cc = $(call cc,cgame,$(CG_CFLAGS))
ui_cc = $(call cc,ui,$(UI_CFLAGS))

vm/qagame.qvm: $(qa_asm)
	$(Q3ASM) -o $@ $(qa_asm)

vm/cgame.qvm: $(cg_asm)
	$(Q3ASM) -o $@ $(cg_asm)

vm/ui.qvm: $(ui_asm)
	$(Q3ASM) -o $@ $(ui_asm)

vm/game/%.asm: $(QADIR)/%.c; $(qa_cc)
vm/cgame/%.asm: $(QADIR)/%.c; $(cg_cc)
vm/cgame/%.asm: $(CGDIR)/%.c; $(cg_cc)
vm/ui/%.asm: $(QADIR)/%.c; $(ui_cc)
vm/ui/%.asm: $(UIDIR)/%.c; $(ui_cc)

clean:
	$(RM) -rf *.pk3 vm/ ../cgame/*.asm ../game/*.asm ../ui/*.asm
