BOOK = ndk-reference
ROOT = ../../..
RUNTIME_DIR = $(ROOT)/software/runtime/asm
NOVA_CLI = dotnet run --project $(ROOT)/e6502.Nova -c Release --
NOVA_CLI_SOURCES = $(ROOT)/e6502.Nova/Program.cs $(ROOT)/e6502.Nova/BuildTools.cs
RUNTIME_SOURCES = $(wildcard $(RUNTIME_DIR)/*.inc $(RUNTIME_DIR)/*.s)
LIBRARY_CHAPTERS = $(wildcard libraries/*.tex)
AUTHORED_CHAPTERS = $(wildcard chapters/*.tex) $(LIBRARY_CHAPTERS)
DIAGRAM_SOURCES = $(wildcard diagrams/*.mmd)
DIAGRAM_SVGS = $(patsubst diagrams/%.mmd,build/diagrams/%.svg,$(DIAGRAM_SOURCES))
GENERATED = generated/library-reference.tex generated/ndk-api.json

all: pdf

pdf: $(BOOK).pdf

$(BOOK).pdf: $(BOOK).tex ../common/nova-book.cls cover.tex frontmatter/preface.tex backmatter/generated-reference.tex $(AUTHORED_CHAPTERS) $(GENERATED) $(DIAGRAM_SVGS)
	TEXINPUTS=../common//: latexmk -pdf -shell-escape -interaction=nonstopmode -halt-on-error $(BOOK).tex

generated/library-reference.tex generated/ndk-api.json: $(NOVA_CLI_SOURCES) $(RUNTIME_SOURCES)
	mkdir -p generated
	$(NOVA_CLI) codegen ndk-reference --runtime-dir $(RUNTIME_DIR) --tex generated/library-reference.tex --json generated/ndk-api.json

build/diagrams/%.svg: diagrams/%.mmd
	@command -v mmdc >/dev/null || { echo "mmdc is required to render Mermaid diagrams"; exit 1; }
	mkdir -p build/diagrams
	@if [ -x "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome" ]; then \
		PUPPETEER_EXECUTABLE_PATH="/Applications/Google Chrome.app/Contents/MacOS/Google Chrome" mmdc -i $< -o $@ -b transparent; \
	else \
		mmdc -i $< -o $@ -b transparent; \
	fi

clean:
	latexmk -C $(BOOK).tex
	rm -rf build generated

.PHONY: all pdf clean
