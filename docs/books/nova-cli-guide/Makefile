all: pdf

pdf:
	dotnet run --project ../../../e6502.Nova -c Release -- docs nova-cli-guide --repo ../../..

clean:
	rm -f nova-cli-guide.pdf

.PHONY: all pdf clean
