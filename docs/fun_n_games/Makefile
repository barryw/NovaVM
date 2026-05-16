GUIDE = nova_fun_n_games

all: pdf

pdf:
	latexmk -pdf -interaction=nonstopmode $(GUIDE).tex

clean:
	latexmk -C

.PHONY: all pdf clean
