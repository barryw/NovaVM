GUIDE=novabasic_user_guide

.PHONY: all pdf clean

all: pdf

pdf:
	latexmk -pdf -interaction=nonstopmode $(GUIDE).tex

clean:
	latexmk -C
