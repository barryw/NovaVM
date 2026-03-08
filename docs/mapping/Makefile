GUIDE = mapping_the_novavm

all: pdf

pdf:
	latexmk -pdf -interaction=nonstopmode $(GUIDE).tex

clean:
	latexmk -C

.PHONY: all pdf clean
